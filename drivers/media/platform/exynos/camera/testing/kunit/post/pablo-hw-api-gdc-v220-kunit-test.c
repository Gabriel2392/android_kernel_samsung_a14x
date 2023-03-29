// SPDX-License-Identifier: GPL-2.0
/*
 * Samsung Exynos SoC series Pablo driver
 *
 * Exynos Pablo image subsystem functions
 *
 * Copyright (c) 2022 Samsung Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "pablo-kunit-test.h"
#include "gdc/pablo-gdc.h"
#include "gdc/pablo-hw-api-gdc.h"
#include "gdc/pablo-hw-reg-gdc-v220.h"
#include "is-common-enum.h"

static struct cameraepp_hw_api_gdc_kunit_test_ctx {
	void *addr;
	struct gdc_dev *gdc;
	struct gdc_crop_param *crop_param;
} test_ctx;

static const struct gdc_variant gdc_variant = {
	.limit_input = {
		.min_w = 96,
		.min_h = 64,
		.max_w = 8192,
		.max_h = 6144,
	},
	.limit_output = {
		.min_w = 96,
		.min_h = 64,
		.max_w = 8192,
		.max_h = 6144,
	},
	.version = 0x0,
};

static const struct gdc_fmt gdc_formats[] = {
	{
		.name		= "YUV 4:2:0 non-contiguous 2-planar, Y/CbCr",
		.pixelformat	= V4L2_PIX_FMT_NV12M,
		.cfg_val	= GDC_CFG_FMT_YCBCR420_2P,
		.bitperpixel	= { 8, 4 },
		.num_planes	= 2,
		.num_comp	= 2,
		.h_shift	= 1,
		.v_shift	= 1,
	}, {
		.name		= "YUV 4:2:0 non-contiguous 2-planar, Y/CrCb",
		.pixelformat	= V4L2_PIX_FMT_NV21M,
		.cfg_val	= GDC_CFG_FMT_YCRCB420_2P,
		.bitperpixel	= { 8, 4 },
		.num_planes	= 2,
		.num_comp	= 2,
		.h_shift	= 1,
		.v_shift	= 1,
	}, {
		.name		= "YUV 4:2:0 contiguous 2-planar, Y/CbCr",
		.pixelformat	= V4L2_PIX_FMT_NV12,
		.cfg_val	= GDC_CFG_FMT_YCBCR420_2P,
		.bitperpixel	= { 12 },
		.num_planes = 1,
		.num_comp	= 2,
		.h_shift	= 1,
		.v_shift	= 1,
	}, {
		.name		= "YUV 4:2:0 contiguous 2-planar, Y/CrCb",
		.pixelformat	= V4L2_PIX_FMT_NV21,
		.cfg_val	= GDC_CFG_FMT_YCRCB420_2P,
		.bitperpixel	= { 12 },
		.num_planes = 1,
		.num_comp	= 2,
		.h_shift	= 1,
		.v_shift	= 1,
	}, {
		.name		= "YUV 4:2:2 packed, YCrYCb",
		.pixelformat	= V4L2_PIX_FMT_YVYU,
		.cfg_val	= GDC_CFG_FMT_YVYU,
		.bitperpixel	= { 16 },
		.num_planes = 1,
		.num_comp	= 1,
		.h_shift	= 1,
	}, {
		.name		= "YUV 4:2:2 packed, YCbYCr",
		.pixelformat	= V4L2_PIX_FMT_YUYV,
		.cfg_val	= GDC_CFG_FMT_YUYV,
		.bitperpixel	= { 16 },
		.num_planes = 1,
		.num_comp	= 1,
		.h_shift	= 1,
	}, {
		.name		= "YUV 4:2:2 contiguous 2-planar, Y/CbCr",
		.pixelformat	= V4L2_PIX_FMT_NV16,
		.cfg_val	= GDC_CFG_FMT_YCBCR422_2P,
		.bitperpixel	= { 16 },
		.num_planes = 1,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV 4:2:2 contiguous 2-planar, Y/CrCb",
		.pixelformat	= V4L2_PIX_FMT_NV61,
		.cfg_val	= GDC_CFG_FMT_YCRCB422_2P,
		.bitperpixel	= { 16 },
		.num_planes = 1,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV 4:2:2 non-contiguous 2-planar, Y/CbCr",
		.pixelformat	= V4L2_PIX_FMT_NV16M,
		.cfg_val	= GDC_CFG_FMT_YCBCR422_2P,
		.bitperpixel	= { 8, 8 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV 4:2:2 non-contiguous 2-planar, Y/CrCb",
		.pixelformat	= V4L2_PIX_FMT_NV61M,
		.cfg_val	= GDC_CFG_FMT_YCRCB422_2P,
		.bitperpixel	= { 8, 8 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "P010_16B",
		.pixelformat	= V4L2_PIX_FMT_NV12M_P010,
		.cfg_val	= GDC_CFG_FMT_P010_16B_2P,
		.bitperpixel	= { 16, 16 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "P010_16B",
		.pixelformat	= V4L2_PIX_FMT_NV21M_P010,
		.cfg_val	= GDC_CFG_FMT_P010_16B_2P,
		.bitperpixel	= { 16, 16 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "P210_16B",
		.pixelformat	= V4L2_PIX_FMT_NV16M_P210,
		.cfg_val	= GDC_CFG_FMT_P210_16B_2P,
		.bitperpixel	= { 16, 16 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "P210_16B",
		.pixelformat	= V4L2_PIX_FMT_NV61M_P210,
		.cfg_val	= GDC_CFG_FMT_P210_16B_2P,
		.bitperpixel	= { 16, 16 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV422 2P 10bit(8+2)",
		.pixelformat	= V4L2_PIX_FMT_NV16M_S10B,
		.cfg_val	= GDC_CFG_FMT_YCRCB422_8P2_2P,
		.bitperpixel	= { 8, 8 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV422 2P 10bit(8+2)",
		.pixelformat	= V4L2_PIX_FMT_NV61M_S10B,
		.cfg_val	= GDC_CFG_FMT_YCRCB422_8P2_2P,
		.bitperpixel	= { 8, 8 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV420 2P 10bit(8+2)",
		.pixelformat	= V4L2_PIX_FMT_NV12M_S10B,
		.cfg_val	= GDC_CFG_FMT_YCRCB420_8P2_2P,
		.bitperpixel	= { 8, 8 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "YUV420 2P 10bit(8+2)",
		.pixelformat	= V4L2_PIX_FMT_NV21M_S10B,
		.cfg_val	= GDC_CFG_FMT_YCRCB420_8P2_2P,
		.bitperpixel	= { 8, 8 },
		.num_planes = 2,
		.num_comp	= 2,
		.h_shift	= 1,
	}, {
		.name		= "NV12M SBWC LOSSY 8bit",
		.pixelformat	= V4L2_PIX_FMT_NV12M_SBWCL_8B,
		.cfg_val	= GDC_CFG_FMT_NV12M_SBWC_LOSSY_8B,
		.bitperpixel	= { 8, 4 },
		.num_planes	= 2,
		.num_comp	= 2,
		.h_shift	= 1,
		.v_shift	= 1,
	}, {
		.name		= "NV12M SBWC LOSSY 10bit",
		.pixelformat	= V4L2_PIX_FMT_NV12M_SBWCL_10B,
		.cfg_val	= GDC_CFG_FMT_NV12M_SBWC_LOSSY_10B,
		.bitperpixel	= { 16, 8 },
		.num_planes	= 2,
		.num_comp	= 2,
		.h_shift	= 1,
		.v_shift	= 1,
	},
};

/* Define the test cases. */
static void camerapp_hw_api_gdc_get_size_constraints_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_dev *gdc = tctx->gdc;
	const struct gdc_variant *constraints;
	u32 val;

	*(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_VER].sfr_offset) = gdc_variant.version;

	constraints = func->camerapp_hw_gdc_get_size_constraints(gdc->pmio);
	val = func->camerapp_hw_gdc_get_ver(gdc->pmio);

	KUNIT_EXPECT_EQ(test, constraints->limit_input.min_w, gdc_variant.limit_input.min_w);
	KUNIT_EXPECT_EQ(test, constraints->limit_input.min_h, gdc_variant.limit_input.min_h);
	KUNIT_EXPECT_EQ(test, constraints->limit_input.max_w, gdc_variant.limit_input.max_w);
	KUNIT_EXPECT_EQ(test, constraints->limit_input.max_h, gdc_variant.limit_input.max_h);
	KUNIT_EXPECT_EQ(test, constraints->limit_output.min_w, gdc_variant.limit_output.min_w);
	KUNIT_EXPECT_EQ(test, constraints->limit_output.min_h, gdc_variant.limit_output.min_h);
	KUNIT_EXPECT_EQ(test, constraints->limit_output.max_w, gdc_variant.limit_output.max_w);
	KUNIT_EXPECT_EQ(test, constraints->limit_output.max_h, gdc_variant.limit_output.max_h);
	KUNIT_EXPECT_EQ(test, constraints->version, gdc_variant.version);
	KUNIT_EXPECT_EQ(test, constraints->version, val);
}

