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
#include "is-hw-api-csi.h"
#include "is-device-csi.h"
#include "is-device-sensor.h"
#include "csi/is-hw-csi-v6_0.h"
#include "pablo-smc.h"

/* Define the test cases. */
static void pablo_hw_csi_bns_dump_kunit_test(struct kunit *test)
{
	int ret;
	void *test_addr = NULL;
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_bns_dump);

	ret = func->csi_hw_bns_dump(test_addr);
	KUNIT_EXPECT_EQ(test, ret, -EFAULT);

	test_addr = kunit_kzalloc(test, 0x8000, 0);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	ret = func->csi_hw_bns_dump(test_addr);
	KUNIT_EXPECT_EQ(test, ret, 0);

	kunit_kfree(test, test_addr);
}

static void pablo_hw_csi_clear_fro_count_kunit_test(struct kunit *test)
{
	int ret;
	void *test_addr = NULL;
	void *test_addr2 = NULL;
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_clear_fro_count);

	ret = func->csi_hw_clear_fro_count(test_addr, test_addr2);
	KUNIT_EXPECT_EQ(test, ret, -EFAULT);

	test_addr = kunit_kzalloc(test, 0x8000, 0);
	test_addr2 = kunit_kzalloc(test, 0x8000, 0);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr2);
	ret = func->csi_hw_clear_fro_count(test_addr, test_addr2);
	KUNIT_EXPECT_EQ(test, ret, 0);

	kunit_kfree(test, test_addr);
	kunit_kfree(test, test_addr2);
}

static void pablo_hw_csi_fro_dump_kunit_test(struct kunit *test)
{
	int ret;
	void *test_addr = NULL;
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_fro_dump);

	ret = func->csi_hw_fro_dump(test_addr);
	KUNIT_EXPECT_EQ(test, ret, -EFAULT);

	test_addr = kunit_kzalloc(test, 0x8000, 0);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	ret = func->csi_hw_fro_dump(test_addr);
	KUNIT_EXPECT_EQ(test, ret, 0);

	kunit_kfree(test, test_addr);
}

static void pablo_hw_csi_g_bns_scale_factor_kunit_test(struct kunit *test)
{
	u32 factor, in, out;
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_g_bns_scale_factor);

	for (in = 2000; in >= 100; in-=100) {
		for (out = in; out >= 100; out-=100) {
			factor = func->csi_hw_g_bns_scale_factor(in, out);
			if (factor)
				KUNIT_EXPECT_EQ(test, factor, (u32)((in * 2) / out));
			else
				KUNIT_EXPECT_EQ(test, factor, (u32)0);
		}
	}
}

static void pablo_hw_csi_g_dam_common_frame_id_kunit_test(struct kunit *test)
{
	int ret;
	u32 batch_num = 1;
	u32 frame_id[2] = { 0 };
	void *test_addr = kunit_kzalloc(test, 0x8000, 0);
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_g_dma_common_frame_id);

	ret = func->csi_hw_g_dma_common_frame_id(test_addr, batch_num, frame_id);
	KUNIT_EXPECT_EQ(test, ret, -ENOEXEC);

	batch_num = 8;
	*(u32 *)(test_addr +
		csi_dma_cmn_regs[CSIS_DMA_CMN_R_CSIS_CMN_FRO_PREV_FRAME_ID0].sfr_offset) = 0x23333333;

	ret = func->csi_hw_g_dma_common_frame_id(test_addr, batch_num, frame_id);
	KUNIT_EXPECT_EQ(test, ret, (int)0);
	KUNIT_EXPECT_EQ(test, frame_id[0], (u32)0x33333332);
	KUNIT_EXPECT_EQ(test, frame_id[1], (u32)0x0);

	batch_num = 16;
	*(u32 *)(test_addr +
		csi_dma_cmn_regs[CSIS_DMA_CMN_R_CSIS_CMN_FRO_PREV_FRAME_ID0].sfr_offset) = 0x23333333;
	*(u32 *)(test_addr +
		csi_dma_cmn_regs[CSIS_DMA_CMN_R_CSIS_CMN_FRO_PREV_FRAME_ID1].sfr_offset) = 0x45555555;
	ret = func->csi_hw_g_dma_common_frame_id(test_addr, batch_num, frame_id);
	KUNIT_EXPECT_EQ(test, ret, (int)0);
	KUNIT_EXPECT_EQ(test, frame_id[0], (u32)0x55555554);
	KUNIT_EXPECT_EQ(test, frame_id[1], (u32)0x55555555);

	kunit_kfree(test, test_addr);
}

