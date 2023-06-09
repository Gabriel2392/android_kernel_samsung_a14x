// SPDX-License-Identifier: GPL-2.0
/*
 * Samsung Exynos SoC series Pablo driver
 *
 * MCSC HW control APIs
 *
 * Copyright (C) 2020 Samsung Electronics Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "is-hw-api-mcscaler-v3.h"
#include "sfr/is-sfr-mcsc-v10_20.h"
#include "is-hw.h"
#include "is-hw-control.h"
#include "is-param.h"

const struct scaler_filter_v_coef_cfg v_coef_4tap[7] = {
	/* x8/8 */
	{
		{  0, -15, -25, -31, -33, -33, -31, -27, -23},
		{512, 508, 495, 473, 443, 408, 367, 324, 279},
		{  0,  20,  45,  75, 110, 148, 190, 234, 279},
		{  0,  -1,  -3,  -5,  -8, -11, -14, -19, -23},
	},
	/* x7/8 */
	{
		{ 32,  17,   3,  -7, -14, -18, -20, -20, -19},
		{448, 446, 437, 421, 399, 373, 343, 310, 275},
		{ 32,  55,  79, 107, 138, 170, 204, 240, 275},
		{  0,  -6,  -7,  -9, -11, -13, -15, -18, -19},
	},
	/* x6/8 */
	{
		{ 61,  46,  31,  19,   9,   2,  -3,  -7,  -9},
		{390, 390, 383, 371, 356, 337, 315, 291, 265},
		{ 61,  83, 106, 130, 156, 183, 210, 238, 265},
		{  0,  -7,  -8,  -8,  -9, -10, -10, -10,  -9},
	},
	/* x5/8 */
	{
		{ 85,  71,  56,  43,  32,  23,  16,   9,   5},
		{341, 341, 336, 328, 317, 304, 288, 271, 251},
		{ 86, 105, 124, 145, 166, 187, 209, 231, 251},
		{  0,  -5,  -4,  -4,  -3,  -2,  -1,   1,   5},
	},
	/* x4/8 */
	{
		{104,  89,  76,  63,  52,  42,  33,  26,  20},
		{304, 302, 298, 293, 285, 275, 264, 251, 236},
		{104, 120, 136, 153, 170, 188, 205, 221, 236},
		{  0,   1,   2,   3,   5,   7,  10,  14,  20},
	},
	/* x3/8 */
	{
		{118, 103,  90,  78,  67,  57,  48,  40,  33},
		{276, 273, 270, 266, 260, 253, 244, 234, 223},
		{118, 129, 143, 157, 171, 185, 199, 211, 223},
		{  0,   7,   9,  11,  14,  17,  21,  27,  33},
	},
	/* x2/8 */
	{
		{127, 111, 100,  88,  78,  68,  59,  50,  43},
		{258, 252, 250, 247, 242, 237, 230, 222, 213},
		{127, 135, 147, 159, 171, 182, 193, 204, 213},
		{  0,  14,  15,  18,  21,  25,  30,  36,  43},
	}
};

const struct scaler_filter_h_coef_cfg h_coef_8tap[7] = {
	/* x8/8 */
	{
		{  0,  -2,  -4,  -5,  -6,  -6,  -6,  -6,  -5},
		{  0,   8,  14,  20,  23,  25,  26,  25,  23},
		{  0, -25, -46, -62, -73, -80, -83, -82, -78},
		{512, 509, 499, 482, 458, 429, 395, 357, 316},
		{  0,  30,  64, 101, 142, 185, 228, 273, 316},
		{  0,  -9, -19, -30, -41, -53, -63, -71, -78},
		{  0,   2,   5,   8,  12,  15,  19,  21,  23},
		{  0,  -1,  -1,  -2,  -3,  -3,  -4,  -5,  -5},
	},
	/* x7/8 */
	{
		{ 12,   9,   7,   5,   3,   2,   1,   0,  -1},
		{-32, -24, -16,  -9,  -3,   2,   7,  10,  13},
		{ 56,  29,   6, -14, -30, -43, -53, -60, -65},
		{444, 445, 438, 426, 410, 390, 365, 338, 309},
		{ 52,  82, 112, 144, 177, 211, 244, 277, 309},
		{-32, -39, -46, -52, -58, -63, -66, -66, -65},
		{ 12,  13,  14,  15,  16,  16,  16,  15,  13},
		{  0,  -3,	-3,  -3,  -3,  -3,  -2,  -2,  -1},
	},
	/* x6/8 */
	{
		{  8,   9,   8,   8,   8,   7,   7,   5,   5},
		{-44, -40, -36, -32, -27, -22, -18, -13,  -9},
		{100,  77,  57,  38,  20,   5,  -9, -20, -30},
		{384, 382, 377, 369, 358, 344, 329, 310, 290},
		{100, 123, 147, 171, 196, 221, 245, 268, 290},
		{-44, -47, -49, -49, -48, -47, -43, -37, -30},
		{  8,   8,   7,   5,   3,   1,  -2,  -5,  -9},
		{  0,   0,   1,   2,   2,   3,   3,   4,   5},
	},
	/* x5/8 */
	{
		{ -3,  -3,  -1,   0,   1,   2,   2,   3,   3},
		{-31, -32, -33, -32, -31, -30, -28, -25, -23},
		{130, 113,  97,  81,  66,  52,  38,  26,  15},
		{320, 319, 315, 311, 304, 296, 286, 274, 261},
		{130, 147, 165, 182, 199, 216, 232, 247, 261},
		{-31, -29, -26, -22, -17, -11,  -3,   5,  15},
		{ -3,  -6,  -8, -11, -13, -16, -18, -21, -23},
		{  0,   3,   3,   3,   3,   3,   3,   3,   3},
	},
	/* x4/8 */
	{
		{-11, -10,  -9,  -8,  -7,  -6,  -5,  -5,  -4},
		{  0,  -4,  -7, -10, -12, -14, -15, -16, -17},
		{140, 129, 117, 106,  95,  85,  74,  64,  55},
		{255, 254, 253, 250, 246, 241, 236, 229, 222},
		{140, 151, 163, 174, 185, 195, 204, 214, 222},
		{  0,   5,  10,  16,  22,  29,  37,  46,  55},
		{-12, -13, -14, -15, -16, -16, -17, -17, -17},
		{  0,   0,  -1,  -1,  -1,  -2,  -2,  -3,  -4},
	},
	/* x3/8 */
	{
		{ -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5},
		{ 31,  27,  23,  19,  16,  12,  10,   7,   5},
		{133, 126, 119, 112, 105,  98,  91,  84,  78},
		{195, 195, 194, 193, 191, 189, 185, 182, 178},
		{133, 139, 146, 152, 158, 163, 169, 174, 178},
		{ 31,  37,  41,  47,  53,  59,  65,  71,  78},
		{ -6,  -4,  -3,  -2,  -2,   0,   1,   3,   5},
		{  0,  -3,  -3,  -4,  -4,  -4,  -4,  -4,  -5},
	},
	/* x2/8 */
	{
		{ 10,   9,   7,   6,   5,   4,   4,   3,   2},
		{ 52,  48,  45,  41,  38,  35,  31,  29,  26},
		{118, 114, 110, 106, 102,  98,  94,  89,  85},
		{152, 152, 151, 150, 149, 148, 146, 145, 143},
		{118, 122, 125, 129, 132, 135, 138, 140, 143},
		{ 52,  56,  60,  64,  68,  72,  77,  81,  85},
		{ 10,  11,  13,  15,  17,  19,  21,  23,  26},
		{  0,   0,   1,   1,   1,   1,   1,   2,   2},
	}
};
void is_scaler_set_djag_hf_img_size(void __iomem *base_addr, u32 width, u32 height);
void is_scaler_start(void __iomem *base_addr, u32 hw_id)
{
	/* Qactive must set to "1" before scaler enable */
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_APB_CLK_GATE_CTRL], &mcsc_fields[MCSC_F_QACTIVE_ENABLE], 1);

	switch (hw_id) {
	case DEV_HW_MCSC0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SCALER_ENABLE], &mcsc_fields[MCSC_F_SCALER_ENABLE], 1);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}
}

void is_scaler_rdma_start(void __iomem *base_addr, u32 hw_id)
{
	is_scaler_dump(base_addr);
	switch (hw_id) {
	case DEV_HW_MCSC0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SCALER_RDMA_START],
			&mcsc_fields[MCSC_F_SCALER_RDMA_START], 1);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC rdma api\n", hw_id);
		break;
	}
}

void is_scaler_stop(void __iomem *base_addr, u32 hw_id)
{
	switch (hw_id) {
	case DEV_HW_MCSC0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SCALER_ENABLE], &mcsc_fields[MCSC_F_SCALER_ENABLE], 0);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}

#if defined(ENABLE_HWACG_CONTROL)
	/* Qactive must set to "0" for ip clock gating */
	if (!is_hw_get_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_ENABLE])
		&& is_hw_get_field(base_addr,
			&mcsc_regs[MCSC_R_SCALER_RUNNING_STATUS], &mcsc_fields[MCSC_F_SCALER_IDLE]))
		is_hw_set_field(base_addr,
			&mcsc_regs[MCSC_R_APB_CLK_GATE_CTRL], &mcsc_fields[MCSC_F_QACTIVE_ENABLE], 0);
#endif
}

#define IS_RESET_DONE(addr, reg, field)	\
	(is_hw_get_field(addr, &mcsc_regs[reg], &mcsc_fields[field]) != 0)

u32 is_scaler_sw_reset(void __iomem *base_addr, u32 hw_id, u32 global, u32 partial)
{
	u32 reset_count = 0;

	/* request scaler reset */
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC_RESET_CTRL_GLOBAL],
		&mcsc_fields[MCSC_F_SW_RESET_GLOBAL], 1);

	/* wait reset complete */
	do {
		reset_count++;
		if (reset_count > 10000)
			return reset_count;
	} while (IS_RESET_DONE(base_addr, MCSC_R_SCALER_RESET_STATUS, MCSC_F_SW_RESET_GLOBAL_STATUS));

	return 0;
}

void is_scaler_clear_intr_all(void __iomem *base_addr, u32 hw_id)
{
	u32 reg_val = 0;

	switch (hw_id) {
	case DEV_HW_MCSC0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_STALL_TIMEOUT_INT_0], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_FM_SUB_FRAME_START_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_FM_SUB_FRAME_FINISH_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SHADOW_COPY_FINISH_OVER_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SHADOW_COPY_FINISH_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_INPUT_FRAME_CRUSH_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SCALER_OVERFLOW_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_INPUT_VERTICAL_UNF_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_INPUT_VERTICAL_OVF_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_INPUT_HORIZONTAL_UNF_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_INPUT_HORIZONTAL_OVF_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_CORE_FINISH_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA_FINISH_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_FRAME_START_INT], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_FRAME_END_INT], 1);

		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT], reg_val);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}
}

void is_scaler_disable_intr(void __iomem *base_addr, u32 hw_id)
{
	u32 reg_val = 0;

	switch (hw_id) {
	case DEV_HW_MCSC0:
		reg_val = is_hw_set_field_value(reg_val,
			&mcsc_fields[MCSC_F_STALL_TIMEOUT_INT_MASK_0], 1);
		reg_val = is_hw_set_field_value(reg_val,
			&mcsc_fields[MCSC_F_FM_SUB_FRAME_START_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val,
			&mcsc_fields[MCSC_F_FM_SUB_FRAME_FINISH_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val,
			&mcsc_fields[MCSC_F_SHADOW_COPY_FINISH_OVER_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val,
			&mcsc_fields[MCSC_F_SHADOW_COPY_FINISH_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val,
			&mcsc_fields[MCSC_F_INPUT_FRAME_CRUSH_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val,
			&mcsc_fields[MCSC_F_SCALER_OVERFLOW_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val,
			&mcsc_fields[MCSC_F_INPUT_VERTICAL_UNF_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val,
			&mcsc_fields[MCSC_F_INPUT_VERTICAL_OVF_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val,
			&mcsc_fields[MCSC_F_INPUT_HORIZONTAL_UNF_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val,
			&mcsc_fields[MCSC_F_INPUT_HORIZONTAL_OVF_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_CORE_FINISH_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA_FINISH_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_FRAME_START_INT_MASK], 1);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_FRAME_END_INT_MASK], 1);

		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT_MASK], reg_val);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}
}

void is_scaler_mask_intr(void __iomem *base_addr, u32 hw_id, u32 intr_mask)
{
	switch (hw_id) {
	case DEV_HW_MCSC0:
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT_MASK], intr_mask);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}
}

u32 is_scaler_get_mask_intr(void)
{
	return MCSC_INTR_MASK;
}

unsigned int is_scaler_intr_occurred0(unsigned int status, enum mcsc_event_type type)
{
	u32 mask;

	switch (type) {
	case INTR_FRAME_START:
		mask = 1 << INTR_MC_SCALER_FRAME_START;
		break;
	case INTR_FRAME_END:
		mask = 1 << INTR_MC_SCALER_FRAME_END;
		break;
	case INTR_OVERFLOW:
		mask = 1 << INTR_MC_SCALER_OVERFLOW;
		break;
	case INTR_INPUT_VERTICAL_UNF:
		mask = 1 << INTR_MC_SCALER_INPUT_VERTICAL_UNF;
		break;
	case INTR_INPUT_VERTICAL_OVF:
		mask = 1 << INTR_MC_SCALER_INPUT_VERTICAL_OVF;
		break;
	case INTR_INPUT_HORIZONTAL_UNF:
		mask = 1 << INTR_MC_SCALER_INPUT_HORIZONTAL_UNF;
		break;
	case INTR_INPUT_HORIZONTAL_OVF:
		mask = 1 << INTR_MC_SCALER_INPUT_HORIZONTAL_OVF;
		break;
	case INTR_WDMA_FINISH:
		mask = 1 << INTR_MC_SCALER_WDMA_FINISH;
		break;
	default:
		return 0;
	}

	return status & mask;
}

static void is_scaler0_set_shadow_ctrl(void __iomem *base_addr, enum mcsc_shadow_ctrl ctrl)
{
	/* not support */
}

void is_scaler_set_shadow_ctrl(void __iomem *base_addr, u32 hw_id, enum mcsc_shadow_ctrl ctrl)
{
	switch (hw_id) {
	case DEV_HW_MCSC0:
		is_scaler0_set_shadow_ctrl(base_addr, ctrl);
		break;
	default:
		break;
	}
}

void is_scaler_clear_shadow_ctrl(void __iomem *base_addr, u32 hw_id)
{
	/* not support */
}

void is_scaler_get_input_status(void __iomem *base_addr, u32 hw_id, u32 *hl, u32 *vl)
{
	switch (hw_id) {
	case DEV_HW_MCSC0:
		*hl = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SCALER_INPUT_STATUS],
			&mcsc_fields[MCSC_F_CUR_HORIZONTAL_CNT]);
		*vl = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SCALER_INPUT_STATUS],
			&mcsc_fields[MCSC_F_CUR_VERTICAL_CNT]);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}
}