static void camerapp_hw_api_gdc_sfr_dump_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_dev *gdc = tctx->gdc;

	func->camerapp_gdc_sfr_dump(gdc->regs_base);
}

static void camerapp_hw_api_gdc_start_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_dev *gdc = tctx->gdc;
	struct c_loader_buffer clb;
	u32 val;

	func->camerapp_hw_gdc_start(gdc->pmio, &clb);

	val = *(u32 *)(tctx->addr + gdc_regs[GDC_R_FRAMESTART_CMD].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)1);
}

static void camerapp_hw_api_gdc_stop_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_dev *gdc = tctx->gdc;
	u32 val;

	*(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_PROCESSING].sfr_offset )= 0x1;

	func->camerapp_hw_gdc_stop(gdc->pmio);

	val = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_PROCESSING].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)0);
}

static void camerapp_hw_api_gdc_sw_reset_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_dev *gdc = tctx->gdc;
	u32 val1, val2;

	val1 = func->camerapp_hw_gdc_sw_reset(gdc->pmio);

	val2 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_SW_RESET].sfr_offset);
	KUNIT_EXPECT_EQ(test, val1, (u32)10001);
	KUNIT_EXPECT_EQ(test, val2, (u32)0);
}

static void camerapp_hw_gdc_get_intr_status_and_clear_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_dev *gdc = tctx->gdc;
	u32 val, c;
	u32 irq = 0xAAAABBBB;

	*(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_INT_STATUS].sfr_offset) = irq;

	val = func->camerapp_hw_gdc_get_intr_status_and_clear(gdc->pmio);

	c = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_INT_STATUS].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, irq);
	KUNIT_EXPECT_EQ(test, c, irq);
}