static void pablo_hw_csi_g_ebuf_irq_src_kunit_test(struct kunit *test)
{
	struct csis_irq_src irq_src = { 0 };
	int ebuf_ch = 0;
	u32 val;
	unsigned int num_of_ebuf = 4;
	unsigned int offset_fake_frame_done = 4;
	void *test_addr = kunit_kzalloc(test, 0x8000, 0);
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_g_ebuf_irq_src);

	func->csi_hw_g_ebuf_irq_src(test_addr, &irq_src, ebuf_ch, offset_fake_frame_done);
	KUNIT_EXPECT_EQ(test, irq_src.ebuf_status, (u32)0);

	*(u32 *)(test_addr + csi_ebuf_regs[CSIS_EBUF_R_EBUF_INTR_STATUS].sfr_offset) = 0xF;
	for (ebuf_ch = 0; ebuf_ch < num_of_ebuf; ebuf_ch++) {
		func->csi_hw_g_ebuf_irq_src(test_addr, &irq_src, ebuf_ch, offset_fake_frame_done);
		KUNIT_EXPECT_EQ(test, irq_src.ebuf_status, (u32)(1 << ebuf_ch));
		val = *(u32 *)(test_addr + csi_ebuf_regs[CSIS_EBUF_R_EBUF_INTR_CLEAR].sfr_offset);
		KUNIT_EXPECT_EQ(test, val, (u32)(1 << ebuf_ch));
	}
	kunit_kfree(test, test_addr);
}

static void pablo_hw_csi_g_irq_src_kunit_test(struct kunit *test)
{
	int ret;
	struct csis_irq_src irq_src = { 0 };
	bool clear = true;
	u32 fs_int_src = 0xAA;
	u32 fe_int_src = 0xBB;
	u32 line_end = 0xCC;
	void *test_addr = kunit_kzalloc(test, 0x8000, 0);
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_g_irq_src);

	*(u32 *)(test_addr + csi_regs[CSIS_R_FS_INT_SRC].sfr_offset) = fs_int_src;
	*(u32 *)(test_addr + csi_regs[CSIS_R_FE_INT_SRC].sfr_offset) = fe_int_src;
	*(u32 *)(test_addr + csi_regs[CSIS_R_LINE_END].sfr_offset) = line_end;
	*(u32 *)(test_addr + csi_regs[CSIS_R_ERR_LOST_FS].sfr_offset) = 0xF;
	*(u32 *)(test_addr + csi_regs[CSIS_R_ERR_LOST_FE].sfr_offset) = 0xF;
	*(u32 *)(test_addr + csi_regs[CSIS_R_ERR_VRESOL].sfr_offset) = 0xF;
	*(u32 *)(test_addr + csi_regs[CSIS_R_ERR_HRESOL].sfr_offset) = 0xF;
	*(u32 *)(test_addr + csi_regs[CSIS_R_CSIS_INT_SRC0].sfr_offset) = CSIS_IRQ_MASK0;
	*(u32 *)(test_addr + csi_regs[CSIS_R_CSIS_INT_SRC1].sfr_offset) = CSIS_IRQ_MASK1;

	ret = func->csi_hw_g_irq_src(test_addr, &irq_src, clear);
	KUNIT_EXPECT_EQ(test, ret, 0);
	KUNIT_EXPECT_EQ(test, irq_src.err_flag, (bool)true);
	KUNIT_EXPECT_EQ(test, irq_src.otf_start, fs_int_src);
	KUNIT_EXPECT_EQ(test, irq_src.otf_end, fe_int_src);
	KUNIT_EXPECT_EQ(test, irq_src.line_end, line_end);
	KUNIT_EXPECT_EQ(test, irq_src.err_id[0], (ulong)0xFFFF);

	*(u32 *)(test_addr + csi_regs[CSIS_R_CSIS_INT_SRC0].sfr_offset) = 0x0;
	*(u32 *)(test_addr + csi_regs[CSIS_R_CSIS_INT_SRC1].sfr_offset) = 0x0;
	ret = func->csi_hw_g_irq_src(test_addr, &irq_src, clear);
	KUNIT_EXPECT_EQ(test, ret, 0);
	KUNIT_EXPECT_EQ(test, irq_src.err_flag, (bool)false);

	kunit_kfree(test, test_addr);
}