void is_scaler_set_input_source(void __iomem *base_addr, u32 hw_id, u32 rdma, bool strgen)
{
	/* parameter - 0: No input , 1: OTF input, 2: RDMA input */
	/* reg value - 0: OTF input, 1: RDMA input */
	// adjust input type
	if (rdma >= 1)
		rdma = rdma - 1;
	switch (hw_id) {
	case DEV_HW_MCSC0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SCALER_INPUT_TYPE],
			&mcsc_fields[MCSC_F_SCALER_INPUT_TYPE], rdma);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}
}

u32 is_scaler_get_input_source(void __iomem *base_addr, u32 hw_id)
{
	/* not support */
	return 0;
}

void is_scaler_set_input_img_size(void __iomem *base_addr, u32 hw_id, u32 width, u32 height)
{
	u32 reg_val = 0;

	if (width % MCSC_WIDTH_ALIGN != 0)
		err_hw("INPUT_IMG_HSIZE_%d(%d) should be multiple of 4", hw_id, width);

	switch (hw_id) {
	case DEV_HW_MCSC0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SCALER_INPUT_IMG_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SCALER_INPUT_IMG_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INPUT_IMG_SIZE], reg_val);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}

}

void is_scaler_get_input_img_size(void __iomem *base_addr, u32 hw_id, u32 *width, u32 *height)
{
	switch (hw_id) {
	case DEV_HW_MCSC0:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SCALER_INPUT_IMG_SIZE],
			&mcsc_fields[MCSC_F_SCALER_INPUT_IMG_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SCALER_INPUT_IMG_SIZE],
			&mcsc_fields[MCSC_F_SCALER_INPUT_IMG_VSIZE]);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}

}

u32 is_scaler_get_scaler_path(void __iomem *base_addr, u32 hw_id, u32 output_id)
{
	u32 input = 0, enable_poly = 0, enable_post = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		enable_poly = is_hw_get_field(base_addr,
			&mcsc_regs[MCSC_R_SC0_CTRL], &mcsc_fields[MCSC_F_SC0_ENABLE]);
		enable_post = is_hw_get_field(base_addr,
			&mcsc_regs[MCSC_R_PC0_CTRL], &mcsc_fields[MCSC_F_PC0_ENABLE]);
		break;
	case MCSC_OUTPUT1:
		enable_poly = is_hw_get_field(base_addr,
			&mcsc_regs[MCSC_R_SC1_CTRL], &mcsc_fields[MCSC_F_SC1_ENABLE]);
		enable_post = is_hw_get_field(base_addr,
			&mcsc_regs[MCSC_R_PC1_CTRL], &mcsc_fields[MCSC_F_PC1_ENABLE]);
		break;
	case MCSC_OUTPUT2:
		enable_poly = is_hw_get_field(base_addr,
			&mcsc_regs[MCSC_R_SC2_CTRL], &mcsc_fields[MCSC_F_SC2_ENABLE]);
		enable_post = is_hw_get_field(base_addr,
			&mcsc_regs[MCSC_R_PC2_CTRL], &mcsc_fields[MCSC_F_PC2_ENABLE]);
		break;
	default:
		break;
	}
	dbg_hw(2, "[ID:%d]%s:[SRC:%d]->[OUT:%d], en(poly:%d, post:%d)\n",
		hw_id, __func__, input, output_id, enable_poly, enable_post);

	return (DEV_HW_MCSC0 + input);
}

void is_scaler_set_poly_scaler_enable(void __iomem *base_addr, u32 hw_id, u32 output_id, u32 enable)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC0_CTRL], &mcsc_fields[MCSC_F_SC0_ENABLE], enable);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC1_CTRL], &mcsc_fields[MCSC_F_SC1_ENABLE], enable);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC2_CTRL], &mcsc_fields[MCSC_F_SC2_ENABLE], enable);
		break;
	default:
		break;
	}
}

void is_scaler_set_poly_scaler_bypass(void __iomem *base_addr, u32 output_id, u32 bypass)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC0_CTRL], &mcsc_fields[MCSC_F_SC0_BYPASS], bypass);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC1_CTRL], &mcsc_fields[MCSC_F_SC1_BYPASS], bypass);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC2_CTRL], &mcsc_fields[MCSC_F_SC2_BYPASS], bypass);
		break;
	default:
		break;
	}
}

void is_scaler_set_poly_src_size(void __iomem *base_addr, u32 output_id,
	u32 pos_x, u32 pos_y, u32 width, u32 height)
{
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC0_SRC_HPOS], pos_x);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC0_SRC_VPOS], pos_y);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC0_SRC_POS], reg_val);

		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC0_SRC_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC0_SRC_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC0_SRC_SIZE], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC1_SRC_HPOS], pos_x);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC1_SRC_VPOS], pos_y);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC1_SRC_POS], reg_val);

		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC1_SRC_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC1_SRC_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC1_SRC_SIZE], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC2_SRC_HPOS], pos_x);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC2_SRC_VPOS], pos_y);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC2_SRC_POS], reg_val);

		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC2_SRC_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC2_SRC_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC2_SRC_SIZE], reg_val);
		break;
	default:
		break;
	}
}

void is_scaler_get_poly_src_size(void __iomem *base_addr, u32 output_id, u32 *width, u32 *height)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC0_SRC_SIZE],
			&mcsc_fields[MCSC_F_SC0_SRC_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC0_SRC_SIZE],
			&mcsc_fields[MCSC_F_SC0_SRC_VSIZE]);
		break;
	case MCSC_OUTPUT1:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC1_SRC_SIZE],
			&mcsc_fields[MCSC_F_SC1_SRC_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC1_SRC_SIZE],
			&mcsc_fields[MCSC_F_SC1_SRC_VSIZE]);
		break;
	case MCSC_OUTPUT2:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC2_SRC_SIZE],
			&mcsc_fields[MCSC_F_SC2_SRC_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC2_SRC_SIZE],
			&mcsc_fields[MCSC_F_SC2_SRC_VSIZE]);
		break;
	default:
		break;
	}
}

void is_scaler_set_poly_dst_size(void __iomem *base_addr, u32 output_id, u32 width, u32 height)
{
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC0_DST_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC0_DST_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC0_DST_SIZE], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC1_DST_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC1_DST_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC1_DST_SIZE], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC2_DST_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC2_DST_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC2_DST_SIZE], reg_val);
		break;
	default:
		break;
	}
}

void is_scaler_get_poly_dst_size(void __iomem *base_addr, u32 output_id, u32 *width, u32 *height)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC0_DST_SIZE],
			&mcsc_fields[MCSC_F_SC0_DST_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC0_DST_SIZE],
			&mcsc_fields[MCSC_F_SC0_DST_VSIZE]);
		break;
	case MCSC_OUTPUT1:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC1_DST_SIZE],
			&mcsc_fields[MCSC_F_SC1_DST_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC1_DST_SIZE],
			&mcsc_fields[MCSC_F_SC1_DST_VSIZE]);
		break;
	case MCSC_OUTPUT2:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC2_DST_SIZE],
			&mcsc_fields[MCSC_F_SC2_DST_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC2_DST_SIZE],
			&mcsc_fields[MCSC_F_SC2_DST_VSIZE]);
		break;
	default:
		break;
	}
}

void is_scaler_set_poly_scaling_ratio(void __iomem *base_addr, u32 output_id, u32 hratio, u32 vratio)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC0_H_RATIO],
			&mcsc_fields[MCSC_F_SC0_H_RATIO], hratio);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC0_V_RATIO],
			&mcsc_fields[MCSC_F_SC0_V_RATIO], vratio);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC1_H_RATIO],
			&mcsc_fields[MCSC_F_SC1_H_RATIO], hratio);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC1_V_RATIO],
			&mcsc_fields[MCSC_F_SC1_V_RATIO], vratio);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC2_H_RATIO],
			&mcsc_fields[MCSC_F_SC2_H_RATIO], hratio);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC2_V_RATIO],
			&mcsc_fields[MCSC_F_SC2_V_RATIO], vratio);
		break;
	default:
		break;
	}
}

void is_scaler_set_h_init_phase_offset(void __iomem *base_addr, u32 output_id, u32 h_offset)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC0_H_INIT_PHASE_OFFSET],
			&mcsc_fields[MCSC_F_SC0_H_INIT_PHASE_OFFSET], h_offset);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC1_H_INIT_PHASE_OFFSET],
			&mcsc_fields[MCSC_F_SC1_H_INIT_PHASE_OFFSET], h_offset);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC2_H_INIT_PHASE_OFFSET],
			&mcsc_fields[MCSC_F_SC2_H_INIT_PHASE_OFFSET], h_offset);
		break;
	default:
		break;
	}
}

void is_scaler_set_v_init_phase_offset(void __iomem *base_addr, u32 output_id, u32 v_offset)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC0_V_INIT_PHASE_OFFSET],
			&mcsc_fields[MCSC_F_SC0_V_INIT_PHASE_OFFSET], v_offset);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC1_V_INIT_PHASE_OFFSET],
			&mcsc_fields[MCSC_F_SC1_V_INIT_PHASE_OFFSET], v_offset);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC2_V_INIT_PHASE_OFFSET],
			&mcsc_fields[MCSC_F_SC2_V_INIT_PHASE_OFFSET], v_offset);
		break;
	default:
		break;
	}
}

void is_scaler_set_poly_scaler_h_coef(void __iomem *base_addr, u32 output_id, struct scaler_filter_h_coef_cfg *h_coef_cfg)
{
	int index;
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		for (index = 0; index <= 8; index++) {
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC0_H_COEFF_0A + (8 * index)], h_coef_cfg->h_coef_a[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC0_H_COEFF_0B + (8 * index)], h_coef_cfg->h_coef_b[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC0_H_COEFF_0AB + (4 * index)], reg_val);
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC0_H_COEFF_0C + (8 * index)], h_coef_cfg->h_coef_c[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC0_H_COEFF_0D + (8 * index)], h_coef_cfg->h_coef_d[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC0_H_COEFF_0CD + (4 * index)], reg_val);
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC0_H_COEFF_0E + (8 * index)], h_coef_cfg->h_coef_e[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC0_H_COEFF_0F + (8 * index)], h_coef_cfg->h_coef_f[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC0_H_COEFF_0EF + (4 * index)], reg_val);
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC0_H_COEFF_0G + (8 * index)], h_coef_cfg->h_coef_g[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC0_H_COEFF_0H + (8 * index)], h_coef_cfg->h_coef_h[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC0_H_COEFF_0GH + (4 * index)], reg_val);
		}
		break;
	case MCSC_OUTPUT1:
		for (index = 0; index <= 8; index++) {
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC1_H_COEFF_0A + (8 * index)], h_coef_cfg->h_coef_a[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC1_H_COEFF_0B + (8 * index)], h_coef_cfg->h_coef_b[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC1_H_COEFF_0AB + (4 * index)], reg_val);
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC1_H_COEFF_0C + (8 * index)], h_coef_cfg->h_coef_c[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC1_H_COEFF_0D + (8 * index)], h_coef_cfg->h_coef_d[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC1_H_COEFF_0CD + (4 * index)], reg_val);
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC1_H_COEFF_0E + (8 * index)], h_coef_cfg->h_coef_e[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC1_H_COEFF_0F + (8 * index)], h_coef_cfg->h_coef_f[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC1_H_COEFF_0EF + (4 * index)], reg_val);
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC1_H_COEFF_0G + (8 * index)], h_coef_cfg->h_coef_g[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC1_H_COEFF_0H + (8 * index)], h_coef_cfg->h_coef_h[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC1_H_COEFF_0GH + (4 * index)], reg_val);
		}
		break;
	case MCSC_OUTPUT2:
		for (index = 0; index <= 8; index++) {
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC2_H_COEFF_0A + (8 * index)], h_coef_cfg->h_coef_a[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC2_H_COEFF_0B + (8 * index)], h_coef_cfg->h_coef_b[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC2_H_COEFF_0AB + (4 * index)], reg_val);
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC2_H_COEFF_0C + (8 * index)], h_coef_cfg->h_coef_c[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC2_H_COEFF_0D + (8 * index)], h_coef_cfg->h_coef_d[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC2_H_COEFF_0CD + (4 * index)], reg_val);
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC2_H_COEFF_0E + (8 * index)], h_coef_cfg->h_coef_e[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC2_H_COEFF_0F + (8 * index)], h_coef_cfg->h_coef_f[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC2_H_COEFF_0EF + (4 * index)], reg_val);
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC2_H_COEFF_0G + (8 * index)], h_coef_cfg->h_coef_g[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC2_H_COEFF_0H + (8 * index)], h_coef_cfg->h_coef_h[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC2_H_COEFF_0GH + (4 * index)], reg_val);
		}
		break;
	default:
		break;
	}
}

