/*
 * Samsung Exynos SoC series NPU driver
 *
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/version.h>
#include <linux/atomic.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <soc/samsung/exynos/debug-snapshot.h>
#include "npu-common.h"
#include "npu-debug.h"
#include "npu-util-memdump.h"
#include "npu-util-regs.h"
#include "npu-memory.h"
#include "npu-scheduler.h"
#include "npu-ver.h"

/* Maker structure for each memdump */
struct mem_dump_stats {
	u32 readpos;
};

/* Global data storage for memory dump - Initialized from probe function */
static struct {
	atomic_t registered;
	struct npu_iomem_area	*tcu_sram;
	struct npu_iomem_area	*idp_sram;
} npu_memdump;

void *npu_ram_dump_addr;

////////////////////////////////////////////////////////////////////////////////////////
// Firmware log(on SRAM) dump

/* Position of log buffer from start of NPU_IOMEM_SRAM_START */
static const u32 MEM_LOG_OFFSET		= 0x78000;
static const u32 MEM_LOG_SIZE		= 0x8000;
static const char *FW_MEM_LOG_NAME	= "fw-log-SRAM";
#if IS_ENABLED(CONFIG_EXYNOS_NPU_DEBUG_SRAM_DUMP)
static const char *TCU_SRAM_DUMP_SYSFS_NAME	= "SRAM-TCU";
static const char *IDP_SRAM_DUMP_SYSFS_NAME	= "SRAM-IDP";
#endif

#define NPU_SRAM_DUMP_BUF (1024 * 1024)
static int fw_mem_log_open_fops(struct inode *inode, struct file *file)
{
	struct mem_dump_stats *stats;

	stats = kmalloc(sizeof(*stats), GFP_KERNEL);
	if (!stats) {
		npu_err("could not allocate memory for stat object.\n");
		return -ENOMEM;
	}
	/* Save it as a private data */
	memset(stats, 0, sizeof(*stats));
	file->private_data = stats;
	return 0;
}

static ssize_t fw_mem_log_read_fops(struct file *file, char __user *buf, size_t size, loff_t *off)
{
	char *p;
	char ch;
	size_t copied = 0;
	struct mem_dump_stats *stats;

	BUG_ON(!file->private_data);

	if (!npu_memdump.tcu_sram ||
		!npu_memdump.tcu_sram->vaddr) {
		npu_err("No TCUSRAM area\n");
		return 0;
	}

	stats = (struct mem_dump_stats *)file->private_data;

	if (stats->readpos >= MEM_LOG_SIZE)
		return 0;	/* EOF */

	p = npu_memdump.tcu_sram->vaddr + MEM_LOG_OFFSET + stats->readpos;
	if (*p == '\0')
		return 0;	/* No more data */

	while ((*p != '\0') && (stats->readpos < MEM_LOG_SIZE) && (copied < size)) {
		/* Copy to user buffer */
		get_user(ch, p);
		put_user(ch, &buf[copied]);
		/* Adjust pointer */
		p++;
		stats->readpos++;
		copied++;
	}
	return copied;
}

static int fw_mem_log_close_fops(struct inode *inode, struct file *file)
{
	struct mem_dump_stats *stats;

	stats = (struct mem_dump_stats *)file->private_data;
	if (stats)
		kfree(stats);
	return 0;
}

int ram_dump_fault_listner(struct npu_device *npu)
{
	int ret = 0;
	struct npu_system *system = &npu->system;
	u32 *tcu_dump_addr = NULL;
	u32 *idp_dump_addr = NULL;
	struct npu_iomem_area *tcusram;
	struct npu_iomem_area *idpsram;

	tcusram = npu_get_io_area(system, "tcusram");
	if (tcusram) {
		tcu_dump_addr = kzalloc(tcusram->size, GFP_ATOMIC);
		if (tcu_dump_addr) {
			memcpy_fromio(tcu_dump_addr, tcusram->vaddr, tcusram->size);
			pr_err("NPU TCU SRAM dump - %pK / %llu\n", tcu_dump_addr, tcusram->size);
		} else {
			pr_err("tcu_dump_addr is NULL\n");
			ret = -ENOMEM;
			goto exit_err;
		}
	}
	idpsram = npu_get_io_area(system, "idpsram");
	if (idpsram) {
		idp_dump_addr = kzalloc(idpsram->size, GFP_ATOMIC);
		if (idp_dump_addr) {
			memcpy_fromio(idp_dump_addr, idpsram->vaddr, idpsram->size);
			pr_err("NPU IDP SRAM dump - %pK / %llu\n", idp_dump_addr, idpsram->size);
		} else {
			pr_err("idp_dump_addr is NULL\n");
			ret = -ENOMEM;
			goto exit_err;
		}
	}
	/* tcu_dump_addr and idp_dump_addr are not freed, because we expect them left on ramdump */

	return ret;

exit_err:
	if (tcu_dump_addr)
		kfree(tcu_dump_addr);
	if (idp_dump_addr)
		kfree(idp_dump_addr);
	return ret;
}

