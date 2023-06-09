/* SPDX-License-Identifier: GPL-2.0-or-later
 * sound/soc/samsung/vts/vts_dbg.h
 *
 * ALSA SoC - Samsung Abox Debug driver
 *
 * Copyright (c) 2019 Samsung Electronics Co. Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __SND_SOC_VTS_DEBUG_H
#define __SND_SOC_VTS_DEBUG_H

#include "vts.h"

/**
 * Initialize vts debug driver
 * @return	dentry of vts debugfs root directory
 */
extern struct dentry *vts_dbg_get_root_dir(void);

/* VTS dump magic value */
#define VTS_LOG_DUMP_MAGIC  "VTS-LOG"  // 0x474F4C2D535456ull, VTS-LOG */
#define VTS_SRAM_DUMP_MAGIC 0x4742442D535456ull  /* VTS-DBG */
#define VTS_SRAM_PCM_MAGIC  0x4D43502D535456ull  /* VTS-PCM */

#define DMA_TYPE (data->id ? "REC" : "TRI")

#define vts_info(fmt, args...) \
	pr_info("[%s]: " fmt, "VTS", ##args)

#define vts_err(fmt, args...) \
	pr_err("[%s]: " fmt, "VTS", ##args)

#define vts_warn(fmt, args...) \
	pr_warn("[%s]: " fmt, "VTS", ##args)

#define vts_dbg(fmt, args...)

#define vts_dev_info(dev, fmt, args...)	\
	do {	\
		struct vts_data *vdata;	\
		if (dev) {	\
			vdata = dev_get_drvdata(dev);	\
			vts_info(fmt, ##args);	\
			if (vdata && vdata->kernel_log_obj)	\
				if (vdata->kernel_log_obj->enabled &&	\
					vdata->kernel_log_obj->log_level \
					>= MEMLOG_LEVEL_INFO)	\
					memlog_write_printf(	\
					vdata->kernel_log_obj,	\
						MEMLOG_LEVEL_INFO,	\
						fmt, ##args);	\
		}	\
	} while (0)

#define vts_dev_dbg(dev, fmt, args...)	\
	do {	\
		struct vts_data *vdata;	\
		if (dev) {	\
			vdata = dev_get_drvdata(dev);	\
			vts_dbg(fmt, ##args);	\
			if (vdata && vdata->kernel_log_obj)	\
				if (vdata->kernel_log_obj->enabled &&	\
					vdata->kernel_log_obj->log_level \
					>= MEMLOG_LEVEL_DEBUG)	\
					memlog_write_printf(	\
					vdata->kernel_log_obj,	\
						MEMLOG_LEVEL_DEBUG,	\
						fmt, ##args);	\
		}	\
	} while (0)

#define vts_dev_warn(dev, fmt, args...)	\
	do {	\
		struct vts_data *vdata;	\
		if (dev) {	\
			vdata = dev_get_drvdata(dev);	\
			vts_warn(fmt, ##args);	\
			if (vdata && vdata->kernel_log_obj)	\
				if (vdata->kernel_log_obj->enabled &&	\
					vdata->kernel_log_obj->log_level \
					>= MEMLOG_LEVEL_CAUTION)	\
					memlog_write_printf(	\
					vdata->kernel_log_obj,	\
						MEMLOG_LEVEL_CAUTION,	\
						fmt, ##args);	\
		}	\
	} while (0)

#define vts_dev_err(dev, fmt, args...)	\
	do {	\
		struct vts_data *vdata;	\
		if (dev) {	\
			vdata = dev_get_drvdata(dev);	\
		vts_err(fmt, ##args);	\
		if (vdata && vdata->kernel_log_obj)	\
			if (vdata->kernel_log_obj->enabled &&	\
				vdata->kernel_log_obj->log_level \
				>= MEMLOG_LEVEL_ERR)	\
				memlog_write_printf(	\
				vdata->kernel_log_obj,	\
					MEMLOG_LEVEL_ERR,	\
					fmt, ##args);	\
		}	\
	} while (0)

#endif /* __SND_SOC_VTS_DEBUG_H */