void is_scaler_set_poly_scaler_v_coef(void __iomem *base_addr, u32 output_id, struct scaler_filter_v_coef_cfg *v_coef_cfg)
{
	int index;
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		for (index = 0; index <= 8; index++) {
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC0_V_COEFF_0A + (4 * index)], v_coef_cfg->v_coef_a[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC0_V_COEFF_0B + (4 * index)], v_coef_cfg->v_coef_b[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC0_V_COEFF_0AB + (2 * index)], reg_val);
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC0_V_COEFF_0C + (4 * index)], v_coef_cfg->v_coef_c[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC0_V_COEFF_0D + (4 * index)], v_coef_cfg->v_coef_d[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC0_V_COEFF_0CD + (2 * index)], reg_val);
		}
		break;
	case MCSC_OUTPUT1:
		for (index = 0; index <= 8; index++) {
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC1_V_COEFF_0A + (4 * index)], v_coef_cfg->v_coef_a[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC1_V_COEFF_0B + (4 * index)], v_coef_cfg->v_coef_b[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC1_V_COEFF_0AB + (2 * index)], reg_val);
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC1_V_COEFF_0C + (4 * index)], v_coef_cfg->v_coef_c[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC1_V_COEFF_0D + (4 * index)], v_coef_cfg->v_coef_d[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC1_V_COEFF_0CD + (2 * index)], reg_val);
		}
		break;
	case MCSC_OUTPUT2:
		for (index = 0; index <= 8; index++) {
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC2_V_COEFF_0A + (4 * index)], v_coef_cfg->v_coef_a[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC2_V_COEFF_0B + (4 * index)], v_coef_cfg->v_coef_b[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC2_V_COEFF_0AB + (2 * index)], reg_val);
			reg_val = 0;
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC2_V_COEFF_0C + (4 * index)], v_coef_cfg->v_coef_c[index]);
			reg_val = is_hw_set_field_value(reg_val,
				&mcsc_fields[MCSC_F_SC2_V_COEFF_0D + (4 * index)], v_coef_cfg->v_coef_d[index]);
			is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC2_V_COEFF_0CD + (2 * index)], reg_val);
		}
		break;
	default:
		break;
	}
}

u32 get_scaler_coef_ver1(u32 ratio, bool adjust_coef)
{
	u32 coef;

	if (ratio <= RATIO_X8_8)
		coef = MCSC_COEFF_x8_8;
	else if (ratio > RATIO_X8_8 && ratio <= RATIO_X7_8)
		coef = MCSC_COEFF_x7_8;
	else if (ratio > RATIO_X7_8 && ratio <= RATIO_X6_8)
		coef = adjust_coef == true ? MCSC_COEFF_x7_8 : MCSC_COEFF_x6_8;
	else if (ratio > RATIO_X6_8 && ratio <= RATIO_X5_8)
		coef = adjust_coef == true ? MCSC_COEFF_x7_8 : MCSC_COEFF_x5_8;
	else if (ratio > RATIO_X5_8 && ratio <= RATIO_X4_8)
		coef = MCSC_COEFF_x4_8;
	else if (ratio > RATIO_X4_8 && ratio <= RATIO_X3_8)
		coef = MCSC_COEFF_x3_8;
	else if (ratio > RATIO_X3_8 && ratio <= RATIO_X2_8)
		coef = MCSC_COEFF_x2_8;
	else
		coef = MCSC_COEFF_x2_8;

	return coef;
}

u32 get_scaler_coef_ver2(u32 ratio, struct scaler_filter_coef_cfg *sc_coef)
{
	u32 coef;

	if (ratio <= RATIO_X8_8)
		coef = sc_coef->ratio_x8_8;
	else if (ratio > RATIO_X8_8 && ratio <= RATIO_X7_8)
		coef = sc_coef->ratio_x7_8;
	else if (ratio > RATIO_X7_8 && ratio <= RATIO_X6_8)
		coef = sc_coef->ratio_x6_8;
	else if (ratio > RATIO_X6_8 && ratio <= RATIO_X5_8)
		coef = sc_coef->ratio_x5_8;
	else if (ratio > RATIO_X5_8 && ratio <= RATIO_X4_8)
		coef = sc_coef->ratio_x4_8;
	else if (ratio > RATIO_X4_8 && ratio <= RATIO_X3_8)
		coef = sc_coef->ratio_x3_8;
	else if (ratio > RATIO_X3_8 && ratio <= RATIO_X2_8)
		coef = sc_coef->ratio_x2_8;
	else
		coef = sc_coef->ratio_x2_8;

	return coef;
}

void is_scaler_set_poly_scaler_coef(void __iomem *base_addr, u32 output_id,
	u32 hratio, u32 vratio, struct scaler_coef_cfg *sc_coefs[], int num_coef,
	enum exynos_sensor_position sensor_position)
{
	struct scaler_coef_cfg *sc_coef = sc_coefs[0];
	struct scaler_filter_h_coef_cfg *h_coef = NULL;
	struct scaler_filter_v_coef_cfg *v_coef = NULL;
	/* this value equals 0 - scale-down operation */
	u32 h_phase_offset = 0, v_phase_offset = 0;
	u32 h_coef_idx, v_coef_idx;

	if (sc_coef) {
		if (sc_coef->use_poly_sc_coef) {
			h_coef = &sc_coef->poly_sc_h_coef;
			v_coef = &sc_coef->poly_sc_v_coef;
		}
		else {
			h_coef_idx = get_scaler_coef_ver2(hratio, &sc_coef->poly_sc_coef[0]);
			v_coef_idx = get_scaler_coef_ver2(vratio, &sc_coef->poly_sc_coef[1]);
			h_coef = (struct scaler_filter_h_coef_cfg *)&h_coef_8tap[h_coef_idx];
			v_coef = (struct scaler_filter_v_coef_cfg *)&v_coef_4tap[v_coef_idx];
		}
		is_scaler_set_poly_scaler_h_coef(base_addr, output_id, h_coef);
		is_scaler_set_poly_scaler_v_coef(base_addr, output_id, v_coef);
	} else {
		err_hw("sc_coef is NULL");
	}
	/* scale up case */
	if (hratio < RATIO_X8_8)
		h_phase_offset = hratio >> 1;
	if (vratio < RATIO_X8_8)
		v_phase_offset = vratio >> 1;

	is_scaler_set_h_init_phase_offset(base_addr, output_id, h_phase_offset);
	is_scaler_set_v_init_phase_offset(base_addr, output_id, v_phase_offset);
}

void is_scaler_set_poly_round_mode(void __iomem *base_addr, u32 output_id, u32 mode)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC0_ROUND_MODE],
			&mcsc_fields[MCSC_F_SC0_ROUND_MODE], mode);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC1_ROUND_MODE],
			&mcsc_fields[MCSC_F_SC1_ROUND_MODE], mode);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC2_ROUND_MODE],
			&mcsc_fields[MCSC_F_SC2_ROUND_MODE], mode);
		break;
	default:
		break;
	}
}

void is_scaler_set_post_scaler_enable(void __iomem *base_addr, u32 output_id, u32 enable)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_CTRL], &mcsc_fields[MCSC_F_PC0_ENABLE], enable);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_CTRL], &mcsc_fields[MCSC_F_PC1_ENABLE], enable);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_CTRL], &mcsc_fields[MCSC_F_PC2_ENABLE], enable);
		break;
	default:
		break;
	}
}

void is_scaler_set_post_img_size(void __iomem *base_addr, u32 output_id, u32 width, u32 height)
{
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_IMG_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_IMG_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC0_IMG_SIZE], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_IMG_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_IMG_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC1_IMG_SIZE], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_IMG_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_IMG_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC2_IMG_SIZE], reg_val);
		break;
	default:
		break;
	}
}

void is_scaler_get_post_img_size(void __iomem *base_addr, u32 output_id, u32 *width, u32 *height)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC0_IMG_SIZE],
			&mcsc_fields[MCSC_F_PC0_IMG_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC0_IMG_SIZE],
			&mcsc_fields[MCSC_F_PC0_IMG_VSIZE]);
		break;
	case MCSC_OUTPUT1:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC1_IMG_SIZE],
			&mcsc_fields[MCSC_F_PC1_IMG_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC1_IMG_SIZE],
			&mcsc_fields[MCSC_F_PC1_IMG_VSIZE]);
		break;
	case MCSC_OUTPUT2:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC2_IMG_SIZE],
			&mcsc_fields[MCSC_F_PC2_IMG_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC2_IMG_SIZE],
			&mcsc_fields[MCSC_F_PC2_IMG_VSIZE]);
		break;
	default:
		break;
	}
}

void is_scaler_set_post_dst_size(void __iomem *base_addr, u32 output_id, u32 width, u32 height)
{
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_DST_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_DST_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC0_DST_SIZE], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_DST_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_DST_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC1_DST_SIZE], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_DST_HSIZE], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_DST_VSIZE], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC2_DST_SIZE], reg_val);
		break;
	default:
		break;
	}
}

void is_scaler_get_post_dst_size(void __iomem *base_addr, u32 output_id, u32 *width, u32 *height)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC0_DST_SIZE],
			&mcsc_fields[MCSC_F_PC0_DST_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC0_DST_SIZE],
			&mcsc_fields[MCSC_F_PC0_DST_VSIZE]);
		break;
	case MCSC_OUTPUT1:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC1_DST_SIZE],
			&mcsc_fields[MCSC_F_PC1_DST_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC1_DST_SIZE],
			&mcsc_fields[MCSC_F_PC1_DST_VSIZE]);
		break;
	case MCSC_OUTPUT2:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC2_DST_SIZE],
			&mcsc_fields[MCSC_F_PC2_DST_HSIZE]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC2_DST_SIZE],
			&mcsc_fields[MCSC_F_PC2_DST_VSIZE]);
		break;
	default:
		break;
	}
}

void is_scaler_set_post_scaling_ratio(void __iomem *base_addr, u32 output_id, u32 hratio, u32 vratio)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_H_RATIO],
			&mcsc_fields[MCSC_F_PC0_H_RATIO], hratio);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_V_RATIO],
			&mcsc_fields[MCSC_F_PC0_V_RATIO], vratio);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_H_RATIO],
			&mcsc_fields[MCSC_F_PC1_H_RATIO], hratio);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_V_RATIO],
			&mcsc_fields[MCSC_F_PC1_V_RATIO], vratio);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_H_RATIO],
			&mcsc_fields[MCSC_F_PC2_H_RATIO], hratio);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_V_RATIO],
			&mcsc_fields[MCSC_F_PC2_V_RATIO], vratio);
		break;
	default:
		break;
	}
}


void is_scaler_set_post_h_init_phase_offset(void __iomem *base_addr, u32 output_id, u32 h_offset)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_H_INIT_PHASE_OFFSET],
			&mcsc_fields[MCSC_F_PC0_H_INIT_PHASE_OFFSET], h_offset);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_H_INIT_PHASE_OFFSET],
			&mcsc_fields[MCSC_F_PC1_H_INIT_PHASE_OFFSET], h_offset);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_H_INIT_PHASE_OFFSET],
			&mcsc_fields[MCSC_F_PC2_H_INIT_PHASE_OFFSET], h_offset);
		break;
	default:
		break;
	}
}

void is_scaler_set_post_v_init_phase_offset(void __iomem *base_addr, u32 output_id, u32 v_offset)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_V_INIT_PHASE_OFFSET],
			&mcsc_fields[MCSC_F_PC0_V_INIT_PHASE_OFFSET], v_offset);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_V_INIT_PHASE_OFFSET],
			&mcsc_fields[MCSC_F_PC1_V_INIT_PHASE_OFFSET], v_offset);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_V_INIT_PHASE_OFFSET],
			&mcsc_fields[MCSC_F_PC2_V_INIT_PHASE_OFFSET], v_offset);
		break;
	default:
		break;
	}
}

void is_scaler_set_post_scaler_h_v_coef(void __iomem *base_addr, u32 output_id, u32 h_sel, u32 v_sel)
{
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_H_COEFF_SEL], h_sel);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_V_COEFF_SEL], v_sel);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC0_COEFF_CTRL], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_H_COEFF_SEL], h_sel);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_V_COEFF_SEL], v_sel);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC1_COEFF_CTRL], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_H_COEFF_SEL], h_sel);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_V_COEFF_SEL], v_sel);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC2_COEFF_CTRL], reg_val);
		break;
	default:
		break;
	}
}

void is_scaler_set_post_scaler_coef(void __iomem *base_addr, u32 output_id,
	u32 hratio, u32 vratio, struct scaler_coef_cfg *sc_coefs[], int num_coef)
{
	u32 h_coef = 0, v_coef = 0;
	/* this value equals 0 - scale-down operation */
	u32 h_phase_offset = 0, v_phase_offset = 0;

	if (sc_coefs[0]) {
		h_coef = get_scaler_coef_ver2(hratio, &sc_coefs[0]->post_sc_coef[0]);
		v_coef = get_scaler_coef_ver2(vratio, &sc_coefs[0]->post_sc_coef[1]);
	}

	/* scale up case */
	if (hratio < RATIO_X8_8)
		h_phase_offset = hratio >> 1;
	if (vratio < RATIO_X8_8)
		v_phase_offset = vratio >> 1;

	is_scaler_set_post_h_init_phase_offset(base_addr, output_id, h_phase_offset);
	is_scaler_set_post_v_init_phase_offset(base_addr, output_id, v_phase_offset);
	is_scaler_set_post_scaler_h_v_coef(base_addr, output_id, h_coef, v_coef);
}