void npu_util_dump_handle_nrespone(struct npu_system *system)
{
	int i = 0;
	struct npu_device *device = NULL;

	//WARN_ON(system);

	device = container_of(system, struct npu_device, system);

	/* retry 5 times for checking PC value move or not. */
	for (i = 0; i < 5; i++)
		npu_soc_status_report(&device->system);

	npu_ver_dump(device);
	// npu_reg_dump(device);
	npu_memory_dump(&device->system.memory);
	npu_scheduler_param_handler_dump();
	session_fault_listener();

	/* trigger a wdreset to analyse s2d dumps in this case */
	dbg_snapshot_expire_watchdog();
}

int npu_util_dump_handle_error_k(struct npu_device *device)
{
	int ret = 0;
	int i = 0;

	fw_will_note(FW_LOGSIZE);
	for (i = 0; i < 5; i++)
		npu_soc_status_report(&device->system);

	npu_ver_dump(device);
	// npu_reg_dump(device);
	npu_memory_dump(&device->system.memory);
	npu_scheduler_param_handler_dump();
	session_fault_listener();
	/* trigger a wdreset to analyse s2d dumps in this case */
	dbg_snapshot_expire_watchdog();

	return ret;
}

static const struct file_operations fw_mem_log_fops = {
	.owner = THIS_MODULE,
	.open = fw_mem_log_open_fops,
	.release = fw_mem_log_close_fops,
	.read = fw_mem_log_read_fops,
};

#if IS_ENABLED(CONFIG_EXYNOS_NPU_DEBUG_SRAM_DUMP)
static unsigned long __npu_access_ok(const void __user *addr, unsigned long size)
{
	return access_ok(addr, size);
}

static ssize_t __npu_sram_dump_write(
		const char *inbuf, size_t count, size_t *offset, size_t *size)
{
	int ret;

	/* Parsing */
	ret = sscanf(inbuf, "%zx.%zx", offset, size);
	if (ret != 2) {
		npu_err("Command line parsing error.\n");
		return -EINVAL;
	}

	npu_info("SRAM dump setting : offset 0x%zx, size 0x%zx bytes\n", *offset, *size);
	barrier();

	return count;
}

static ssize_t __npu_sram_dump_read(
		char __user *outbuf, size_t count, void __iomem *sram_vaddr, size_t sram_size)
{
	ssize_t	ret = 0;
	u32	*tmp_buf = NULL;
	size_t	i;

	tmp_buf = kzalloc(sram_size, GFP_KERNEL);
	if (!tmp_buf) {
		ret = -ENOMEM;
		goto err_exit;
	}

	memcpy_fromio((void *)tmp_buf, sram_vaddr, sram_size);
	for (i = 0; i < sram_size / sizeof(u32); i += 4) {
		npu_dbg("SRAM %pK -> %pK : %08x %08x %08x %08x\n",
				sram_vaddr + i * sizeof(u32), tmp_buf + i,
				*(tmp_buf + i), *(tmp_buf + i + 1),
				*(tmp_buf + i + 2), *(tmp_buf + i + 3));
	}

	if (__npu_access_ok(outbuf, sram_size)) {
		ret = copy_to_user(outbuf, (char *)tmp_buf, min(sram_size, count) / sizeof(char));
		if (ret) {
			npu_err("copy_to_user failed : %zd\n", ret);
			goto err_exit;
		}
		npu_info("ret value : %zd (%zd)\n", ret, sram_size);
		// return copied size (target size - uncopied size)
		ret = sram_size - ret;
	} else {
		npu_info("buffer inaccessible : 0x%pK (%zd)\n", outbuf, sram_size);
		// no copy, return target size
		ret = sram_size;
	}

err_exit:
	if (tmp_buf)
		kfree(tmp_buf);
	return ret;
}