static void pablo_hw_csi_g_frame_ptr_kunit_test(struct kunit *test)
{
	u32 frame_ptr;
	u32 vc = 0;
	u32 f_no;
	void *test_addr = kunit_kzalloc(test, 0x8000, 0);
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_g_frameptr);

	for(f_no = 0; f_no < 0x1F; f_no++) {
		*(u32 *)(test_addr + csi_dmax_chx_regs[CSIS_DMAX_CHX_R_ACT_CTRL].sfr_offset) = f_no << 2;
		frame_ptr = func->csi_hw_g_frameptr(test_addr, vc);
		KUNIT_EXPECT_EQ(test, frame_ptr, f_no);
	}

	kunit_kfree(test, test_addr);
}

static void pablo_hw_csi_g_mapped_phy_port_kunit_test(struct kunit *test)
{
	u32 port, csi_ch;
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_g_mapped_phy_port);

	for (csi_ch = 0; csi_ch < 4; csi_ch++) {
		port = func->csi_hw_g_mapped_phy_port(csi_ch);
		KUNIT_EXPECT_EQ(test, port, csi_ch);
	}
}

static void pablo_hw_csi_g_output_cur_dma_enable_kunit_test(struct kunit *test)
{
	bool enable;
	u32 vc = 0;
	void *test_addr = kunit_kzalloc(test, 0x8000, 0);
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_g_output_cur_dma_enable);

	*(u32 *)(test_addr + csi_dmax_chx_regs[CSIS_DMAX_CHX_R_ACT_CTRL].sfr_offset) = 0x0;
	enable = func->csi_hw_g_output_cur_dma_enable(test_addr, vc);
	KUNIT_EXPECT_EQ(test, enable, (bool)false);

	*(u32 *)(test_addr + csi_dmax_chx_regs[CSIS_DMAX_CHX_R_ACT_CTRL].sfr_offset) = 0x1;
	enable = func->csi_hw_g_output_cur_dma_enable(test_addr, vc);
	KUNIT_EXPECT_EQ(test, enable, (bool)true);

	kunit_kfree(test, test_addr);
}

static void pablo_hw_csi_reset_bns_kunit_test(struct kunit *test)
{
	u32 val;
	void *test_addr = kunit_kzalloc(test, 0x8000, 0);
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_reset_bns);

	func->csi_hw_reset_bns(test_addr);

	val = *(u32 *)(test_addr + csi_bns_regs[CSIS_BNS_R_BYR_BNS_BYPASS].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)0x1);

	kunit_kfree(test, test_addr);
}

typedef unsigned long (*psmc_call_t)(unsigned long cmd, unsigned long arg0,
		unsigned long arg1, unsigned long arg2);

#define BNS_INPUT_MUX_NUM	1
#define DMA_INPUT_MUX_NUM	5
#define IBUF_MUX_NUM		4
#define MAX_MUX_LIST_CNT (BNS_INPUT_MUX_NUM + DMA_INPUT_MUX_NUM + IBUF_MUX_NUM)