void is_scaler_set_post_round_mode(void __iomem *base_addr, u32 output_id, u32 mode)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_ROUND_MODE],
			&mcsc_fields[MCSC_F_PC0_ROUND_MODE], mode);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_ROUND_MODE],
			&mcsc_fields[MCSC_F_PC1_ROUND_MODE], mode);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_ROUND_MODE],
			&mcsc_fields[MCSC_F_PC2_ROUND_MODE], mode);
		break;
	default:
		break;
	}
}

void is_scaler_set_420_conversion(void __iomem *base_addr, u32 output_id, u32 conv420_weight, u32 conv420_en)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_CONV420_CTRL],
			&mcsc_fields[MCSC_F_PC0_CONV420_ENABLE], conv420_en);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_CONV420_WEIGHT],
			&mcsc_fields[MCSC_F_PC0_CONV420_WEIGHT], conv420_weight);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_CONV420_CTRL],
			&mcsc_fields[MCSC_F_PC1_CONV420_ENABLE], conv420_en);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_CONV420_WEIGHT],
			&mcsc_fields[MCSC_F_PC1_CONV420_WEIGHT], conv420_weight);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_CONV420_CTRL],
			&mcsc_fields[MCSC_F_PC2_CONV420_ENABLE], conv420_en);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_CONV420_WEIGHT],
			&mcsc_fields[MCSC_F_PC2_CONV420_WEIGHT], conv420_weight);
		break;
	default:
		break;
	}
}

void is_scaler_set_bchs_enable(void __iomem *base_addr, u32 output_id, bool bchs_en)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_BCHS_CTRL],
			&mcsc_fields[MCSC_F_PC0_BCHS_ENABLE], bchs_en);

		/* default BCHS clamp value */
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC0_BCHS_CLAMP_Y], 0x03FF0000);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC0_BCHS_CLAMP_C], 0x03FF0000);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_BCHS_CTRL],
			&mcsc_fields[MCSC_F_PC1_BCHS_ENABLE], bchs_en);

		/* default BCHS clamp value */
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC1_BCHS_CLAMP_Y], 0x03FF0000);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC1_BCHS_CLAMP_C], 0x03FF0000);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_BCHS_CTRL],
			&mcsc_fields[MCSC_F_PC2_BCHS_ENABLE], bchs_en);

		/* default BCHS clamp value */
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC2_BCHS_CLAMP_Y], 0x03FF0000);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC2_BCHS_CLAMP_C], 0x03FF0000);
		break;
	default:
		break;
	}
}

/* brightness/contrast control */
void is_scaler_set_b_c(void __iomem *base_addr, u32 output_id, u32 y_offset, u32 y_gain)
{
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_BCHS_Y_OFFSET], y_offset);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_BCHS_Y_GAIN], y_gain);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC0_BCHS_BC], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_BCHS_Y_OFFSET], y_offset);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_BCHS_Y_GAIN], y_gain);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC1_BCHS_BC], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_BCHS_Y_OFFSET], y_offset);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_BCHS_Y_GAIN], y_gain);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC2_BCHS_BC], reg_val);
		break;
	default:
		break;
	}
}

/* hue/saturation control */
void is_scaler_set_h_s(void __iomem *base_addr, u32 output_id,
	u32 c_gain00, u32 c_gain01, u32 c_gain10, u32 c_gain11)
{
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_BCHS_C_GAIN_00], c_gain00);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_BCHS_C_GAIN_01], c_gain01);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC0_BCHS_HS1], reg_val);
		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_BCHS_C_GAIN_10], c_gain10);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_BCHS_C_GAIN_11], c_gain11);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC0_BCHS_HS2], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_BCHS_C_GAIN_00], c_gain00);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_BCHS_C_GAIN_01], c_gain01);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC1_BCHS_HS1], reg_val);
		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_BCHS_C_GAIN_10], c_gain10);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_BCHS_C_GAIN_11], c_gain11);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC1_BCHS_HS2], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_BCHS_C_GAIN_00], c_gain00);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_BCHS_C_GAIN_01], c_gain01);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC2_BCHS_HS1], reg_val);
		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_BCHS_C_GAIN_10], c_gain10);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_BCHS_C_GAIN_11], c_gain11);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC2_BCHS_HS2], reg_val);
		break;
	default:
		break;
	}
}

void is_scaler_set_bchs_clamp(void __iomem *base_addr, u32 output_id,
	u32 y_max, u32 y_min, u32 c_max, u32 c_min)
{
	u32 reg_val_y = 0, reg_idx_y = 0;
	u32 reg_val_c = 0, reg_idx_c = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val_y = is_hw_set_field_value(reg_val_y, &mcsc_fields[MCSC_F_PC0_BCHS_Y_CLAMP_MAX], y_max);
		reg_val_y = is_hw_set_field_value(reg_val_y, &mcsc_fields[MCSC_F_PC0_BCHS_Y_CLAMP_MIN], y_min);
		reg_val_c = is_hw_set_field_value(reg_val_c, &mcsc_fields[MCSC_F_PC0_BCHS_C_CLAMP_MAX], c_max);
		reg_val_c = is_hw_set_field_value(reg_val_c, &mcsc_fields[MCSC_F_PC0_BCHS_C_CLAMP_MIN], c_min);
		reg_idx_y = MCSC_R_PC0_BCHS_CLAMP_Y;
		reg_idx_c = MCSC_R_PC0_BCHS_CLAMP_C;
		break;
	case MCSC_OUTPUT1:
		reg_val_y = is_hw_set_field_value(reg_val_y, &mcsc_fields[MCSC_F_PC1_BCHS_Y_CLAMP_MAX], y_max);
		reg_val_y = is_hw_set_field_value(reg_val_y, &mcsc_fields[MCSC_F_PC1_BCHS_Y_CLAMP_MIN], y_min);
		reg_val_c = is_hw_set_field_value(reg_val_c, &mcsc_fields[MCSC_F_PC1_BCHS_C_CLAMP_MAX], c_max);
		reg_val_c = is_hw_set_field_value(reg_val_c, &mcsc_fields[MCSC_F_PC1_BCHS_C_CLAMP_MIN], c_min);
		reg_idx_y = MCSC_R_PC1_BCHS_CLAMP_Y;
		reg_idx_c = MCSC_R_PC1_BCHS_CLAMP_C;
		break;
	case MCSC_OUTPUT2:
		reg_val_y = is_hw_set_field_value(reg_val_y, &mcsc_fields[MCSC_F_PC2_BCHS_Y_CLAMP_MAX], y_max);
		reg_val_y = is_hw_set_field_value(reg_val_y, &mcsc_fields[MCSC_F_PC2_BCHS_Y_CLAMP_MIN], y_min);
		reg_val_c = is_hw_set_field_value(reg_val_c, &mcsc_fields[MCSC_F_PC2_BCHS_C_CLAMP_MAX], c_max);
		reg_val_c = is_hw_set_field_value(reg_val_c, &mcsc_fields[MCSC_F_PC2_BCHS_C_CLAMP_MIN], c_min);
		reg_idx_y = MCSC_R_PC2_BCHS_CLAMP_Y;
		reg_idx_c = MCSC_R_PC2_BCHS_CLAMP_C;
		break;
	default:
		return;
	}

	is_hw_set_reg(base_addr, &mcsc_regs[reg_idx_y], reg_val_y);
	is_hw_set_reg(base_addr, &mcsc_regs[reg_idx_c], reg_val_c);
	dbg_hw(2, "[OUT:%d]set_bchs_clamp: Y:[%#x]%#x, C:[%#x]%#x\n", output_id,
			mcsc_regs[reg_idx_y].sfr_offset, is_hw_get_reg(base_addr, &mcsc_regs[reg_idx_y]),
			mcsc_regs[reg_idx_c].sfr_offset, is_hw_get_reg(base_addr, &mcsc_regs[reg_idx_c]));
}

void is_scaler_set_rdma_enable(void __iomem *base_addr, u32 hw_id, bool dma_in_en)
{
	if (dma_in_en)
		is_scaler_rdma_start(base_addr, hw_id);
}

void is_scaler_set_dma_out_enable(void __iomem *base_addr, u32 output_id, bool dma_out_en)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_DMA_OUT_CTRL],
			&mcsc_fields[MCSC_F_PC0_DMA_OUT_ENABLE], (u32)dma_out_en);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_DMA_OUT_CTRL],
			&mcsc_fields[MCSC_F_PC1_DMA_OUT_ENABLE], (u32)dma_out_en);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_DMA_OUT_CTRL],
			&mcsc_fields[MCSC_F_PC2_DMA_OUT_ENABLE], (u32)dma_out_en);
		break;
	default:
		break;
	}
}

u32 is_scaler_get_dma_out_enable(void __iomem *base_addr, u32 output_id)
{
	int ret = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC0_DMA_OUT_CTRL],
			&mcsc_fields[MCSC_F_PC0_DMA_OUT_ENABLE]);
		break;
	case MCSC_OUTPUT1:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC1_DMA_OUT_CTRL],
			&mcsc_fields[MCSC_F_PC1_DMA_OUT_ENABLE]);
		break;
	case MCSC_OUTPUT2:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC2_DMA_OUT_CTRL],
			&mcsc_fields[MCSC_F_PC2_DMA_OUT_ENABLE]);
		break;
	default:
		break;
	}

	return ret;
}

u32 is_scaler_g_dma_offset(struct param_mcs_output *output,
	u32 start_pos_x, u32 plane_i)
{
	u32 dma_offset, bitsperpixel;

	bitsperpixel =
		(output->bitsperpixel >> (plane_i * 8)) & 0xFF;
	dma_offset =
		start_pos_x * bitsperpixel / BITS_PER_BYTE;

	return dma_offset;
}

/* SBWC */
void is_scaler_set_wdma_sbwc_config(void __iomem *base_addr,
	struct param_mcs_output *output, u32 output_id,
	u32 *y_stride, u32 *uv_stride, u32 *y_2bit_stride, u32 *uv_2bit_stride)
{
	/* nothing to do */
}

void is_scaler_set_rdma_sbwc_enable(void __iomem *base_addr, u32 hw_id, bool sbwc_en)
{
	/* not support */
}

u32 is_scaler_get_rdma_sbwc_enable(void __iomem *base_addr, u32 hw_id)
{
	/* not support */
	return 0;
}

void is_scaler_set_rdma_sbwc_64b_align(void __iomem *base_addr, u32 hw_id, u32 align_64b)
{
	/* not support */
}

void is_scaler_set_rdma_sbwc_lossy_rate(void __iomem *base_addr, u32 hw_id, u32 lossy_rate)
{
	/* not support */
}

void is_scaler_set_wdma_sbwc_enable(void __iomem *base_addr, u32 output_id, bool sbwc_en)
{
	/* not support */
}

u32 is_scaler_get_wdma_sbwc_enable(void __iomem *base_addr, u32 output_id)
{
	/* not support */
	return 0;
}

void is_scaler_set_wdma_sbwc_64b_align(void __iomem *base_addr, u32 output_id, u32 align_64b)
{
	/* not support */
}

void is_scaler_set_wdma_sbwc_lossy_rate(void __iomem *base_addr, u32 output_id, u32 lossy_rate)
{
	/* not support */
}

void is_scaler_set_wdma_sbwc_sram_addr(void __iomem *base_addr, u32 output_id, u32 sram_addr)
{
	/* not support */
}

void is_scaler_set_rdma_sbwc_header_addr(void __iomem *base_addr, u32 haddr, int buf_index)
{
	/* not support */
}

void is_scaler_set_rdma_sbwc_header_stride(void __iomem *base_addr, u32 width)
{
	/* not support */
}

/* VOTF */
void is_scaler_set_rdma_votf_enable(void __iomem *base_addr, u32 hw_id, bool votf_en)
{
	/* not support */
}

void is_scaler_set_wdma_votf_enable(void __iomem *base_addr, u32 output_id, bool votf_en)
{
	/* not support */
}

u32 is_scaler_get_rdma_format(void __iomem *base_addr, u32 hw_id)
{
	/* not support */
	return 0;
}

void is_scaler_set_rdma_format(void __iomem *base_addr, u32 hw_id, u32 in_fmt)
{
	/* not support */
}

u32 is_scaler_get_mono_ctrl(void __iomem *base_addr, u32 hw_id)
{
	/* not support */
	return 0;
}

void is_scaler_set_mono_ctrl(void __iomem *base_addr, u32 hw_id, u32 in_fmt)
{
	/* not support */
}

void is_scaler_set_rdma_10bit_type(void __iomem *base_addr, u32 dma_in_10bit_type)
{
	/* not support */
}

void is_scaler_set_wdma_mono_ctrl(void __iomem *base_addr, u32 output_id, u32 fmt, bool en)
{
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA0_MONO_EN], en);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA0_MONO_CTRL], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA1_MONO_EN], en);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA1_MONO_CTRL], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA2_MONO_EN], en);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA2_MONO_CTRL], reg_val);
		break;
	default:
		break;
	}
}

