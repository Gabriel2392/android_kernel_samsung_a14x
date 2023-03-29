// SPDX-License-Identifier: GPL-2.0
/*
 * Samsung Specific feature
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
 *
 * Authors:
 *	Storage Driver <storage.sec@samsung.com>
 */
#ifndef __MMC_SEC_SYSFS_H__
#define __MMC_SEC_SYSFS_H__

#include "dw_mmc.h"

#define MAX_REQ_TYPE_INDEX  5	// sbc, cmd, data, stop, busy
#define MAX_ERR_TYPE_INDEX  2	// crc, timeout
#define MAX_LOG_INDEX   (MAX_REQ_TYPE_INDEX * MAX_ERR_TYPE_INDEX)

#define SD_SBC_OFFSET 0
#define SD_CMD_OFFSET 2
#define SD_DATA_OFFSET 4
#define SD_STOP_OFFSET 6
#define SD_BUSY_OFFSET 8

#define STATUS_MASK (R1_ERROR | R1_CC_ERROR | R1_CARD_ECC_FAILED | \
		     R1_WP_VIOLATION | R1_OUT_OF_RANGE)

void sd_sec_init_sysfs(struct dw_mci *host);

enum {
	SEC_INVALID_SD_SLOT = 0,
	SEC_NO_DET_SD_SLOT,
	SEC_HOTPLUG_SD_SLOT,
	SEC_HYBRID_SD_SLOT,
};

struct sd_sec_err_info {
	char type[5];	// sbc, cmd, data, stop, busy
	int err_type;
	u32 status;
	u64 first_issue_time;
	u64 last_issue_time;
	u32 count;
};

struct sd_sec_status_err_info {
	u32 ge_cnt;		// status[19] : general error or unknown error
	u32 cc_cnt;		// status[20] : internal card controller error
	u32 ecc_cnt;	// status[21] : ecc error
	u32 wp_cnt;		// status[26] : write protection error
	u32 oor_cnt;	// status[31] : out of range error
	u32 noti_cnt;	// uevent notification count
};

struct sd_sec_vendor_info {
	struct mmc_host *mmc;
	unsigned int card_detect_cnt;
	int slot_type;
	unsigned long tstamp_last_cmd;
	struct work_struct noti_work;
	struct sd_sec_err_info err_info[MAX_LOG_INDEX];
	struct sd_sec_status_err_info status_err;
	struct sd_sec_err_info saved_err_info[MAX_LOG_INDEX];
	struct sd_sec_status_err_info saved_status_err;
};

extern struct sd_sec_vendor_info svi;

extern struct device *sec_sdcard_cmd_dev;

#endif