#define BNS_MUX_OFS	0
#define DMA_MUX_OFS(x) 	((x) + BNS_INPUT_MUX_NUM)
#define IBUF_MUX_OFS(x)	((x) + BNS_INPUT_MUX_NUM + DMA_INPUT_MUX_NUM)

static const u32 mux_list[MAX_MUX_LIST_CNT] = {
		0x1725020C, /* BNS_MUX */
		0x17250240, /* DMA_MUX0 */
		0x17250244, /* DMA_MUX1 */
		0x17250248, /* DMA_MUX2 */
		0x1725024C, /* DMA_MUX3 */
		0x17250250, /* DMA_MUX4 */
		0x17250254, /* IBUF_MUX0 */
		0x17250258, /* IBUF_MUX1 */
		0x1725025c, /* IBUF_MUX2 */
		0x17250260, /* IBUF_MUX3 */
};
static u32 mux_val[MAX_MUX_LIST_CNT];

static unsigned long pablo_smc_call_mock(unsigned long cmd, unsigned long arg0,
		unsigned long arg1, unsigned long arg2)
{
	u32 i;

	/* Only SMC_CMD_REG (==-101) cmd is acceptable. */
	if (cmd != (unsigned long)-101)
		return (unsigned long)-1;

	for (i = 0; i < MAX_MUX_LIST_CNT; i++) {
		if (arg0 == SMC_REG_ID_SFR_W(mux_list[i])) {
				mux_val[i] = arg1;
				break;
		}
	}

	return 0;
}

static void pablo_hw_csi_s_bns_ch_kunit_test(struct kunit *test)
{
	u32 ch, otf_ch_max = 4;
	void *test_addr = kunit_kzalloc(test, 0x8000, 0);
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();
	struct pablo_smc_ops *psmc_ops = pablo_get_smc_ops();
	psmc_call_t psmc_backup;

	/* Change pablo_smc ops into mock ops. */
	psmc_backup = psmc_ops->call;

	psmc_ops->call = pablo_smc_call_mock;

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_s_bns_ch);

	memset(mux_val, 0x00, sizeof(mux_val));

	for (ch = 0; ch < otf_ch_max; ch++) {
		func->csi_hw_s_bns_ch(test_addr, ch);
		KUNIT_EXPECT_EQ(test, ch, mux_val[BNS_MUX_OFS]);
	}

	/* Restore pablo_smc ops. */
	psmc_ops->call = psmc_backup;

	kunit_kfree(test, test_addr);
}

static void pablo_hw_csi_s_cfg_ebuf_kunit_test(struct kunit *test)
{
	u32 val;
	u32 ebuf_ch, vc;
	u32 width = 1920;
	u32 height = 1080;
	u32 offset;
	int ret;
	void *test_addr = kunit_kzalloc(test, 0x8000, 0);
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_s_cfg_ebuf);

	for (ebuf_ch = 0; ebuf_ch < 4; ebuf_ch++) {
		for (vc = 0; vc < 2; vc++) {
			ret = func->csi_hw_s_cfg_ebuf(test_addr, ebuf_ch, vc, width, height);
			KUNIT_EXPECT_EQ(test, ret, 0);

			offset = CSIS_EBUF_R_EBUF0_CHID0_SIZE + (6 * ebuf_ch) + vc;
			val = *(u32 *)(test_addr + csi_ebuf_regs[offset].sfr_offset);

			KUNIT_EXPECT_EQ(test, width / 8, (u32)(val & 0xFFFF)); /* 8PPC */
			KUNIT_EXPECT_EQ(test, height, (u32)(val >> 16));
		}
	}

	ebuf_ch = 4;
	vc = 0;
	ret = func->csi_hw_s_cfg_ebuf(test_addr, ebuf_ch, vc, width, height);
	KUNIT_EXPECT_EQ(test, ret, (int)-EINVAL);

	ebuf_ch = 0;
	vc = 3;
	ret = func->csi_hw_s_cfg_ebuf(test_addr, ebuf_ch, vc, width, height);
	KUNIT_EXPECT_EQ(test, ret, (int)-EINVAL);

	kunit_kfree(test, test_addr);
}