void is_scaler_set_wdma_rgb_coefficient(void __iomem *base_addr)
{
	u32 reg_val = 0;

	/* this value is valid only YUV422 only.  2020 wide coefficient is used in JPEG, Pictures, preview. */
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_RGB_SRC_Y_OFFSET], 0);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA_RGB_OFFSET], reg_val);

	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_RGB_COEF_C00], 512);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_RGB_COEF_C10], 0);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA_RGB_COEF_0], reg_val);
	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_RGB_COEF_C20], 738);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_RGB_COEF_C01], 512);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA_RGB_COEF_1], reg_val);
	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_RGB_COEF_C11], -82);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_RGB_COEF_C21], -286);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA_RGB_COEF_2], reg_val);
	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_RGB_COEF_C02], 512);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_RGB_COEF_C12], 942);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA_RGB_COEF_3], reg_val);
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_WDMA_RGB_COEF_4],
		&mcsc_fields[MCSC_F_RGB_COEF_C22], 0);

}

void is_scaler_set_wdma_format_type(void __iomem *base_addr, u32 output_id, u32 format_type)
{
	/* not support */
}
void is_scaler_set_wdma_rgb_format(void __iomem *base_addr, u32 output_id, u32 out_fmt)
{
	/* not support */
}

void is_scaler_set_wdma_yuv_format(void __iomem *base_addr, u32 output_id, u32 out_fmt)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_WDMA0_DATA_FORMAT],
			&mcsc_fields[MCSC_F_WDMA0_DATA_FORMAT], out_fmt);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_WDMA1_DATA_FORMAT],
			&mcsc_fields[MCSC_F_WDMA1_DATA_FORMAT], out_fmt);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_WDMA2_DATA_FORMAT],
			&mcsc_fields[MCSC_F_WDMA2_DATA_FORMAT], out_fmt);
		break;
	default:
		break;
	}
}

void is_scaler_set_wdma_format(void __iomem *base_addr, u32 hw_id, u32 output_id, u32 plane, u32 out_fmt)
{
	u32 in_fmt;
	u32 format_type = 0;

	in_fmt = is_scaler_get_mono_ctrl(base_addr, hw_id);
	if (in_fmt == MCSC_MONO_Y8 && out_fmt != MCSC_MONO_Y8) {
		warn_hw("[ID:%d]Input format(MONO), out_format(%d) is changed to MONO format!\n",
			hw_id, out_fmt);
		out_fmt = MCSC_MONO_Y8;
	}

	/* format_type 0 : YUV , 1 : RGB */
	switch (out_fmt) {
	case MCSC_RGB_ARGB8888:
	case MCSC_RGB_BGRA8888:
	case MCSC_RGB_RGBA8888:
	case MCSC_RGB_ABGR8888:
	case MCSC_RGB_ABGR1010102:
	case MCSC_RGB_RGBA1010102:
		is_scaler_set_wdma_rgb_coefficient(base_addr);
		out_fmt = out_fmt - MCSC_RGB_RGBA8888 + 1;
		format_type = 1;
		break;
	default:
		format_type = 0;
		break;
	}

	/* 1 plane or RGB : mono ctrl should be disabled */
	if (plane == 1 || format_type == 1)
		is_scaler_set_wdma_mono_ctrl(base_addr, output_id, out_fmt, false);
	else if (out_fmt == MCSC_MONO_Y8)
		is_scaler_set_wdma_mono_ctrl(base_addr, output_id, out_fmt, true);

	is_scaler_set_wdma_format_type(base_addr, output_id, format_type);
	if (format_type) {
		if (output_id == MCSC_OUTPUT2)
			is_scaler_set_wdma_rgb_format(base_addr, output_id, out_fmt);
		else
			err_hw("RGB Conversion support only WDMA2");
	}
	else {
		is_scaler_set_wdma_yuv_format(base_addr, output_id, out_fmt);
	}
}

void is_scaler_set_wdma_dither(void __iomem *base_addr, u32 output_id,
	u32 fmt, u32 bitwidth, u32 plane)
{
	u32 reg_val = 0;
	u32 round_en = 0;
	u32 dither_en = 0;

	if (fmt == DMA_INOUT_FORMAT_RGB && bitwidth == DMA_INOUT_BIT_WIDTH_8BIT) {
		dither_en = 0;
		round_en = 1;
	} else if (bitwidth == DMA_INOUT_BIT_WIDTH_8BIT || plane == 4) {
		dither_en = 1;
		round_en = 0;
	} else {
		dither_en = 0;
		round_en = 0;
	}

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA0_DITHER_EN], dither_en);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA0_ROUND_EN], round_en);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA0_DITHER], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA1_DITHER_EN], dither_en);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA1_ROUND_EN], round_en);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA1_DITHER], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA2_DITHER_EN], dither_en);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA2_ROUND_EN], round_en);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA2_DITHER], reg_val);
		break;
	default:
		break;
	}

}

void is_scaler_get_wdma_format(void __iomem *base_addr, u32 output_id, u32 color_type, u32 *out_fmt)
{
	/* only support color_type 0 : YUV */
	switch (output_id) {
	case MCSC_OUTPUT0:
		*out_fmt = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA0_DATA_FORMAT],
			&mcsc_fields[MCSC_F_WDMA0_DATA_FORMAT]);
		break;
	case MCSC_OUTPUT1:
		*out_fmt = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA1_DATA_FORMAT],
			&mcsc_fields[MCSC_F_WDMA1_DATA_FORMAT]);
		break;
	case MCSC_OUTPUT2:
		*out_fmt = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA2_DATA_FORMAT],
			&mcsc_fields[MCSC_F_WDMA2_DATA_FORMAT]);
		break;
	default:
		break;
	}
}

void is_scaler_set_swap_mode(void __iomem *base_addr, u32 output_id, u32 swap)
{
	/* not support */
}

void is_scaler_set_max_mo(void __iomem *base_addr, u32 output_id, u32 mo)
{
	/* not support */
}

void is_scaler_set_flip_mode(void __iomem *base_addr, u32 output_id, u32 flip)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_WDMA0_FLIP_CONTROL],
			&mcsc_fields[MCSC_F_WDMA0_FLIP_CONTROL], flip);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_WDMA1_FLIP_CONTROL],
			&mcsc_fields[MCSC_F_WDMA1_FLIP_CONTROL], flip);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_WDMA2_FLIP_CONTROL],
			&mcsc_fields[MCSC_F_WDMA2_FLIP_CONTROL], flip);
		break;
	default:
		break;
	}
}

void is_scaler_set_rdma_size(void __iomem *base_addr, u32 width, u32 height)
{
	/* not support */
}

void is_scaler_get_rdma_size(void __iomem *base_addr, u32 *width, u32 *height)
{
	/* not support */
}

void is_scaler_set_wdma_size(void __iomem *base_addr, u32 output_id, u32 width, u32 height)
{
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA0_WIDTH], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA0_HEIGHT], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA0_IMG_SIZE], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA1_WIDTH], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA1_HEIGHT], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA1_IMG_SIZE], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA2_WIDTH], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA2_HEIGHT], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA2_IMG_SIZE], reg_val);
		break;
	default:
		break;
	}
}

void is_scaler_get_wdma_size(void __iomem *base_addr, u32 output_id, u32 *width, u32 *height)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA0_IMG_SIZE],
			&mcsc_fields[MCSC_F_WDMA0_WIDTH]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA0_IMG_SIZE],
			&mcsc_fields[MCSC_F_WDMA0_HEIGHT]);
		break;
	case MCSC_OUTPUT1:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA1_IMG_SIZE],
			&mcsc_fields[MCSC_F_WDMA1_WIDTH]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA1_IMG_SIZE],
			&mcsc_fields[MCSC_F_WDMA1_HEIGHT]);
		break;
	case MCSC_OUTPUT2:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA2_IMG_SIZE],
			&mcsc_fields[MCSC_F_WDMA2_WIDTH]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA2_IMG_SIZE],
			&mcsc_fields[MCSC_F_WDMA2_HEIGHT]);
		break;
	default:
		break;
	}
}

void is_scaler_set_rdma_stride(void __iomem *base_addr, u32 y_stride, u32 uv_stride)
{
	/* not support */
}

void is_scaler_set_rdma_2bit_stride(void __iomem *base_addr, u32 y_2bit_stride, u32 uv_2bit_stride)
{
	/* not support */
}

void is_scaler_get_rdma_stride(void __iomem *base_addr, u32 *y_stride, u32 *uv_stride)
{
	/* not support */
}

void is_scaler_set_wdma_stride(void __iomem *base_addr, u32 output_id, u32 y_stride, u32 uv_stride)
{
	u32 reg_val = 0;
	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA0_Y_STRIDE], y_stride);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA0_C_STRIDE], uv_stride);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA0_STRIDE], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA1_Y_STRIDE], y_stride);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA1_C_STRIDE], uv_stride);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA1_STRIDE], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA2_Y_STRIDE], y_stride);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_WDMA2_C_STRIDE], uv_stride);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_WDMA2_STRIDE], reg_val);
		break;
	default:
		break;
	}
}

void is_scaler_set_wdma_2bit_stride(void __iomem *base_addr, u32 output_id,
	u32 y_2bit_stride, u32 uv_2bit_stride)
{
	/* not support */
}

void is_scaler_get_wdma_stride(void __iomem *base_addr, u32 output_id, u32 *y_stride, u32 *uv_stride)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		*y_stride = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA0_STRIDE],
			&mcsc_fields[MCSC_F_WDMA0_Y_STRIDE]);
		*uv_stride = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA0_STRIDE],
			&mcsc_fields[MCSC_F_WDMA0_C_STRIDE]);
		break;
	case MCSC_OUTPUT1:
		*y_stride = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA1_STRIDE],
			&mcsc_fields[MCSC_F_WDMA1_Y_STRIDE]);
		*uv_stride = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA1_STRIDE],
			&mcsc_fields[MCSC_F_WDMA1_C_STRIDE]);
		break;
	case MCSC_OUTPUT2:
		*y_stride = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA2_STRIDE],
			&mcsc_fields[MCSC_F_WDMA2_Y_STRIDE]);
		*uv_stride = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_WDMA2_STRIDE],
			&mcsc_fields[MCSC_F_WDMA2_C_STRIDE]);
		break;
	default:
		break;
	}
}

void is_scaler_set_rdma_frame_seq(void __iomem *base_addr, u32 frame_seq)
{
	/* not support */
}

void is_scaler_set_rdma_addr(void __iomem *base_addr,
	u32 y_addr, u32 cb_addr, u32 cr_addr, int buf_index)
{
	/* not support */
}

void is_scaler_set_rdma_2bit_addr(void __iomem *base_addr,
	u32 y_2bit_addr, u32 cbcr_2bit_addr, int buf_index)
{
	/* not support */
}

void get_wdma_addr_arr(u32 output_id, u32 *addr)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		addr[0] = MCSC_R_WDMA0_BASE_ADDR_0;
		addr[1] = MCSC_R_WDMA0_BASE_ADDR_0_IDX1;
		addr[2] = MCSC_R_WDMA0_BASE_ADDR_0_IDX2;
		addr[3] = MCSC_R_WDMA0_BASE_ADDR_0_IDX3;
		addr[4] = MCSC_R_WDMA0_BASE_ADDR_0_IDX4;
		addr[5] = MCSC_R_WDMA0_BASE_ADDR_0_IDX5;
		addr[6] = MCSC_R_WDMA0_BASE_ADDR_0_IDX6;
		addr[7] = MCSC_R_WDMA0_BASE_ADDR_0_IDX7;
		break;
	case MCSC_OUTPUT1:
		addr[0] = MCSC_R_WDMA1_BASE_ADDR_0;
		addr[1] = MCSC_R_WDMA1_BASE_ADDR_0_IDX1;
		addr[2] = MCSC_R_WDMA1_BASE_ADDR_0_IDX2;
		addr[3] = MCSC_R_WDMA1_BASE_ADDR_0_IDX3;
		addr[4] = MCSC_R_WDMA1_BASE_ADDR_0_IDX4;
		addr[5] = MCSC_R_WDMA1_BASE_ADDR_0_IDX5;
		addr[6] = MCSC_R_WDMA1_BASE_ADDR_0_IDX6;
		addr[7] = MCSC_R_WDMA1_BASE_ADDR_0_IDX7;
		break;
	case MCSC_OUTPUT2:
		addr[0] = MCSC_R_WDMA2_BASE_ADDR_0;
		addr[1] = MCSC_R_WDMA2_BASE_ADDR_0_IDX1;
		addr[2] = MCSC_R_WDMA2_BASE_ADDR_0_IDX2;
		addr[3] = MCSC_R_WDMA2_BASE_ADDR_0_IDX3;
		addr[4] = MCSC_R_WDMA2_BASE_ADDR_0_IDX4;
		addr[5] = MCSC_R_WDMA2_BASE_ADDR_0_IDX5;
		addr[6] = MCSC_R_WDMA2_BASE_ADDR_0_IDX6;
		addr[7] = MCSC_R_WDMA2_BASE_ADDR_0_IDX7;
		break;
	default:
		panic("invalid output_id(%d)", output_id);
		break;
	}
}

void is_scaler_set_wdma_addr(void __iomem *base_addr, u32 output_id,
	u32 y_addr, u32 cb_addr, u32 cr_addr, int buf_index)
{
	u32 addr[8] = {0, };

	get_wdma_addr_arr(output_id, addr);
	if (!addr[0])
		return;

	is_hw_set_reg(base_addr, &mcsc_regs[addr[buf_index]], y_addr);
	is_hw_set_reg(base_addr, &mcsc_regs[addr[buf_index] + 1], cb_addr);
	is_hw_set_reg(base_addr, &mcsc_regs[addr[buf_index] + 2], cr_addr);
}

void is_scaler_set_wdma_2bit_addr(void __iomem *base_addr, u32 output_id,
	u32 y_2bit_addr, u32 cbcr_2bit_addr, int buf_index)
{
	/* Not support */
}