#define DECLARE_NPU_SRAM_DUMP(__NPU_SRAM)	\
static size_t __NPU_SRAM##_sram_dump_offset;	\
static size_t __NPU_SRAM##_sram_dump_size;	\
static int __NPU_SRAM##_sram_dump_open(struct inode *inode, struct file *file)	\
{	\
	npu_info(#__NPU_SRAM " SRAM dump open\n");	\
	return 0;	\
}	\
static ssize_t __NPU_SRAM##_sram_dump_read(	\
		struct file *file, char __user *buf, size_t count, loff_t *offp)	\
{	\
	BUG_ON(!npu_memdump.__NPU_SRAM##_sram->vaddr);	\
	npu_dbg(#__NPU_SRAM " SRAM dump : va 0x%pK + %zx (pa 0x%x + %zx), 0x%zx bytes\n",	\
			npu_memdump.__NPU_SRAM##_sram->vaddr,	\
			__NPU_SRAM##_sram_dump_offset,		\
			npu_memdump.__NPU_SRAM##_sram->paddr,	\
			__NPU_SRAM##_sram_dump_offset,		\
			__NPU_SRAM##_sram_dump_size);	\
	return __npu_sram_dump_read(buf, count,	\
			npu_memdump.__NPU_SRAM##_sram->vaddr + __NPU_SRAM##_sram_dump_offset,	\
			__NPU_SRAM##_sram_dump_size);	\
}	\
static ssize_t __NPU_SRAM##_sram_dump_write(	\
		struct file *file, const char * __user userbuf, size_t count, loff_t *offp)	\
{	\
	unsigned long remain;	\
	char inbuf[32];	\
	remain = copy_from_user(inbuf, userbuf, min(ARRAY_SIZE(inbuf), count));	\
	if (remain) {	\
		npu_err("Invalid input data\n");	\
		return -EINVAL;	\
	}	\
	return __npu_sram_dump_write(	\
			inbuf, count,	\
			&__NPU_SRAM##_sram_dump_offset,	\
			&__NPU_SRAM##_sram_dump_size);	\
}	\
static int __NPU_SRAM##_sram_dump_close(struct inode *inode, struct file *file)	\
{	\
	npu_info(#__NPU_SRAM " SRAM dump close\n");	\
	return 0;	\
}	\
static const struct file_operations __NPU_SRAM##_sram_dump_fops = {	\
	.owner = THIS_MODULE,	\
	.open = __NPU_SRAM##_sram_dump_open,	\
	.release = __NPU_SRAM##_sram_dump_close,	\
	.read = __NPU_SRAM##_sram_dump_read,	\
	.write = __NPU_SRAM##_sram_dump_write,	\
} \

#define NPU_SRAM_DUMP_FOPS(__NPU_SRAM)	__NPU_SRAM##_sram_dump_fops

DECLARE_NPU_SRAM_DUMP(tcu);
DECLARE_NPU_SRAM_DUMP(idp);
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Lifecycle functions

int npu_util_memdump_probe(struct npu_system *system)
{
	struct npu_iomem_area *tcusram;
	struct npu_iomem_area *idpsram;

	BUG_ON(!system);



	atomic_set(&npu_memdump.registered, 0);
	tcusram = npu_get_io_area(system, "tcusram");
	npu_memdump.tcu_sram = tcusram;
	idpsram = npu_get_io_area(system, "idpsram");
	npu_memdump.idp_sram = idpsram;

	if (tcusram)
		probe_info("%s: paddr = %08llx\n", FW_MEM_LOG_NAME,
			tcusram->paddr + tcusram->size);
#if IS_ENABLED(CONFIG_EXYNOS_NPU_DEBUG_SRAM_DUMP)
	if (tcusram) {
		probe_info("%s: paddr = %08x\n", TCU_SRAM_DUMP_SYSFS_NAME,
			tcusram->paddr);
		tcu_sram_dump_size = tcusram->size;
	}
	if (idpsram) {
		probe_info("%s: paddr = %08x\n", IDP_SRAM_DUMP_SYSFS_NAME,
			idpsram->paddr);
		idp_sram_dump_size = idpsram->size;
	}
#endif

	return 0;
}

int npu_util_memdump_open(struct npu_system *system)
{
	int ret = 0;
	int loaded = 0;


	loaded = atomic_xchg(&npu_memdump.registered, 1);
	if (loaded == 0) {
		/* Initial load */
		npu_info("register on debugfs\n");
		ret = npu_debug_register(FW_MEM_LOG_NAME, &fw_mem_log_fops);
		if (ret) {
			npu_err("npu_debug_register error : ret = %d\n", ret);
			goto p_err;
		}
#if IS_ENABLED(CONFIG_EXYNOS_NPU_DEBUG_SRAM_DUMP)
		npu_info("register debugfs %s\n", TCU_SRAM_DUMP_SYSFS_NAME);
		ret = npu_debug_register(TCU_SRAM_DUMP_SYSFS_NAME, &NPU_SRAM_DUMP_FOPS(tcu));
		if (ret) {
			npu_err("%s npu_debug_register error : ret = %d\n", TCU_SRAM_DUMP_SYSFS_NAME, ret);
			goto p_err;
		}
		npu_info("register debugfs %s\n", IDP_SRAM_DUMP_SYSFS_NAME);
		ret = npu_debug_register(IDP_SRAM_DUMP_SYSFS_NAME, &NPU_SRAM_DUMP_FOPS(idp));
		if (ret) {
			npu_err("%s npu_debug_register error : ret = %d\n", IDP_SRAM_DUMP_SYSFS_NAME, ret);
			goto p_err;
		}
#endif
	}
	return 0;
p_err:
	npu_info("fail(%d)\n", ret);
	return ret;
}

int npu_util_memdump_close(struct npu_system *system)
{
	/* NOP */
	return 0;
}

int npu_util_memdump_start(struct npu_system *system)
{
	/* NOP */
	return 0;
}

int npu_util_memdump_stop(struct npu_system *system)
{
	/* NOP */
	return 0;
}