static void camerapp_hw_gdc_has_comp_header_kunit_test(struct kunit *test)
{
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	bool val;

	val = func->camerapp_hw_gdc_has_comp_header(0);

	KUNIT_EXPECT_EQ(test, val, (bool)false);
}

static void camerapp_hw_get_comp_buf_size_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_frame frame;
	struct gdc_dev *gdc = tctx->gdc;
	u32 width, height, pixfmt, plane;
	int val1, val2;

	width = 320;
	height = 240;
	pixfmt = V4L2_PIX_FMT_NV12M_SBWCL_8B;
	for(plane = 0; plane < 2; plane ++) {
		val1 = func->camerapp_hw_get_comp_buf_size(gdc, &frame,
			width, height, pixfmt, plane, GDC_SBWC_SIZE_ALL);
		val2 = plane ? 19264 : 38464;
		KUNIT_EXPECT_EQ(test, val1, val2);
	}

	pixfmt = V4L2_PIX_FMT_NV12M_SBWCL_10B;
	for(plane = 0; plane < 2; plane ++) {
		val1 = func->camerapp_hw_get_comp_buf_size(gdc, &frame,
			width, height, pixfmt, plane, GDC_SBWC_SIZE_ALL);
		val2 = plane ? 28864 : 57664;
		KUNIT_EXPECT_EQ(test, val1, val2);
	}
}