void is_scaler_get_wdma_addr(void __iomem *base_addr, u32 output_id,
	u32 *y_addr, u32 *cb_addr, u32 *cr_addr, int buf_index)
{
	u32 addr[8] = {0, };

	get_wdma_addr_arr(output_id, addr);
	if (!addr[buf_index])
		return;

	*y_addr = is_hw_get_reg(base_addr, &mcsc_regs[addr[buf_index]]);
	*cb_addr = is_hw_get_reg(base_addr, &mcsc_regs[addr[buf_index] + 1]);
	*cr_addr = is_hw_get_reg(base_addr, &mcsc_regs[addr[buf_index] + 2]);
}

void is_scaler_clear_rdma_addr(void __iomem *base_addr)
{
	/* not support */
}

void is_scaler_clear_wdma_addr(void __iomem *base_addr, u32 output_id)
{
	u32 addr[8] = {0, }, buf_index;

	get_wdma_addr_arr(output_id, addr);
	for (buf_index = 0; buf_index < 8; buf_index++) {
		if (!addr[buf_index])
			return;

		is_hw_set_reg(base_addr, &mcsc_regs[addr[buf_index]], 0x0);
		is_hw_set_reg(base_addr, &mcsc_regs[addr[buf_index] + 1], 0x0);
		is_hw_set_reg(base_addr, &mcsc_regs[addr[buf_index] + 2], 0x0);
	}
}

/* for hwfc */
void is_scaler_set_hwfc_auto_clear(void __iomem *base_addr, u32 output_id, bool auto_clear)
{
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_HWFC_ENABLE_AUTO_CLEAR],
			&mcsc_fields[MCSC_F_HWFC_ENABLE_AUTO_CLEAR], auto_clear);
}

void is_scaler_set_hwfc_idx_reset(void __iomem *base_addr, u32 output_id, bool reset)
{
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_HWFC_INDEX_RESET],
			&mcsc_fields[MCSC_F_HWFC_INDEX_RESET], reset);
}

void is_scaler_set_hwfc_mode(void __iomem *base_addr, u32 hwfc_output_ids)
{
	u32 val = MCSC_HWFC_MODE_OFF;
	u32 read_val;

	read_val = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_HWFC_MODE],
			&mcsc_fields[MCSC_F_HWFC_MODE]);

	if ((hwfc_output_ids & (1 << MCSC_OUTPUT1)) && (hwfc_output_ids & (1 << MCSC_OUTPUT2))) {
		val = MCSC_HWFC_MODE_REGION_A_B_PORT;
	} else if (hwfc_output_ids & (1 << MCSC_OUTPUT1)) {
		val = MCSC_HWFC_MODE_REGION_A_PORT;
	} else if (hwfc_output_ids & (1 << MCSC_OUTPUT2)) {
		err_hw("set_hwfc_mode: invalid output_ids(0x%x)\n", hwfc_output_ids);
		return;
	}

	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_HWFC_MODE],
			&mcsc_fields[MCSC_F_HWFC_MODE], val);

	dbg_hw(2, "set_hwfc_mode: regs(0x%x)(0x%x), hwfc_ids(0x%x)\n",
		read_val, val, hwfc_output_ids);
}

void is_scaler_set_hwfc_config(void __iomem *base_addr,
	u32 output_id, u32 fmt, u32 plane, u32 dma_idx, u32 width, u32 height)
{
	u32 val = 0;
	u32 img_resol = width * height;
	u32 total_img_byte0 = 0;
	u32 total_img_byte1 = 0;
	u32 total_img_byte2 = 0;
	u32 total_width_byte0 = 0;
	u32 total_width_byte1 = 0;
	u32 total_width_byte2 = 0;
	enum is_mcsc_hwfc_format hwfc_fmt = 0;

	switch (fmt) {
	case DMA_INOUT_FORMAT_YUV422:
		switch (plane) {
		case 3:
			total_img_byte0 = img_resol;
			total_img_byte1 = img_resol >> 1;
			total_img_byte2 = img_resol >> 1;
			total_width_byte0 = width;
			total_width_byte1 = width >> 1;
			total_width_byte2 = width >> 1;
			break;
		case 2:
			total_img_byte0 = img_resol;
			total_img_byte1 = img_resol;
			total_width_byte0 = width;
			total_width_byte1 = width;
			break;
		case 1:
			total_img_byte0 = img_resol << 1;
			total_width_byte0 = width << 1;
			break;
		default:
			err_hw("invalid hwfc plane (%d, %d, %dx%d)",
				fmt, plane, width, height);
			return;
		}

		hwfc_fmt = MCSC_HWFC_FMT_YUV444_YUV422;
		break;
	case DMA_INOUT_FORMAT_YUV420:
		switch (plane) {
		case 3:
			total_img_byte0 = img_resol;
			total_img_byte1 = img_resol >> 2;
			total_img_byte2 = img_resol >> 2;
			total_width_byte0 = width;
			total_width_byte1 = width >> 2;
			total_width_byte2 = width >> 2;
			break;
		case 2:
			total_img_byte0 = img_resol;
			total_img_byte1 = img_resol >> 1;
			total_width_byte0 = width;
			total_width_byte1 = width >> 1;
			break;
		default:
			err_hw("invalid hwfc plane (%d, %d, %dx%d)",
				fmt, plane, width, height);
			return;
		}

		hwfc_fmt = MCSC_HWFC_FMT_YUV420;
		break;
	default:
		err_hw("invalid hwfc format (%d, %d, %dx%d)",
			fmt, plane, width, height);
		return;
	}

	switch (output_id) {
	case MCSC_OUTPUT0:
		break;
	case MCSC_OUTPUT1:
		val = is_hw_get_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_CONFIG_IMAGE_A]);
		/* format */
		val = is_hw_set_field_value(val, &mcsc_fields[MCSC_F_HWFC_FORMAT_A], hwfc_fmt);

		/* plane */
		val = is_hw_set_field_value(val, &mcsc_fields[MCSC_F_HWFC_PLANE_A], plane);

		/* dma idx */
		val = is_hw_set_field_value(val, &mcsc_fields[MCSC_F_HWFC_ID0_A], dma_idx);
		val = is_hw_set_field_value(val, &mcsc_fields[MCSC_F_HWFC_ID1_A], dma_idx+2);
		val = is_hw_set_field_value(val, &mcsc_fields[MCSC_F_HWFC_ID2_A], dma_idx+4);

		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_CONFIG_IMAGE_A], val);

		/* total pixel/byte */
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_TOTAL_IMAGE_BYTE0_A], total_img_byte0);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_TOTAL_IMAGE_BYTE1_A], total_img_byte1);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_TOTAL_IMAGE_BYTE2_A], total_img_byte2);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_TOTAL_WIDTH_BYTE0_A], total_width_byte0);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_TOTAL_WIDTH_BYTE1_A], total_width_byte1);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_TOTAL_WIDTH_BYTE2_A], total_width_byte2);
		break;
	case MCSC_OUTPUT2:
		val = is_hw_get_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_CONFIG_IMAGE_B]);
		/* format */
		val = is_hw_set_field_value(val, &mcsc_fields[MCSC_F_HWFC_FORMAT_B], hwfc_fmt);

		/* plane */
		val = is_hw_set_field_value(val, &mcsc_fields[MCSC_F_HWFC_PLANE_B], plane);

		/* dma idx */
		val = is_hw_set_field_value(val, &mcsc_fields[MCSC_F_HWFC_ID0_B], dma_idx);
		val = is_hw_set_field_value(val, &mcsc_fields[MCSC_F_HWFC_ID1_B], dma_idx+2);
		val = is_hw_set_field_value(val, &mcsc_fields[MCSC_F_HWFC_ID2_B], dma_idx+4);

		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_CONFIG_IMAGE_B], val);

		/* total pixel/byte */
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_TOTAL_IMAGE_BYTE0_B], total_img_byte0);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_TOTAL_IMAGE_BYTE1_B], total_img_byte1);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_TOTAL_IMAGE_BYTE2_B], total_img_byte2);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_TOTAL_WIDTH_BYTE0_B], total_width_byte0);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_TOTAL_WIDTH_BYTE1_B], total_width_byte1);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_HWFC_TOTAL_WIDTH_BYTE2_B], total_width_byte2);
		break;
	default:
		break;
	}
}

u32 is_scaler_get_hwfc_idx_bin(void __iomem *base_addr, u32 output_id)
{
	u32 val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		break;
	case MCSC_OUTPUT1:
		val = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_HWFC_REGION_IDX_BIN],
			&mcsc_fields[MCSC_F_HWFC_REGION_IDX_BIN_A]);
		break;
	case MCSC_OUTPUT2:
		val = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_HWFC_REGION_IDX_BIN],
			&mcsc_fields[MCSC_F_HWFC_REGION_IDX_BIN_B]);
		break;
	default:
		break;
	}

	return val;
}

u32 is_scaler_get_hwfc_cur_idx(void __iomem *base_addr, u32 output_id)
{
	u32 val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		break;
	case MCSC_OUTPUT1:
		val = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_HWFC_CURR_REGION],
			&mcsc_fields[MCSC_F_HWFC_CURR_REGION_A]);
		break;
	case MCSC_OUTPUT2:
		 val = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_HWFC_CURR_REGION],
			&mcsc_fields[MCSC_F_HWFC_CURR_REGION_B]);
		break;
	default:
		break;
	}

	return val;
}

/* for DJAG */
void is_scaler_set_djag_enable(void __iomem *base_addr, u32 djag_enable)
{
	u32 reg_val = 0;

	reg_val = is_hw_get_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_CTRL]);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_ENABLE], djag_enable);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_PS_ENABLE], djag_enable);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_CTRL], reg_val);
}

void is_scaler_set_djag_input_size(void __iomem *base_addr, u32 width, u32 height)
{
	u32 reg_val = 0;

	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_INPUT_IMG_HSIZE], width);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_INPUT_IMG_VSIZE], height);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_IMG_SIZE], reg_val);
}

void is_scaler_set_djag_src_size(void __iomem *base_addr, u32 pos_x, u32 pos_y, u32 width, u32 height)
{
	u32 reg_val = 0;

	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_PS_SRC_HPOS], pos_x);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_PS_SRC_VPOS], pos_y);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_PS_SRC_POS], reg_val);

	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_PS_SRC_HSIZE], width);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_PS_SRC_VSIZE], height);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_PS_SRC_SIZE], reg_val);
}

void is_scaler_set_djag_dst_size(void __iomem *base_addr, u32 width, u32 height)
{
	u32 reg_val = 0;

	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_PS_DST_HSIZE], width);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_PS_DST_VSIZE], height);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_PS_DST_SIZE], reg_val);

	/* High frequency component size should be same with pre-scaled image size in DJAG. */
	is_scaler_set_djag_hf_img_size(base_addr, width, height);
}

void is_scaler_set_djag_scaling_ratio(void __iomem *base_addr, u32 hratio, u32 vratio)
{
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_PS_H_RATIO],
		&mcsc_fields[MCSC_F_DJAG_PS_H_RATIO], hratio);
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_PS_V_RATIO],
		&mcsc_fields[MCSC_F_DJAG_PS_V_RATIO], vratio);
}

void is_scaler_set_djag_init_phase_offset(void __iomem *base_addr, u32 h_offset, u32 v_offset)
{
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_PS_H_INIT_PHASE_OFFSET],
		&mcsc_fields[MCSC_F_DJAG_PS_H_INIT_PHASE_OFFSET], h_offset);
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_PS_V_INIT_PHASE_OFFSET],
		&mcsc_fields[MCSC_F_DJAG_PS_V_INIT_PHASE_OFFSET], v_offset);
}

void is_scaler_set_djag_round_mode(void __iomem *base_addr, u32 round_enable)
{
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_PS_ROUND_MODE],
		&mcsc_fields[MCSC_F_DJAG_PS_ROUND_MODE], round_enable);
}