static void pablo_hw_csi_s_dma_common_dynamic_kunit_test(struct kunit *test)
{
	int ret;
	size_t size = 0;
	unsigned int dma_ch = 0;
	void *test_addr = kunit_kzalloc(test, 0x8000, 0);
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_s_dma_common_dynamic);

	ret = func->csi_hw_s_dma_common_dynamic(test_addr, size, dma_ch);
	KUNIT_EXPECT_EQ(test, ret, 0);

	kunit_kfree(test, test_addr);
}

static void pablo_hw_csi_s_dma_common_pattern_kunit_test(struct kunit *test)
{
	int ret;
	int clk_mhz;
	int vvalid;
	int vblank;
	int vblank_size;
	u32 width = 1920;
	u32 height = 1080;
	u32 fps = 30;
	u32 clk = 24000000;
	u32 hblank = 0xFF;
	u32 v_to_hblank = 0x14;
	u32 h_to_vblank = 0x28;
	u32 val;
	void *test_addr = kunit_kzalloc(test, 0x8000, 0);
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();

	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_addr);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, func);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_s_dma_common_pattern_enable);
	KUNIT_EXPECT_NOT_ERR_OR_NULL(test, func->csi_hw_s_dma_common_pattern_disable);

	ret = func->csi_hw_s_dma_common_pattern_enable(test_addr, width, height, fps, clk);
	KUNIT_EXPECT_EQ(test, ret, 0);

	val = *(u32 *)(test_addr +
		csi_dma_cmn_regs[CSIS_DMA_CMN_R_CSIS_CMN_TEST_PATTERN_CTRL].sfr_offset);
	KUNIT_EXPECT_EQ(test, hblank, (u32)((val >> 8) & 0xFF));
	KUNIT_EXPECT_EQ(test, v_to_hblank, (u32)(val >> 16) & 0xFF);
	KUNIT_EXPECT_EQ(test, h_to_vblank, (u32)(val & 0xFF));

	val = *(u32 *)(test_addr +
		csi_dma_cmn_regs[CSIS_DMA_CMN_R_CSIS_CMN_TEST_PATTERN_SIZE].sfr_offset);
	KUNIT_EXPECT_EQ(test, width / 2, (u32)(val & 0x3FFF));
	KUNIT_EXPECT_EQ(test, height, (u32)((val >> 16) & 0x3FFF));

	clk_mhz = clk / 1000000;
	vvalid = (width * height) / (clk_mhz * 2);
	vblank = ((1000000 / fps) - vvalid);
	if (vblank < 0)
		vblank = 1000; /* 1000 us */
	vblank_size = vblank * clk_mhz;

	val = *(u32 *)(test_addr +
		csi_dma_cmn_regs[CSIS_DMA_CMN_R_CSIS_CMN_TEST_PATTERN_ON].sfr_offset);
	KUNIT_EXPECT_EQ(test, (u32)(val >> 31), (u32)1);
	KUNIT_EXPECT_EQ(test, (u32)(val >> 30) & 0x1, (u32)1);
	KUNIT_EXPECT_EQ(test, (u32)(val & 0x3FFFFFFF), (u32)vblank_size);

	/* error return check */
	width = 1;
	ret = func->csi_hw_s_dma_common_pattern_enable(test_addr, width, height, fps, clk);
	KUNIT_EXPECT_EQ(test, ret, (int)-EINVAL);

	func->csi_hw_s_dma_common_pattern_disable(test_addr);
	val = *(u32 *)(test_addr +
		csi_dma_cmn_regs[CSIS_DMA_CMN_R_CSIS_CMN_TEST_PATTERN_ON].sfr_offset);
	KUNIT_EXPECT_EQ(test, (u32)(val >> 31), (u32)0);

	kunit_kfree(test, test_addr);
}