static void camerapp_hw_gdc_update_scale_parameters_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_dev *gdc = tctx->gdc;
	struct gdc_crop_param *crop_param = tctx->crop_param;
	struct gdc_frame s_frame, d_frame;
	u32 val1, val2;

	s_frame.width = 320;
	s_frame.height = 240;
	d_frame.width = 320;
	d_frame.height = 240;
	crop_param->is_grid_mode = 0;
	crop_param->is_bypass_mode = 0;
	crop_param->crop_width = 320;
	crop_param->crop_height = 240;
	crop_param->crop_start_x = 0;
	crop_param->crop_start_y = 0;

	/* normal operation */
	func->camerapp_hw_gdc_update_scale_parameters(gdc->pmio, &s_frame, &d_frame, crop_param);

	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_CONFIG].sfr_offset);
	KUNIT_EXPECT_EQ(test, val1, (u32)0);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_INPUT_ORG_SIZE].sfr_offset);
	val2 = (s_frame.height << 16) |s_frame.width;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_INPUT_CROP_START].sfr_offset);
	val2 = (crop_param->crop_start_y << 16) |crop_param->crop_start_x;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_INPUT_CROP_SIZE].sfr_offset);
	val2 = (crop_param->crop_height << 16) |crop_param->crop_width;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_SCALE].sfr_offset);
	val2 = (0xCCCD << 16) | 0xCCCD;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_SCALE_SHIFTER].sfr_offset);
	val2 = (0x3 << 4) | 0x3;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_INV_SCALE].sfr_offset);
	val2 = (0x140 << 16) | 0x140;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_OUT_CROP_START].sfr_offset);
	val2 = (0x0 << 16) | 0x0;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_OUT_CROP_SIZE].sfr_offset);
	val2 = (d_frame.height << 16) | d_frame.width;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_OUT_SCALE].sfr_offset);
	val2 = (0x80 << 16) | 0x80;
	KUNIT_EXPECT_EQ(test, val1, val2);

	/* scale up 2x */
	s_frame.width = 160;
	s_frame.height = 120;
	d_frame.width = 320;
	d_frame.height = 240;
	crop_param->crop_width = 160;
	crop_param->crop_height = 120;
	crop_param->crop_start_x = 0;
	crop_param->crop_start_y = 0;

	func->camerapp_hw_gdc_update_scale_parameters(gdc->pmio, &s_frame, &d_frame, crop_param);

	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_CONFIG].sfr_offset);
	KUNIT_EXPECT_EQ(test, val1, (u32)0);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_INPUT_ORG_SIZE].sfr_offset);
	val2 = (s_frame.height << 16) |s_frame.width;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_INPUT_CROP_START].sfr_offset);
	val2 = (crop_param->crop_start_y << 16) |crop_param->crop_start_x;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_INPUT_CROP_SIZE].sfr_offset);
	val2 = (crop_param->crop_height << 16) |crop_param->crop_width;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_SCALE].sfr_offset);
	val2 = (0xCCCD << 16) | 0xCCCD;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_SCALE_SHIFTER].sfr_offset);
	val2 = (0x2 << 4) | 0x2;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_INV_SCALE].sfr_offset);
	val2 = (0xa0 << 16) | 0xa0;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_OUT_CROP_START].sfr_offset);
	val2 = (0x0 << 16) | 0x0;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_OUT_CROP_SIZE].sfr_offset);
	val2 = (d_frame.height << 16) | d_frame.width;
	KUNIT_EXPECT_EQ(test, val1, val2);
	val1 = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_OUT_SCALE].sfr_offset);
	val2 = (0x40 << 16) | 0x40;
	KUNIT_EXPECT_EQ(test, val1, val2);
}

static void camerapp_hw_gdc_update_grid_param_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_crop_param *crop_param = tctx->crop_param;
	struct gdc_dev *gdc = tctx->gdc;
	u32 cal_sfr_offset;
	u32 val1, val2;
	u32 i, j;
	u32 sfr_offset = 0x0004;
	u32 sfr_start_x = 0x1000;
	u32 sfr_start_y = 0x3000;

	for (i = 0; i < GRID_Y_SIZE; i++) {
		for (j = 0; j < GRID_X_SIZE; j++) {
			crop_param->calculated_grid_x[i][j] = 0xAA;
			crop_param->calculated_grid_y[i][j] = 0xBB;
		}
	}
	crop_param->use_calculated_grid = 0;

	func->camerapp_hw_gdc_update_grid_param(gdc->pmio, crop_param);

	for (i = 0; i < GRID_Y_SIZE; i++) {
		for (j = 0; j < GRID_X_SIZE; j++) {
			cal_sfr_offset = (sfr_offset * i * GRID_X_SIZE) + (sfr_offset * j);
			val1 = *(u32 *)(tctx->addr + sfr_start_x + cal_sfr_offset);
			val2 = *(u32 *)(tctx->addr + sfr_start_y + cal_sfr_offset);
			KUNIT_EXPECT_EQ(test, val1, (u32)0);
			KUNIT_EXPECT_EQ(test, val2, (u32)0);
		}
	}

	crop_param->use_calculated_grid = 1;

	func->camerapp_hw_gdc_update_grid_param(gdc->pmio, crop_param);

	for (i = 0; i < GRID_Y_SIZE; i++) {
		for (j = 0; j < GRID_X_SIZE; j++) {
			cal_sfr_offset = (sfr_offset * i * GRID_X_SIZE) + (sfr_offset * j);
			val1 = *(u32 *)(tctx->addr + sfr_start_x + cal_sfr_offset);
			val2 = *(u32 *)(tctx->addr + sfr_start_y + cal_sfr_offset);
			KUNIT_EXPECT_EQ(test, val1, (u32)crop_param->calculated_grid_x[i][j]);
			KUNIT_EXPECT_EQ(test, val2, (u32)crop_param->calculated_grid_y[i][j]);
		}
	}
}

