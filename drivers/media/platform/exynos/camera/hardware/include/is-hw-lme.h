/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Samsung Exynos SoC series Pablo driver
 *
 * Exynos Pablo image subsystem functions
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef IS_HW_LME_H
#define IS_HW_LME_H

#include "is-hw.h"
#include "is-interface-ddk.h"

struct is_hw_lme {
	struct is_lib_isp		lib[IS_STREAM_COUNT];
	struct lme_param_set		param_set[IS_STREAM_COUNT];
};

#endif
