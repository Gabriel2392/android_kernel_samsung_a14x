/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Samsung Exynos SoC series Pablo driver
 *
 * Exynos Pablo image subsystem functions
 *
 * Copyright (c) 2021 Samsung Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef IS_HW_BYRP_V2_H
#define IS_HW_BYRP_V2_H

#include "is-hw.h"
#include "is-param.h"
#include "is-hw-common-dma.h"
#include "is-hw-api-byrp-v2_0.h"

#define __CEIL_DIV(x, a)	(((x)+(a)-1)/(a))

enum is_hw_byrp_event_id {
	BYRP_EVENT_FRAME_START = 1,
	BYRP_EVENT_FRAME_END,
};

struct is_hw_byrp {
	struct byrp_param_set		param_set[IS_STREAM_COUNT];
	struct is_common_dma		*rdma;
	struct is_common_dma		*wdma;
	struct is_byrp_config		config;
	u32				irq_state[BYRP_INTR_MAX];
	u32				instance;
	u32				rdma_max_cnt;
	u32				wdma_max_cnt;
	unsigned long			state;
	atomic_t			strip_index;

	struct is_priv_buf		*pb_c_loader_payload;
	unsigned long			kva_c_loader_payload;
	dma_addr_t			dva_c_loader_payload;
	struct is_priv_buf		*pb_c_loader_header;
	unsigned long			kva_c_loader_header;
	dma_addr_t			dva_c_loader_header;
};

void is_hw_byrp_dump(void);
int is_hw_byrp_test(struct is_hw_ip *hw_ip, struct is_interface *itf,
	struct is_interface_ischain *itfc, int id, const char *name);
#endif