static void camerapp_hw_gdc_set_format_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_frame s_frame, d_frame;
	struct gdc_dev *gdc = tctx->gdc;
	u32 val;

	s_frame.gdc_fmt = &gdc_formats[1];
	d_frame.gdc_fmt = &gdc_formats[1];

	func->camerapp_hw_gdc_set_format(gdc->pmio, &s_frame, &d_frame);

	val = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_YUV_FORMAT].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)0x61);
}

static void camerapp_hw_gdc_set_dma_address_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_frame s_frame, d_frame;
	struct gdc_dev *gdc = tctx->gdc;
	u64 addr = 0xDEADDEADF;
	u32 val;

	s_frame.gdc_fmt = &gdc_formats[1];
	d_frame.gdc_fmt = &gdc_formats[1];
	s_frame.addr.y = addr;
	s_frame.addr.cb = addr;
	s_frame.addr.y_2bit = addr;
	s_frame.addr.cbcr_2bit = addr;
	d_frame.addr.y = addr;
	d_frame.addr.cb = addr;
	d_frame.addr.y_2bit = addr;
	d_frame.addr.cbcr_2bit = addr;

	func->camerapp_hw_gdc_set_dma_address(gdc->pmio, &s_frame, &d_frame);

	/* WDMA */
	val = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_DST_BASE_LUM].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)(d_frame.addr.y));
	val = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_DST_BASE_CHROMA].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)(d_frame.addr.cb));

	val = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_DST_2BIT_BASE_LUM].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)(d_frame.addr.y_2bit));
	val = *(u32 *)(tctx->addr + gdc_regs[GDC_R_GDC_DST_2BIT_BASE_CHROMA].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)(d_frame.addr.cbcr_2bit));

	/* RDMA */
	val = *(u32 *)(tctx->addr + gdc_regs[GDC_R_PXC_DPB_BASE_LUM].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)(s_frame.addr.y));
	val = *(u32 *)(tctx->addr + gdc_regs[GDC_R_PXC_DPB_BASE_CHROMA].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)(s_frame.addr.cb));
	val = *(u32 *)(tctx->addr + gdc_regs[GDC_R_PXC_DPB_2BIT_BASE_LUM].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)(s_frame.addr.y_2bit));
	val = *(u32 *)(tctx->addr + gdc_regs[GDC_R_PXC_DPB_2BIT_BASE_CHROMA].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)(s_frame.addr.cbcr_2bit));
}

static void camerapp_hw_gdc_update_param_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_dev *gdc = tctx->gdc;
	struct gdc_ctx *ctx;

	ctx = kunit_kzalloc(test, sizeof(struct gdc_ctx), 0);

	ctx->s_frame.width = 320;
	ctx->s_frame.height = 240;
	ctx->s_frame.gdc_fmt = &gdc_formats[1];
	ctx->d_frame.width = 320;
	ctx->d_frame.height = 240;
	ctx->d_frame.gdc_fmt = &gdc_formats[1];
	ctx->cur_index = 0;
	ctx->crop_param[ctx->cur_index].is_grid_mode = 0;
	ctx->crop_param[ctx->cur_index].is_bypass_mode = 0;
	ctx->crop_param[ctx->cur_index].crop_start_x = 0;
	ctx->crop_param[ctx->cur_index].crop_start_y = 0;
	ctx->crop_param[ctx->cur_index].crop_width = 320;
	ctx->crop_param[ctx->cur_index].crop_height = 240;
	gdc->current_ctx = ctx;

	func->camerapp_hw_gdc_update_param(gdc->pmio, gdc);

	kunit_kfree(test, ctx);
}

static void camerapp_hw_gdc_init_pmio_config_kunit_test(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_dev *gdc = tctx->gdc;

	func->camerapp_hw_gdc_init_pmio_config(gdc);
}

static void camerapp_hw_gdc_g_reg_cnt_kunit_test(struct kunit *test)
{
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	int reg_cnt;

	reg_cnt = func->camerapp_hw_gdc_g_reg_cnt();
	KUNIT_EXPECT_EQ(test, reg_cnt, (u32)GDC_REG_CNT);
}