static void pablo_hw_csi_hw_s_otf_out_mux(struct kunit *test)
{
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();
	struct pablo_smc_ops *psmc_ops = pablo_get_smc_ops();
	void *reg_mux = NULL; /* Not actually used */
	psmc_call_t psmc_backup;
	u32 otf_ch;
	u32 csi_ch = 1;
	u32 img_vc = 0;

	/* Change pablo_smc ops into mock ops. */
	psmc_backup = psmc_ops->call;
	psmc_ops->call = pablo_smc_call_mock;

	memset(mux_val, 0x00, sizeof(mux_val));

	/* Case 1. Set MUX */
	for (otf_ch = 0; otf_ch < MAX_NUM_CSIS_OTF_CH; otf_ch++) {
		func->csi_hw_s_otf_out_mux(reg_mux, csi_ch, otf_ch, img_vc, true);
		KUNIT_EXPECT_EQ(test, csi_ch, mux_val[IBUF_MUX_OFS(otf_ch)]);
	}

	/* Case 2. Reset MUX */
	for (otf_ch = 0; otf_ch < MAX_NUM_CSIS_OTF_CH; otf_ch++) {
		func->csi_hw_s_otf_out_mux(reg_mux, csi_ch, otf_ch, img_vc, false);
		KUNIT_EXPECT_EQ(test, 0x3f, mux_val[IBUF_MUX_OFS(otf_ch)]);
	}

	/* Restore pablo_smc ops. */
	psmc_ops->call = psmc_backup;
}

static void pablo_hw_csi_hw_s_dma_input_mux(struct kunit *test)
{
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();
	struct pablo_smc_ops *psmc_ops = pablo_get_smc_ops();
	void *reg_mux = NULL; /* Not actually used */
	bool bns_en;
	u32 otf_ch = 2;
	u32 bns_dma_mux_val = 0x24;
	u32 csi_ch = 0;
	u32 wdma_ch;
	psmc_call_t psmc_backup;

	/* Change pablo_smc ops into mock ops. */
	psmc_backup = psmc_ops->call;
	psmc_ops->call = pablo_smc_call_mock;

	memset(mux_val, 0x00, sizeof(mux_val));

	/* Case 1. CSIS OTF out -> WDMA */
	bns_en = false;
	for (wdma_ch = 0; wdma_ch < DMA_INPUT_MUX_NUM; wdma_ch++) {
		func->csi_hw_s_dma_input_mux(reg_mux, wdma_ch, bns_en,
				bns_dma_mux_val, csi_ch, otf_ch);
		KUNIT_EXPECT_EQ(test, otf_ch, mux_val[DMA_MUX_OFS(wdma_ch)]);
	}

	/* Case 2. CSIS OTF out -> BNS -> WDMA */
	bns_en = true;
	for (wdma_ch = 0; wdma_ch < DMA_INPUT_MUX_NUM; wdma_ch++) {
		func->csi_hw_s_dma_input_mux(reg_mux, wdma_ch, bns_en,
			bns_dma_mux_val, csi_ch, otf_ch);
		KUNIT_EXPECT_EQ(test, bns_dma_mux_val, mux_val[DMA_MUX_OFS(wdma_ch)]);
	}

	/* Restore pablo_smc ops. */
	psmc_ops->call = psmc_backup;
}

static void pablo_hw_csi_hw_s_init_input_mux(struct kunit *test)
{
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();
	struct pablo_smc_ops *psmc_ops = pablo_get_smc_ops();
	void *reg_mux = NULL; /* Not actually used */
	u32 wdma_ch;
	psmc_call_t psmc_backup;

	/* Change pablo_smc ops into mock ops. */
	psmc_backup = psmc_ops->call;
	psmc_ops->call = pablo_smc_call_mock;

	memset(mux_val, 0x00, sizeof(mux_val));

	for (wdma_ch = 0; wdma_ch < DMA_INPUT_MUX_NUM; wdma_ch++) {
		func->csi_hw_s_init_input_mux(reg_mux, wdma_ch);
		KUNIT_EXPECT_EQ(test, 0xFFFFFFFF, mux_val[DMA_MUX_OFS(wdma_ch)]);
	}

	/* Restore pablo_smc ops. */
	psmc_ops->call = psmc_backup;
}