void is_scaler_set_djag_tuning_param(void __iomem *base_addr,
	const struct djag_scaling_ratio_depended_config *djag_tune)
{
	u32 reg_val = 0;

	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_XFILTER_DEJAGGING_WEIGHT0],
		djag_tune->xfilter_dejagging_coeff_cfg.xfilter_dejagging_weight0);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_XFILTER_DEJAGGING_WEIGHT1],
		djag_tune->xfilter_dejagging_coeff_cfg.xfilter_dejagging_weight1);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_XFILTER_HF_BOOST_WEIGHT],
		djag_tune->xfilter_dejagging_coeff_cfg.xfilter_hf_boost_weight);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_CENTER_HF_BOOST_WEIGHT],
		djag_tune->xfilter_dejagging_coeff_cfg.center_hf_boost_weight);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DIAGONAL_HF_BOOST_WEIGHT],
		djag_tune->xfilter_dejagging_coeff_cfg.diagonal_hf_boost_weight);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_CENTER_WEIGHTED_MEAN_WEIGHT],
		djag_tune->xfilter_dejagging_coeff_cfg.center_weighted_mean_weight);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_XFILTER_DEJAGGING_COEFF], reg_val);

	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_THRES_1X5_MATCHING_SAD],
		djag_tune->thres_1x5_matching_cfg.thres_1x5_matching_sad);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_THRES_1X5_ABSHF],
		djag_tune->thres_1x5_matching_cfg.thres_1x5_abshf);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_THRES_1X5_MATCHING], reg_val);

	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_THRES_SHOOTING_LLCRR],
		djag_tune->thres_shooting_detect_cfg.thres_shooting_llcrr);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_THRES_SHOOTING_LCR],
		djag_tune->thres_shooting_detect_cfg.thres_shooting_lcr);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_THRES_SHOOTING_NEIGHBOR],
		djag_tune->thres_shooting_detect_cfg.thres_shooting_neighbor);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_THRES_SHOOTING_DETECT_0], reg_val);

	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_THRES_SHOOTING_UUCDD],
		djag_tune->thres_shooting_detect_cfg.thres_shooting_uucdd);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_THRES_SHOOTING_UCD],
		djag_tune->thres_shooting_detect_cfg.thres_shooting_ucd);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_MIN_MAX_WEIGHT],
		djag_tune->thres_shooting_detect_cfg.min_max_weight);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_THRES_SHOOTING_DETECT_1], reg_val);

	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_LFSR_SEED_0], &mcsc_fields[MCSC_F_DJAG_LFSR_SEED_0],
		djag_tune->lfsr_seed_cfg.lfsr_seed_0);
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_LFSR_SEED_1], &mcsc_fields[MCSC_F_DJAG_LFSR_SEED_1],
		djag_tune->lfsr_seed_cfg.lfsr_seed_1);
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_LFSR_SEED_2], &mcsc_fields[MCSC_F_DJAG_LFSR_SEED_2],
		djag_tune->lfsr_seed_cfg.lfsr_seed_2);

	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_VALUE_0],
		djag_tune->dither_cfg.dither_value[0]);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_VALUE_1],
		djag_tune->dither_cfg.dither_value[1]);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_VALUE_2],
		djag_tune->dither_cfg.dither_value[2]);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_VALUE_3],
		djag_tune->dither_cfg.dither_value[3]);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_VALUE_4],
		djag_tune->dither_cfg.dither_value[4]);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_DITHER_VALUE_04], reg_val);

	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_VALUE_5],
		djag_tune->dither_cfg.dither_value[5]);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_VALUE_6],
		djag_tune->dither_cfg.dither_value[6]);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_VALUE_7],
		djag_tune->dither_cfg.dither_value[7]);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_VALUE_8],
		djag_tune->dither_cfg.dither_value[8]);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_DITHER_VALUE_58], reg_val);

	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_SAT_CTRL],
		djag_tune->dither_cfg.sat_ctrl);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_THRES],
		djag_tune->dither_cfg.dither_thres);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_DITHER_THRES], reg_val);

	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_CP_HF_THRES], &mcsc_fields[MCSC_F_DJAG_CP_HF_THRES],
		djag_tune->cp_cfg.cp_hf_thres);

	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_CP_ARBI_MAX_COV_OFFSET],
		djag_tune->cp_cfg.cp_arbi_max_cov_offset);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_CP_ARBI_MAX_COV_SHIFT],
		djag_tune->cp_cfg.cp_arbi_max_cov_shift);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_CP_ARBI_DENOM],
		djag_tune->cp_cfg.cp_arbi_denom);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_CP_ARBI_MODE],
		djag_tune->cp_cfg.cp_arbi_mode);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_CP_ARBI], reg_val);
}

void is_scaler_set_djag_dither_wb(void __iomem *base_addr, struct djag_wb_thres_cfg *djag_wb, u32 wht, u32 blk)
{
	u32 reg_val = 0;

#if !defined(CONFIG_SOC_EXYNOS9820_EVT0)
	if (!djag_wb)
#endif
		return;

	reg_val = is_hw_get_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_DITHER_WB]);

	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_WHITE_LEVEL], wht);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_BLACK_LEVEL], blk);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_DITHER_WB_THRES],
		djag_wb->dither_wb_thres);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_DITHER_WB], reg_val);
}

void is_scaler_set_djag_hf_enable(void __iomem *base_addr, u32 hf_enable, u32 hf_scaler_enable)
{
	/* not support */
}

void is_scaler_set_djag_hf_img_size(void __iomem *base_addr, u32 width, u32 height)
{
	/* not support */
}

void is_scaler_set_djag_hf_cfg(void __iomem *base_addr, struct hf_cfg_by_ni *hf_cfg)
{
	/* not support */
}

/* for CAC */
void is_scaler_set_cac_enable(void __iomem *base_addr, u32 en)
{
	/* not support */
}

void is_scaler_set_cac_map_crt_thr(void __iomem *base_addr, struct cac_cfg_by_ni *cfg)
{
	/* not support */
}

/* LFRO : Less Fast Read Out */
void is_scaler_set_lfro_mode_enable(void __iomem *base_addr, u32 hw_id, u32 lfro_enable, u32 lfro_total_fnum)
{
	u32 reg_value = 0;

	switch (hw_id) {
	case DEV_HW_MCSC0:
		reg_value = is_hw_set_field_value(reg_value, &mcsc_fields[MCSC_F_FAST_MODE_NUM_MINUS1],
			lfro_total_fnum - 1);
		reg_value = is_hw_set_field_value(reg_value, &mcsc_fields[MCSC_F_FAST_MODE_EN],
			lfro_enable);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_FAST_MODE_CTRL], reg_value);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}
}

u32 is_scaler_get_lfro_mode_status(void __iomem *base_addr, u32 hw_id)
{
	u32 ret = 0;
	u32 fcnt = 0;

	switch (hw_id) {
	case DEV_HW_MCSC0:
		fcnt = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SCALER_FAST_MODE_STATUS],
			&mcsc_fields[MCSC_F_FAST_MODE_FRAME_CNT]);
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SCALER_FAST_MODE_STATUS],
			&mcsc_fields[MCSC_F_FAST_MODE_ERROR_STATUS]);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}

	if (ret)
		warn_hw("[FRO:%d]frame status: (0x%x)\n", fcnt, ret);

	return ret;
}

u32 is_scaler_get_stripe_align(u32 sbwc_type)
{
	return MCSC_WIDTH_ALIGN;
}

/* for Strip */
u32 is_scaler_get_djag_strip_enable(void __iomem *base_addr, u32 output_id)
{
	int ret = 0;

	ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_DJAG_CTRL], &mcsc_fields[MCSC_F_DJAG_PS_STRIP_ENABLE]);
	return ret;
}

void is_scaler_set_djag_strip_enable(void __iomem *base_addr, u32 output_id, u32 enable)
{
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_CTRL], &mcsc_fields[MCSC_F_DJAG_PS_STRIP_ENABLE], enable);
}

u32 is_scaler_get_djag_out_crop_enable(void __iomem *base_addr, u32 output_id)
{
	int ret = 0;

	ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_DJAG_CTRL], &mcsc_fields[MCSC_F_DJAG_OUT_CROP_ENABLE]);
	return ret;
}

void is_scaler_set_djag_out_crop_enable(void __iomem *base_addr, u32 output_id, u32 enable, u32 pre_dst_h, u32 start_pos_h)
{
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_CTRL], &mcsc_fields[MCSC_F_DJAG_OUT_CROP_ENABLE], enable);
}

void is_scaler_set_djag_out_crop_size(void __iomem *base_addr, u32 output_id,
	u32 pos_x, u32 pos_y, u32 width, u32 height)
{
	u32 reg_val = 0;

	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_OUT_CROP_POS_H], pos_x);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_OUT_CROP_POS_V], pos_y);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_OUT_CROP_POS], reg_val);

	reg_val = 0;
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_OUT_CROP_SIZE_H], width);
	reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_DJAG_OUT_CROP_SIZE_V], height);
	is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_DJAG_OUT_CROP_SIZE], reg_val);
}

void is_scaler_get_djag_strip_config(void __iomem *base_addr, u32 output_id, u32 *pre_dst_h, u32 *start_pos_h)
{
	*pre_dst_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_DJAG_PS_STRIP_PRE_DST_SIZE],
			&mcsc_fields[MCSC_F_DJAG_PS_STRIP_PRE_DST_SIZE_H]);
	*start_pos_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_DJAG_PS_STRIP_IN_START_POS],
			&mcsc_fields[MCSC_F_DJAG_PS_STRIP_IN_START_POS_H]);
}

void is_scaler_set_djag_strip_config(void __iomem *base_addr, u32 output_id, u32 pre_dst_h, u32 start_pos_h)
{
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_PS_STRIP_PRE_DST_SIZE],
		&mcsc_fields[MCSC_F_DJAG_PS_STRIP_PRE_DST_SIZE_H], pre_dst_h);
	is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_DJAG_PS_STRIP_IN_START_POS],
		&mcsc_fields[MCSC_F_DJAG_PS_STRIP_IN_START_POS_H], start_pos_h);
}

u32 is_scaler_get_poly_strip_enable(void __iomem *base_addr, u32 output_id)
{
	int ret = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC0_CTRL],
			&mcsc_fields[MCSC_F_SC0_STRIP_ENABLE]);
		break;
	case MCSC_OUTPUT1:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC1_CTRL],
			&mcsc_fields[MCSC_F_SC1_STRIP_ENABLE]);
		break;
	case MCSC_OUTPUT2:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC2_CTRL],
			&mcsc_fields[MCSC_F_SC2_STRIP_ENABLE]);
		break;
	default:
		break;
	}

	return ret;
}

void is_scaler_set_poly_strip_enable(void __iomem *base_addr, u32 output_id, u32 enable)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC0_CTRL], &mcsc_fields[MCSC_F_SC0_STRIP_ENABLE], enable);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC1_CTRL], &mcsc_fields[MCSC_F_SC1_STRIP_ENABLE], enable);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC2_CTRL], &mcsc_fields[MCSC_F_SC2_STRIP_ENABLE], enable);
		break;
	default:
		break;
	}
}

void is_scaler_get_poly_strip_config(void __iomem *base_addr, u32 output_id, u32 *pre_dst_h, u32 *start_pos_h)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		*pre_dst_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC0_STRIP_PRE_DST_SIZE],
			&mcsc_fields[MCSC_F_SC0_STRIP_PRE_DST_SIZE_H]);
		*start_pos_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC0_STRIP_IN_START_POS],
			&mcsc_fields[MCSC_F_SC0_STRIP_IN_START_POS_H]);
		break;
	case MCSC_OUTPUT1:
		*pre_dst_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC1_STRIP_PRE_DST_SIZE],
			&mcsc_fields[MCSC_F_SC1_STRIP_PRE_DST_SIZE_H]);
		*start_pos_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC1_STRIP_IN_START_POS],
			&mcsc_fields[MCSC_F_SC1_STRIP_IN_START_POS_H]);
		break;
	case MCSC_OUTPUT2:
		*pre_dst_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC2_STRIP_PRE_DST_SIZE],
			&mcsc_fields[MCSC_F_SC2_STRIP_PRE_DST_SIZE_H]);
		*start_pos_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC2_STRIP_IN_START_POS],
			&mcsc_fields[MCSC_F_SC2_STRIP_IN_START_POS_H]);
		break;
	default:
		break;
	}
}

void is_scaler_set_poly_strip_config(void __iomem *base_addr, u32 output_id, u32 pre_dst_h, u32 start_pos_h)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC0_STRIP_PRE_DST_SIZE], &mcsc_fields[MCSC_F_SC0_STRIP_PRE_DST_SIZE_H], pre_dst_h);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC0_STRIP_IN_START_POS], &mcsc_fields[MCSC_F_SC0_STRIP_IN_START_POS_H], start_pos_h);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC1_STRIP_PRE_DST_SIZE], &mcsc_fields[MCSC_F_SC1_STRIP_PRE_DST_SIZE_H], pre_dst_h);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC1_STRIP_IN_START_POS], &mcsc_fields[MCSC_F_SC1_STRIP_IN_START_POS_H], start_pos_h);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC2_STRIP_PRE_DST_SIZE], &mcsc_fields[MCSC_F_SC2_STRIP_PRE_DST_SIZE_H], pre_dst_h);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC2_STRIP_IN_START_POS], &mcsc_fields[MCSC_F_SC2_STRIP_IN_START_POS_H], start_pos_h);
		break;
	default:
		break;
	}
}

u32 is_scaler_get_poly_out_crop_enable(void __iomem *base_addr, u32 output_id)
{
	int ret = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC0_CTRL],
			&mcsc_fields[MCSC_F_SC0_OUT_CROP_ENABLE]);
		break;
	case MCSC_OUTPUT1:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC1_CTRL],
			&mcsc_fields[MCSC_F_SC1_OUT_CROP_ENABLE]);
		break;
	case MCSC_OUTPUT2:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC2_CTRL],
			&mcsc_fields[MCSC_F_SC2_OUT_CROP_ENABLE]);
		break;
	default:
		break;
	}

	return ret;
}

void is_scaler_set_poly_out_crop_enable(void __iomem *base_addr, u32 output_id, u32 enable)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC0_CTRL], &mcsc_fields[MCSC_F_SC0_OUT_CROP_ENABLE], enable);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC1_CTRL], &mcsc_fields[MCSC_F_SC1_OUT_CROP_ENABLE], enable);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_SC2_CTRL], &mcsc_fields[MCSC_F_SC2_OUT_CROP_ENABLE], enable);
		break;
	default:
		break;
	}
}

void is_scaler_get_poly_out_crop_size(void __iomem *base_addr, u32 output_id, u32 *width, u32 *height)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC0_OUT_CROP_SIZE],
			&mcsc_fields[MCSC_F_SC0_OUT_CROP_SIZE_H]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC0_OUT_CROP_SIZE],
			&mcsc_fields[MCSC_F_SC0_OUT_CROP_SIZE_V]);
		break;
	case MCSC_OUTPUT1:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC1_OUT_CROP_SIZE],
			&mcsc_fields[MCSC_F_SC1_OUT_CROP_SIZE_H]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC1_OUT_CROP_SIZE],
			&mcsc_fields[MCSC_F_SC1_OUT_CROP_SIZE_V]);
		break;
	case MCSC_OUTPUT2:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC2_OUT_CROP_SIZE],
			&mcsc_fields[MCSC_F_SC2_OUT_CROP_SIZE_H]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_SC2_OUT_CROP_SIZE],
			&mcsc_fields[MCSC_F_SC2_OUT_CROP_SIZE_V]);
		break;
	default:
		break;
	}
}