static void __gdc_init_pmio(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct camerapp_kunit_hw_gdc_func *func = camerapp_kunit_get_hw_gdc_test();
	struct gdc_dev *gdc = tctx->gdc;
	int ret;
	u32 reg_cnt;

	gdc->regs_base = tctx->addr;

	func->camerapp_hw_gdc_init_pmio_config(gdc);
	gdc->pmio_config.cache_type = PMIO_CACHE_NONE; /* override for kunit test */

	gdc->pmio = pmio_init(NULL, NULL, &gdc->pmio_config);

	ret = pmio_field_bulk_alloc(gdc->pmio, &gdc->pmio_fields,
					gdc->pmio_config.fields,
					gdc->pmio_config.num_fields);
	if (ret)
		return;

	reg_cnt = func->camerapp_hw_gdc_g_reg_cnt();

	gdc->pmio_reg_seqs = vzalloc(sizeof(struct pmio_reg_seq) * reg_cnt);
	if (!gdc->pmio_reg_seqs) {
		pmio_field_bulk_free(gdc->pmio, gdc->pmio_fields);
		pmio_exit(gdc->pmio);
		return;
	}
}

static void __gdc_exit_pmio(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;
	struct gdc_dev *gdc = tctx->gdc;

	if (gdc->pmio_reg_seqs)
		vfree(gdc->pmio_reg_seqs);

	if (gdc->pmio) {
		if (gdc->pmio_fields)
			pmio_field_bulk_free(gdc->pmio, gdc->pmio_fields);

		pmio_exit(gdc->pmio);
	}
}

static struct kunit_case camerapp_hw_api_gdc_kunit_test_cases[] = {
	KUNIT_CASE(camerapp_hw_api_gdc_get_size_constraints_kunit_test),
	KUNIT_CASE(camerapp_hw_api_gdc_sfr_dump_kunit_test),
	KUNIT_CASE(camerapp_hw_api_gdc_start_kunit_test),
	KUNIT_CASE(camerapp_hw_api_gdc_stop_kunit_test),
	KUNIT_CASE(camerapp_hw_api_gdc_sw_reset_kunit_test),
	KUNIT_CASE(camerapp_hw_gdc_get_intr_status_and_clear_kunit_test),
	KUNIT_CASE(camerapp_hw_gdc_has_comp_header_kunit_test),
	KUNIT_CASE(camerapp_hw_get_comp_buf_size_kunit_test),
	KUNIT_CASE(camerapp_hw_gdc_update_scale_parameters_kunit_test),
	KUNIT_CASE(camerapp_hw_gdc_update_grid_param_kunit_test),
	KUNIT_CASE(camerapp_hw_gdc_set_format_kunit_test),
	KUNIT_CASE(camerapp_hw_gdc_set_dma_address_kunit_test),
	KUNIT_CASE(camerapp_hw_gdc_update_param_kunit_test),
	KUNIT_CASE(camerapp_hw_gdc_init_pmio_config_kunit_test),
	KUNIT_CASE(camerapp_hw_gdc_g_reg_cnt_kunit_test),
	{},
};

static int camerapp_hw_api_gdc_kunit_test_init(struct kunit *test)
{
	test_ctx.addr = kunit_kzalloc(test, 0x10000, 0);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_ctx.addr);

	test_ctx.gdc = kunit_kzalloc(test, sizeof(struct gdc_dev), 0);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_ctx.gdc);

	test_ctx.crop_param = kunit_kzalloc(test, sizeof(struct gdc_crop_param), 0);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_ctx.crop_param);

	test_ctx.gdc->regs_base = test_ctx.addr;

	test->priv = &test_ctx;

	__gdc_init_pmio(test);

	return 0;
}

static void camerapp_hw_api_gdc_kunit_test_exit(struct kunit *test)
{
	struct cameraepp_hw_api_gdc_kunit_test_ctx *tctx = test->priv;

	__gdc_exit_pmio(test);

	kunit_kfree(test, tctx->crop_param);
	kunit_kfree(test, tctx->addr);
	kunit_kfree(test, tctx->gdc);
}

struct kunit_suite camerapp_hw_api_gdc_kunit_test_suite = {
	.name = "pablo-hw-api-gdc-v220-kunit-test",
	.init = camerapp_hw_api_gdc_kunit_test_init,
	.exit = camerapp_hw_api_gdc_kunit_test_exit,
	.test_cases = camerapp_hw_api_gdc_kunit_test_cases,
};
define_pablo_kunit_test_suites(&camerapp_hw_api_gdc_kunit_test_suite);

MODULE_LICENSE("GPL");