static void pablo_hw_csi_hw_s_sbwc_ctrl(struct kunit *test)
{
	struct pablo_kunit_hw_csi_func *func = pablo_kunit_get_hw_csi_test();
	void *reg_mux = kunit_kzalloc(test, 0x8000, 0);
	u32 sbwc_en, sbwc_type, comp_64b_align, val;

	sbwc_en = 1;
	sbwc_type = DMA_INPUT_SBWC_LOSSYLESS_32B;
	comp_64b_align = 0;

	func->csi_hw_s_sbwc_ctrl(reg_mux, sbwc_en, sbwc_type, comp_64b_align);

	val = *(u32 *)(reg_mux + csi_dmax_regs[CSIS_DMAX_R_SBWC_CTRL].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)0x1);


	sbwc_en = 1;
	sbwc_type = DMA_INPUT_SBWC_LOSSYLESS_64B;
	comp_64b_align = 1;

	func->csi_hw_s_sbwc_ctrl(reg_mux, sbwc_en, sbwc_type, comp_64b_align);

	val = *(u32 *)(reg_mux + csi_dmax_regs[CSIS_DMAX_R_SBWC_CTRL].sfr_offset);
	KUNIT_EXPECT_EQ(test, val, (u32)0xd);

	kunit_kfree(test, reg_mux);
}


static struct kunit_case pablo_hw_csi_kunit_test_cases[] = {
	KUNIT_CASE(pablo_hw_csi_bns_dump_kunit_test),
	KUNIT_CASE(pablo_hw_csi_clear_fro_count_kunit_test),
	KUNIT_CASE(pablo_hw_csi_fro_dump_kunit_test),
	KUNIT_CASE(pablo_hw_csi_g_bns_scale_factor_kunit_test),
	KUNIT_CASE(pablo_hw_csi_g_dam_common_frame_id_kunit_test),
	KUNIT_CASE(pablo_hw_csi_g_ebuf_irq_src_kunit_test),
	KUNIT_CASE(pablo_hw_csi_g_irq_src_kunit_test),
	KUNIT_CASE(pablo_hw_csi_g_frame_ptr_kunit_test),
	KUNIT_CASE(pablo_hw_csi_g_mapped_phy_port_kunit_test),
	KUNIT_CASE(pablo_hw_csi_g_output_cur_dma_enable_kunit_test),
	KUNIT_CASE(pablo_hw_csi_reset_bns_kunit_test),
	KUNIT_CASE(pablo_hw_csi_s_bns_ch_kunit_test),
	KUNIT_CASE(pablo_hw_csi_s_cfg_ebuf_kunit_test),
	KUNIT_CASE(pablo_hw_csi_s_dma_common_dynamic_kunit_test),
	KUNIT_CASE(pablo_hw_csi_s_dma_common_pattern_kunit_test),
	KUNIT_CASE(pablo_hw_csi_hw_s_otf_out_mux),
	KUNIT_CASE(pablo_hw_csi_hw_s_dma_input_mux),
	KUNIT_CASE(pablo_hw_csi_hw_s_init_input_mux),
	KUNIT_CASE(pablo_hw_csi_hw_s_sbwc_ctrl),
	{},
};

struct kunit_suite pablo_hw_csi_kunit_test_suite = {
	.name = "pablo-hw-csi-v8_1-kunit-test",
	.test_cases = pablo_hw_csi_kunit_test_cases,
};
define_pablo_kunit_test_suites(&pablo_hw_csi_kunit_test_suite);

MODULE_LICENSE("GPL");