void is_scaler_set_poly_out_crop_size(void __iomem *base_addr, u32 output_id,
	u32 pos_x, u32 pos_y, u32 width, u32 height)
{
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC0_OUT_CROP_POS_H], pos_x);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC0_OUT_CROP_POS_V], pos_y);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC0_OUT_CROP_POS], reg_val);

		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC0_OUT_CROP_SIZE_H], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC0_OUT_CROP_SIZE_V], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC0_OUT_CROP_SIZE], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC1_OUT_CROP_POS_H], pos_x);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC1_OUT_CROP_POS_V], pos_y);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC1_OUT_CROP_POS], reg_val);

		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC1_OUT_CROP_SIZE_H], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC1_OUT_CROP_SIZE_V], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC1_OUT_CROP_SIZE], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC2_OUT_CROP_POS_H], pos_x);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC2_OUT_CROP_POS_V], pos_y);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC2_OUT_CROP_POS], reg_val);

		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC2_OUT_CROP_SIZE_H], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_SC2_OUT_CROP_SIZE_V], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SC2_OUT_CROP_SIZE], reg_val);
		break;
	default:
		break;
	}
}

u32 is_scaler_get_post_strip_enable(void __iomem *base_addr, u32 output_id)
{
	int ret = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC0_CTRL],
			&mcsc_fields[MCSC_F_PC0_STRIP_ENABLE]);
		break;
	case MCSC_OUTPUT1:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC1_CTRL],
			&mcsc_fields[MCSC_F_PC1_STRIP_ENABLE]);
		break;
	case MCSC_OUTPUT2:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC2_CTRL],
			&mcsc_fields[MCSC_F_PC2_STRIP_ENABLE]);
		break;
	default:
		break;
	}

	return ret;
}

void is_scaler_set_post_strip_enable(void __iomem *base_addr, u32 output_id, u32 enable)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_CTRL], &mcsc_fields[MCSC_F_PC0_STRIP_ENABLE], enable);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_CTRL], &mcsc_fields[MCSC_F_PC1_STRIP_ENABLE], enable);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_CTRL], &mcsc_fields[MCSC_F_PC2_STRIP_ENABLE], enable);
		break;
	default:
		break;
	}
}

void is_scaler_get_post_strip_config(void __iomem *base_addr, u32 output_id, u32 *pre_dst_h, u32 *start_pos_h)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		*pre_dst_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC0_STRIP_PRE_DST_SIZE],
			&mcsc_fields[MCSC_F_PC0_STRIP_PRE_DST_SIZE_H]);
		*start_pos_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC0_STRIP_IN_START_POS],
			&mcsc_fields[MCSC_F_PC0_STRIP_IN_START_POS_H]);
		break;
	case MCSC_OUTPUT1:
		*pre_dst_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC1_STRIP_PRE_DST_SIZE],
			&mcsc_fields[MCSC_F_PC1_STRIP_PRE_DST_SIZE_H]);
		*start_pos_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC1_STRIP_IN_START_POS],
			&mcsc_fields[MCSC_F_PC1_STRIP_IN_START_POS_H]);
		break;
	case MCSC_OUTPUT2:
		*pre_dst_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC2_STRIP_PRE_DST_SIZE],
			&mcsc_fields[MCSC_F_PC2_STRIP_PRE_DST_SIZE_H]);
		*start_pos_h = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC2_STRIP_IN_START_POS],
			&mcsc_fields[MCSC_F_PC2_STRIP_IN_START_POS_H]);
		break;
	default:
		break;
	}
}

void is_scaler_set_post_strip_config(void __iomem *base_addr, u32 output_id, u32 pre_dst_h, u32 start_pos_h)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_STRIP_PRE_DST_SIZE], &mcsc_fields[MCSC_F_PC0_STRIP_PRE_DST_SIZE_H], pre_dst_h);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_STRIP_IN_START_POS], &mcsc_fields[MCSC_F_PC0_STRIP_IN_START_POS_H], start_pos_h);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_STRIP_PRE_DST_SIZE], &mcsc_fields[MCSC_F_PC1_STRIP_PRE_DST_SIZE_H], pre_dst_h);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_STRIP_IN_START_POS], &mcsc_fields[MCSC_F_PC1_STRIP_IN_START_POS_H], start_pos_h);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_STRIP_PRE_DST_SIZE], &mcsc_fields[MCSC_F_PC2_STRIP_PRE_DST_SIZE_H], pre_dst_h);
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_STRIP_IN_START_POS], &mcsc_fields[MCSC_F_PC2_STRIP_IN_START_POS_H], start_pos_h);
		break;
	default:
		break;
	}
}

u32 is_scaler_get_post_out_crop_enable(void __iomem *base_addr, u32 output_id)
{
	int ret = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC0_CTRL],
			&mcsc_fields[MCSC_F_PC0_OUT_CROP_ENABLE]);
		break;
	case MCSC_OUTPUT1:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC1_CTRL],
			&mcsc_fields[MCSC_F_PC1_OUT_CROP_ENABLE]);
		break;
	case MCSC_OUTPUT2:
		ret = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC2_CTRL],
			&mcsc_fields[MCSC_F_PC2_OUT_CROP_ENABLE]);
		break;
	default:
		break;
	}

	return ret;
}

void is_scaler_set_post_out_crop_enable(void __iomem *base_addr, u32 output_id, u32 enable)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC0_CTRL], &mcsc_fields[MCSC_F_PC0_OUT_CROP_ENABLE], enable);
		break;
	case MCSC_OUTPUT1:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC1_CTRL], &mcsc_fields[MCSC_F_PC1_OUT_CROP_ENABLE], enable);
		break;
	case MCSC_OUTPUT2:
		is_hw_set_field(base_addr, &mcsc_regs[MCSC_R_PC2_CTRL], &mcsc_fields[MCSC_F_PC2_OUT_CROP_ENABLE], enable);
		break;
	default:
		break;
	}
}

void is_scaler_get_post_out_crop_size(void __iomem *base_addr, u32 output_id, u32 *width, u32 *height)
{
	switch (output_id) {
	case MCSC_OUTPUT0:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC0_OUT_CROP_SIZE],
			&mcsc_fields[MCSC_F_PC0_OUT_CROP_SIZE_H]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC0_OUT_CROP_SIZE],
			&mcsc_fields[MCSC_F_PC0_OUT_CROP_SIZE_V]);
		break;
	case MCSC_OUTPUT1:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC1_OUT_CROP_SIZE],
			&mcsc_fields[MCSC_F_PC1_OUT_CROP_SIZE_H]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC1_OUT_CROP_SIZE],
			&mcsc_fields[MCSC_F_PC1_OUT_CROP_SIZE_V]);
		break;
	case MCSC_OUTPUT2:
		*width = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC2_OUT_CROP_SIZE],
			&mcsc_fields[MCSC_F_PC2_OUT_CROP_SIZE_H]);
		*height = is_hw_get_field(base_addr, &mcsc_regs[MCSC_R_PC2_OUT_CROP_SIZE],
			&mcsc_fields[MCSC_F_PC2_OUT_CROP_SIZE_V]);
		break;
	default:
		break;
	}
}

void is_scaler_set_post_out_crop_size(void __iomem *base_addr, u32 output_id,
	u32 pos_x, u32 pos_y, u32 width, u32 height)
{
	u32 reg_val = 0;

	switch (output_id) {
	case MCSC_OUTPUT0:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_OUT_CROP_POS_H], pos_x);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_OUT_CROP_POS_V], pos_y);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC0_OUT_CROP_POS], reg_val);

		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_OUT_CROP_SIZE_H], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC0_OUT_CROP_SIZE_V], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC0_OUT_CROP_SIZE], reg_val);
		break;
	case MCSC_OUTPUT1:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_OUT_CROP_POS_H], pos_x);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_OUT_CROP_POS_V], pos_y);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC1_OUT_CROP_POS], reg_val);

		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_OUT_CROP_SIZE_H], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC1_OUT_CROP_SIZE_V], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC1_OUT_CROP_SIZE], reg_val);
		break;
	case MCSC_OUTPUT2:
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_OUT_CROP_POS_H], pos_x);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_OUT_CROP_POS_V], pos_y);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC2_OUT_CROP_POS], reg_val);

		reg_val = 0;
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_OUT_CROP_SIZE_H], width);
		reg_val = is_hw_set_field_value(reg_val, &mcsc_fields[MCSC_F_PC2_OUT_CROP_SIZE_V], height);
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_PC2_OUT_CROP_SIZE], reg_val);
		break;
	default:
		break;
	}
}

/* HF: High Frequency */
void is_scaler_set_hf_scaler_enable(void __iomem *base_addr, u32 hw_id, u32 enable)
{
	/* not supported */
}

void is_scaler_set_hf_scaler_src_size(void __iomem *base_addr,
	u32 pos_x, u32 pos_y, u32 width, u32 height)
{
	/* not supported */
}

void is_scaler_set_hf_scaler_dst_size(void __iomem *base_addr, u32 width, u32 height)
{
	/* not supported */
}

void is_scaler_set_hf_scaler_scaling_ratio(void __iomem *base_addr, u32 hratio, u32 vratio)
{
	/* not supported */
}

void is_scaler_set_hf_scaler_coef(void __iomem *base_addr,
	u32 hratio, u32 vratio, struct scaler_coef_cfg *sc_coef,
	enum exynos_sensor_position sensor_position)
{
	/* not supported */
}

void is_scaler_set_hf_scaler_round_mode(void __iomem *base_addr, u32 mode)
{
	/* not supported */
}

int is_scaler_set_hf_config(void __iomem *base_addr, u32 hw_id,
	bool hf_enable,
	struct param_mcs_output *hf_param,
	struct scaler_coef_cfg *sc_coef, enum exynos_sensor_position sensor_position,
	u32 *payload_size)
{
	/* not supported */
	return 0;
}

int is_scaler_set_votf_config(void __iomem *base_addr, u32 hw_id, bool votf_enable)
{
	return 0;
}

static void is_scaler0_clear_intr_src(void __iomem *base_addr, u32 status)
{
	if (status & (1 << INTR_MC_SCALER_OVERFLOW))
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT],
			(u32)1 << INTR_MC_SCALER_OVERFLOW);

	if (status & (1 << INTR_MC_SCALER_INPUT_VERTICAL_UNF))
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT],
			(u32)1 << INTR_MC_SCALER_INPUT_VERTICAL_UNF);

	if (status & (1 << INTR_MC_SCALER_INPUT_VERTICAL_OVF))
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT],
			(u32)1 << INTR_MC_SCALER_INPUT_VERTICAL_OVF);

	if (status & (1 << INTR_MC_SCALER_INPUT_HORIZONTAL_UNF))
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT],
			(u32)1 << INTR_MC_SCALER_INPUT_HORIZONTAL_UNF);

	if (status & (1 << INTR_MC_SCALER_INPUT_HORIZONTAL_OVF))
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT],
			(u32)1 << INTR_MC_SCALER_INPUT_HORIZONTAL_OVF);

	if (status & (1 << INTR_MC_SCALER_CORE_FINISH))
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT],
			(u32)1 << INTR_MC_SCALER_CORE_FINISH);

	if (status & (1 << INTR_MC_SCALER_WDMA_FINISH))
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT],
			(u32)1 << INTR_MC_SCALER_WDMA_FINISH);

	if (status & (1 << INTR_MC_SCALER_FRAME_START))
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT],
			(u32)1 << INTR_MC_SCALER_FRAME_START);

	if (status & (1 << INTR_MC_SCALER_FRAME_END))
		is_hw_set_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT],
			(u32)1 << INTR_MC_SCALER_FRAME_END);
}

void is_scaler_clear_intr_src(void __iomem *base_addr, u32 hw_id, u32 status)
{
	switch (hw_id) {
	case DEV_HW_MCSC0:
		is_scaler0_clear_intr_src(base_addr, status);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}
}

u32 is_scaler_get_intr_mask(void __iomem *base_addr, u32 hw_id)
{
	int ret = 0;

	switch (hw_id) {
	case DEV_HW_MCSC0:
		ret = is_hw_get_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT_MASK]);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}

	return ret;
}

u32 is_scaler_get_intr_status(void __iomem *base_addr, u32 hw_id)
{
	int ret = 0;

	switch (hw_id) {
	case DEV_HW_MCSC0:
		ret = is_hw_get_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_INTERRUPT]);
		break;
	default:
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);
		break;
	}

	return ret;
}

u32 is_scaler_handle_extended_intr(u32 status)
{
	int ret = 0;
	/* TODO : if recovery logic is needed, implement here. */
	return ret;
}

u32 is_scaler_get_version(void __iomem *base_addr)
{
	return is_hw_get_reg(base_addr, &mcsc_regs[MCSC_R_SCALER_VERSION]);
}

u32 is_scaler_get_idle_status(void __iomem *base_addr, u32 hw_id)
{
	int ret = 0;

	if (hw_id == DEV_HW_MCSC0)
		ret = is_hw_get_field(base_addr,
			&mcsc_regs[MCSC_R_SCALER_RUNNING_STATUS], &mcsc_fields[MCSC_F_SCALER_IDLE]);
	else
		warn_hw("invalid hw_id(%d) for MCSC api\n", hw_id);

	return ret;

}

void is_scaler_dump(void __iomem *base_addr)
{
	info_hw("MCSC ver 10.20");

	is_hw_dump_regs(base_addr, mcsc_regs, MCSC_REG_CNT);
}

void mcsc_hw_s_dtp(void __iomem *base, enum mcsc_dtp_id id, u32 enable,
	enum mcsc_dtp_type type, u32 y, u32 u, u32 v, enum mcsc_dtp_color_bar cb)
{
	/* dummy */
}
