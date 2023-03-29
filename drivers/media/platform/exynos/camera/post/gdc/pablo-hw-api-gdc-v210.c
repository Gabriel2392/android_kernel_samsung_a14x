/*
 * Samsung EXYNOS CAMERA PostProcessing GDC driver
 *
 * Copyright (C) 2018 Samsung Electronics Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "pablo-hw-api-gdc.h"
#include "pablo-hw-reg-gdc-v210.h"
#include "is-common-enum.h"
#include "is-type.h"
#include "pmio.h"

#define GDC_SET_F(base, R, F, val) \
       is_hw_set_field(((struct pablo_mmio *)base)->mmio_base, R, F, val)
#define GDC_SET_R(base, R, val) \
       is_hw_set_reg(((struct pablo_mmio *)base)->mmio_base, R, val)
#define GDC_GET_F(base, R, F) \
       is_hw_get_field(((struct pablo_mmio *)base)->mmio_base, R, F)
#define GDC_GET_R(base, R) \
       is_hw_get_reg(((struct pablo_mmio *)base)->mmio_base, R)

static const struct gdc_variant gdc_variant[] = {
	{
		.limit_input = {
			.min_w          = 96,
			.min_h          = 64,
			.max_w          = 8192,
			.max_h          = 6144,
		},
		.limit_output = {
			.min_w          = 96,
			.min_h          = 64,
			.max_w          = 8192,
			.max_h          = 6144,
		},
		.version                = 0,
	},
};

const struct gdc_variant *camerapp_hw_gdc_get_size_constraints(struct pablo_mmio *pmio)
{
	return gdc_variant;
}

u32 camerapp_hw_gdc_get_ver(struct pablo_mmio *pmio)
{
	return GDC_GET_R(pmio, &gdc_regs[GDC_R_GDC_VER]);
}

void camerapp_hw_gdc_start(struct pablo_mmio *pmio, struct c_loader_buffer *clb)
{
	/* oneshot */
	GDC_SET_F(pmio, &gdc_regs[GDC_R_FRAMESTART_CMD], &gdc_fields[GDC_F_FRAMESTART_CMD], 1);
}

void camerapp_hw_gdc_stop(struct pablo_mmio *pmio)
{
	/* GDC QCH off */
	GDC_SET_R(pmio, &gdc_regs[GDC_R_GDC_PROCESSING], 0);
}

u32 camerapp_hw_gdc_sw_reset(struct pablo_mmio *pmio)
{
	u32 reset_count = 0;

	/* request to stop bus IF */
	GDC_SET_F(pmio, &gdc_regs[GDC_R_REQ_STOP], &gdc_fields[GDC_F_EN_RESET], 1);

	/* wait bus IF reset complete */
	do {
		reset_count++;
		if (reset_count > 10000)
			return reset_count;
	} while (GDC_GET_F(pmio, &gdc_regs[GDC_R_REQ_STOP], &gdc_fields[GDC_F_RESET_OK]) != 1);

	/* request to gdc hw */
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_SW_RESET], &gdc_fields[GDC_F_SW_RESET], 1);

	/* wait reset complete */
	do {
		reset_count++;
		if (reset_count > 10000)
			return reset_count;
	} while (GDC_GET_F(pmio, &gdc_regs[GDC_R_GDC_SW_RESET], &gdc_fields[GDC_F_SW_RESET]) != 0);

	return 0;
}

void camerapp_hw_gdc_clear_intr_all(struct pablo_mmio *pmio)
{
	GDC_SET_R(pmio, &gdc_regs[GDC_R_GDC_INT_STATUS], 0x0001);
}

void camerapp_hw_gdc_mask_intr(struct pablo_mmio *pmio, u32 intr_mask)
{
	GDC_SET_R(pmio, &gdc_regs[GDC_R_GDC_INT_MASK], intr_mask);
}

u32 camerapp_hw_gdc_get_intr_status(struct pablo_mmio *pmio)
{
	return GDC_GET_R(pmio, &gdc_regs[GDC_R_GDC_INT_STATUS]);
}

u32 camerapp_hw_gdc_get_intr_status_and_clear(struct pablo_mmio *pmio)
{
	u32 intStatus;

	intStatus = GDC_GET_R(pmio, &gdc_regs[GDC_R_GDC_INT_STATUS]);
	GDC_SET_R(pmio, &gdc_regs[GDC_R_GDC_INT_STATUS], intStatus);

	return intStatus;
}

u32 camerapp_hw_gdc_get_int_frame_start(void)
{
	return GDC_INT_FRAME_START;
}

u32 camerapp_hw_gdc_get_int_frame_end(void)
{
	return GDC_INT_FRAME_END;
}

void camerapp_hw_gdc_update_grid_param(struct pablo_mmio *pmio, struct gdc_crop_param *crop_param)
{
	struct is_reg reg;
	u32 i = 0, j = 0;
	u32 sfr_offset = 0x0004;
	u32 sfr_start_x = 0x1000;
	u32 sfr_start_y = 0x3000;

	if (crop_param->use_calculated_grid) {
		for (i = 0; i < GRID_Y_SIZE; i++) {
			for (j = 0; j < GRID_X_SIZE; j++) {
				u32 cal_sfr_offset = (sfr_offset * i * GRID_X_SIZE) + (sfr_offset * j);

				reg.sfr_offset = sfr_start_x + cal_sfr_offset;
				GDC_SET_R(pmio, &reg, crop_param->calculated_grid_x[i][j]);
				reg.sfr_offset = sfr_start_y + cal_sfr_offset;
				GDC_SET_R(pmio, &reg, crop_param->calculated_grid_y[i][j]);
			}
		}
	} else {
		for (i = 0; i < GRID_Y_SIZE; i++) {
			for (j = 0; j < GRID_X_SIZE; j++) {
				u32 cal_sfr_offset = (sfr_offset * i * GRID_X_SIZE) + (sfr_offset * j);

				reg.sfr_offset = sfr_start_x + cal_sfr_offset;
				GDC_SET_R(pmio, &reg, 0);
				reg.sfr_offset = sfr_start_y + cal_sfr_offset;
				GDC_SET_R(pmio, &reg, 0);
			}
		}
	}
}

void camerapp_hw_gdc_update_scale_parameters(struct pablo_mmio *pmio, struct gdc_frame *s_frame,
						struct gdc_frame *d_frame, struct gdc_crop_param *crop_param)
{
	u32 gdc_input_width;
	u32 gdc_input_height;
	u32 gdc_crop_offset_x;
	u32 gdc_crop_offset_y;
	u32 gdc_crop_width;
	u32 gdc_crop_height;
	u32 gdc_scale_shifter_x;
	u32 gdc_scale_shifter_y;
	u32 gdc_scale_x;
	u32 gdc_scale_y;
	u32 gdc_inv_scale_x;
	u32 gdc_inv_scale_y;
	u32 gdc_output_offset_x;
	u32 gdc_output_offset_y;
	u32 gdc_output_width;
	u32 gdc_output_height;

	u32 scaleShifterX;
	u32 scaleShifterY;
	u32 imagewidth;
	u32 imageheight;
	u32 out_scaled_width;
	u32 out_scaled_height;

	/* GDC original input size */
	gdc_input_width = s_frame->width;
	gdc_input_height = s_frame->height;

	/* GDC input crop size from original input size */
	gdc_crop_offset_x = crop_param->crop_start_x;
	gdc_crop_offset_y = crop_param->crop_start_y;
	gdc_crop_width = crop_param->crop_width;
	gdc_crop_height = crop_param->crop_height;

	/* Real to virtual grid scaling factor shifters */
	scaleShifterX  = DS_SHIFTER_MAX;
	imagewidth = gdc_input_width << 1;
	while ((imagewidth <= MAX_VIRTUAL_GRID_X) && (scaleShifterX > 0)) {
		imagewidth <<= 1;
		scaleShifterX--;
	}
	gdc_scale_shifter_x = scaleShifterX;

	scaleShifterY  = DS_SHIFTER_MAX;
	imageheight = gdc_input_height << 1;
	while ((imageheight <= MAX_VIRTUAL_GRID_Y) && (scaleShifterY > 0)) {
		imageheight <<= 1;
		scaleShifterY--;
	}
	gdc_scale_shifter_y = scaleShifterY;

	/* Real to virtual grid scaling factors */
	gdc_scale_x = MIN(65535,
		((MAX_VIRTUAL_GRID_X << (DS_FRACTION_BITS + scaleShifterX)) + gdc_input_width / 2) / gdc_input_width);
	gdc_scale_y = MIN(65535,
		((MAX_VIRTUAL_GRID_Y << (DS_FRACTION_BITS + scaleShifterY)) + gdc_input_height / 2) / gdc_input_height);

	/* Virtual to real grid scaling factors */
	gdc_inv_scale_x = gdc_input_width;
	gdc_inv_scale_y = ((gdc_input_height << 3) + 3) / 6;

	/* Meaningful only when out_crop_bypass = 0, x <= (gdc_crop_width - gdc_image_active_width) */
	gdc_output_width = d_frame->width;
	gdc_output_height = d_frame->height;
	if ((gdc_output_width & 0x1) || (gdc_output_height & 0x1)) {
		gdc_dbg("gdc output size is not even.\n");
		gdc_output_width = ALIGN(gdc_output_width, 2);
		gdc_output_height = ALIGN(gdc_output_height, 2);
	}

	if (gdc_crop_width < gdc_output_width)
		gdc_output_offset_x = 0;
	else
		gdc_output_offset_x = ALIGN_DOWN((gdc_crop_width - gdc_output_width) >> 1, 2);

	if (gdc_crop_height < gdc_output_height)
		gdc_output_offset_y = 0;
	else
		gdc_output_offset_y = ALIGN_DOWN((gdc_crop_height - gdc_output_height) >> 1, 2);

	/* if GDC is scaled up : 128(default) = no scaling, 64 = 2 times scaling */
	/* now is selected no scaling. => calcuration (128 * in / out) */
	out_scaled_width = 128;
	out_scaled_height = 128;

	/* Only for scale up */
	if (gdc_crop_width * MAX_OUTPUT_SCALER_RATIO < gdc_output_width) {
		gdc_dbg("gdc_out_scale_x exceeds max ratio(%d). in_crop(%dx%d) out(%dx%d)\n",
				MAX_OUTPUT_SCALER_RATIO,
				gdc_crop_width, gdc_crop_height,
				gdc_output_width, gdc_output_height);
		out_scaled_width = 128 / MAX_OUTPUT_SCALER_RATIO;
	} else if (gdc_crop_width < gdc_output_width) {
		out_scaled_width = 128 * gdc_crop_width / gdc_output_width;
	}

	/* Only for scale up */
	if (gdc_crop_height * MAX_OUTPUT_SCALER_RATIO < gdc_output_height) {
		gdc_dbg("gdc_out_scale_y exceeds max ratio(%d). in_crop(%dx%d) out(%dx%d)\n",
				MAX_OUTPUT_SCALER_RATIO,
				gdc_crop_width, gdc_crop_height,
				gdc_output_width, gdc_output_height);
		out_scaled_height = 128 / MAX_OUTPUT_SCALER_RATIO;
	} else if (gdc_crop_height < gdc_output_height) {
		out_scaled_height = 128 * gdc_crop_height / gdc_output_height;
	}

	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_CONFIG],
		&gdc_fields[GDC_F_GDC_MIRROR_X], 0);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_CONFIG],
		&gdc_fields[GDC_F_GDC_MIRROR_Y], 0);

	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_INPUT_ORG_SIZE],
		&gdc_fields[GDC_F_GDC_ORG_HEIGHT], gdc_input_height);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_INPUT_ORG_SIZE],
		&gdc_fields[GDC_F_GDC_ORG_WIDTH], gdc_input_width);

	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_INPUT_CROP_START],
		&gdc_fields[GDC_F_GDC_CROP_START_X], gdc_crop_offset_x);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_INPUT_CROP_START],
		&gdc_fields[GDC_F_GDC_CROP_START_Y], gdc_crop_offset_y);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_INPUT_CROP_SIZE],
		&gdc_fields[GDC_F_GDC_CROP_WIDTH], gdc_crop_width);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_INPUT_CROP_SIZE],
		&gdc_fields[GDC_F_GDC_CROP_HEIGHT], gdc_crop_height);

	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_SCALE],
		&gdc_fields[GDC_F_GDC_SCALE_X], gdc_scale_x);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_SCALE],
		&gdc_fields[GDC_F_GDC_SCALE_Y], gdc_scale_y);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_SCALE_SHIFTER],
		&gdc_fields[GDC_F_GDC_SCALE_SHIFTER_X], gdc_scale_shifter_x);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_SCALE_SHIFTER],
		&gdc_fields[GDC_F_GDC_SCALE_SHIFTER_Y], gdc_scale_shifter_y);

	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_INV_SCALE],
		&gdc_fields[GDC_F_GDC_INV_SCALE_X], gdc_inv_scale_x);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_INV_SCALE],
		&gdc_fields[GDC_F_GDC_INV_SCALE_Y], gdc_inv_scale_y);

	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_OUT_CROP_START],
		&gdc_fields[GDC_F_GDC_IMAGE_CROP_PRE_X], gdc_output_offset_x);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_OUT_CROP_START],
		&gdc_fields[GDC_F_GDC_IMAGE_CROP_PRE_Y], gdc_output_offset_y);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_OUT_CROP_SIZE],
		&gdc_fields[GDC_F_GDC_IMAGE_ACTIVE_WIDTH], gdc_output_width);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_OUT_CROP_SIZE],
		&gdc_fields[GDC_F_GDC_IMAGE_ACTIVE_HEIGHT], gdc_output_height);

	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_OUT_SCALE],
		&gdc_fields[GDC_F_GDC_OUT_SCALE_Y], out_scaled_height);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_OUT_SCALE],
		&gdc_fields[GDC_F_GDC_OUT_SCALE_X], out_scaled_width);

	gdc_dbg("input %dx%d in_crop %d,%d %dx%d -> out_crop %d,%d %dx%d / scale %dx%d\n",
			gdc_input_width, gdc_input_height,
			gdc_crop_offset_x, gdc_crop_offset_y, gdc_crop_width, gdc_crop_height,
			gdc_output_offset_x, gdc_output_offset_y, gdc_output_width, gdc_output_height,
			out_scaled_height, out_scaled_width);
}

void camerapp_hw_gdc_update_dma_size(struct pablo_mmio *pmio,
	struct gdc_frame *s_frame, struct gdc_frame *d_frame)
{
	u32 input_stride_lum_w, input_stride_chroma_w;
	u32 input_stride_lum_w_2bit, input_stride_chroma_w_2bit;
	u32 output_stride_lum_w, output_stride_chroma_w;
	u32 output_stride_lum_w_2bit, output_stride_chroma_w_2bit;
	u32 in_dma_width, out_dma_width;
/*
 * supported format
 * - YUV422_2P_8bit/10bit
 * - YUV420_2P_8bit/10bit
 * - input 10bit : P010(P210), packed 10bit, 8+2bit
 * - output 10bit : P010(P210), 8+2bit
 */
	out_dma_width = GDC_GET_F(pmio, &gdc_regs[GDC_R_GDC_OUT_CROP_SIZE],
		&gdc_fields[GDC_F_GDC_IMAGE_ACTIVE_WIDTH]);
	in_dma_width = GDC_GET_F(pmio, &gdc_regs[GDC_R_GDC_INPUT_ORG_SIZE],
		&gdc_fields[GDC_F_GDC_ORG_WIDTH]);

	input_stride_lum_w_2bit = 0;
	input_stride_chroma_w_2bit = 0;
	output_stride_lum_w_2bit = 0;
	output_stride_chroma_w_2bit = 0;

	if ((s_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV12M_P010)
			|| (s_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV21M_P010)
			|| (s_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV16M_P210)
			|| (s_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV61M_P210)) {
		input_stride_lum_w = (u32)(((in_dma_width * 2 + 15) / 16) * 16);
		input_stride_chroma_w = (u32)(((in_dma_width * 2 + 15) / 16) * 16);
	} else {
		input_stride_lum_w = (u32)(((in_dma_width + 15) / 16) * 16);
		input_stride_chroma_w = (u32)(((in_dma_width + 15) / 16) * 16);
		if ((s_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV16M_S10B)
				|| (s_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV61M_S10B)
				|| (s_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV12M_S10B)
				|| (s_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV21M_S10B)) {
			input_stride_lum_w_2bit = (u32)((((in_dma_width * 2 + 7) / 8 + 15) / 16) * 16);
			input_stride_chroma_w_2bit = (u32)((((in_dma_width * 2 + 7) / 8 + 15) / 16) * 16);
		}
	}

	if ((d_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV12M_P010)
			|| (d_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV21M_P010)
			|| (d_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV16M_P210)
			|| (d_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV61M_P210)) {
		output_stride_lum_w = (u32)(((out_dma_width * 2 + 15) / 16) * 16);
		output_stride_chroma_w = (u32)(((out_dma_width * 2 + 15) / 16) * 16);
	} else {
		output_stride_lum_w = (u32)(((out_dma_width + 15) / 16) * 16);
		output_stride_chroma_w = (u32)(((out_dma_width + 15) / 16) * 16);
		if ((d_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV16M_S10B)
				|| (d_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV61M_S10B)
				|| (d_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV12M_S10B)
				|| (d_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_NV21M_S10B)) {
			output_stride_lum_w_2bit = (u32)((((out_dma_width * 2 + 7) / 8 + 15) / 16) * 16);
			output_stride_chroma_w_2bit = (u32)((((out_dma_width * 2 + 7) / 8 + 15) / 16) * 16);
		}
	}

/*
	src_10bit_format == packed10bit (10bit + 10bit + 10bit... no padding)
	input_stride_lum_w = (u32)(((in_dma_width * 10 + 7) / 8 + 15) / 16 * 16);
	input_stride_chroma_w = (u32)(((in_dma_width * 10 + 7) / 8 + 15) / 16 * 16);
*/

	gdc_dbg("s_w = %d, lum stride_w = %d, stride_2bit_w = %d\n",
		s_frame->width, input_stride_lum_w, input_stride_lum_w_2bit);
	gdc_dbg("s_w = %d, chroma stride_w = %d, stride_2bit_w = %d\n",
		s_frame->width, input_stride_chroma_w, input_stride_chroma_w_2bit);
	gdc_dbg("d_w = %d, lum stride_w = %d, stride_2bit_w = %d\n",
		d_frame->width, output_stride_lum_w, output_stride_lum_w_2bit);
	gdc_dbg("d_w = %d, chroma stride_w = %d, stride_2bit_w = %d\n",
		d_frame->width, output_stride_chroma_w, output_stride_chroma_w_2bit);

	/* about write DMA stride size : It should be aligned with 16 bytes */
	GDC_SET_F(pmio, &gdc_regs[GDC_R_WAXI_STRIDE_LUM],
		&gdc_fields[GDC_F_WRITE_STRIDE_LUMA], output_stride_lum_w);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_WAXI_STRIDE_CHROMA],
		&gdc_fields[GDC_F_WRITE_STRIDE_CHROMA], output_stride_chroma_w);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_WAXI_STRIDE_2BIT_LUM],
		&gdc_fields[GDC_F_WRITE_STRIDE_2BIT_LUMA], output_stride_lum_w_2bit);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_WAXI_STRIDE_2BIT_CHROMA],
		&gdc_fields[GDC_F_WRITE_STRIDE_2BIT_CHROMA], output_stride_chroma_w_2bit);

	/* about read DMA stride size */
	GDC_SET_F(pmio, &gdc_regs[GDC_R_PXC_STRIDE_LUM],
		&gdc_fields[GDC_F_PXCSTRIDELUM], input_stride_lum_w);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_PXC_STRIDE_CHROMA],
		&gdc_fields[GDC_F_PXCSTRIDECHROMA], input_stride_chroma_w);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_PXC_STRIDE_2BIT_LUM],
		&gdc_fields[GDC_F_PXCSTRIDE2BITLUM], input_stride_lum_w_2bit);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_PXC_STRIDE_2BIT_CHROMA],
		&gdc_fields[GDC_F_PXCSTRIDE2BITCHROMA], input_stride_chroma_w_2bit);

}

void camerapp_hw_gdc_set_dma_address(struct pablo_mmio *pmio,
	struct gdc_frame *s_frame, struct gdc_frame *d_frame)
{
	/*Dst Base address of the first Y, UV plane in the DRAM.  must be [1:0] == 0 : Must be multiple of 4*/
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_DST_BASE_LUM],
		&gdc_fields[GDC_F_GDC_DST_BASE_LUM], d_frame->addr.y);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_DST_BASE_CHROMA],
		&gdc_fields[GDC_F_GDC_DST_BASE_CHROMA], d_frame->addr.cb);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_DST_2BIT_BASE_LUM],
		&gdc_fields[GDC_F_GDC_DST_2BIT_BASE_LUM], d_frame->addr.y_2bit);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_DST_2BIT_BASE_CHROMA],
		&gdc_fields[GDC_F_GDC_DST_2BIT_BASE_CHROMA], d_frame->addr.cbcr_2bit);

	/* RDMA address */
	GDC_SET_F(pmio, &gdc_regs[GDC_R_PXC_DPB_BASE_LUM],
		&gdc_fields[GDC_F_DPBLUMBASE], s_frame->addr.y);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_PXC_DPB_BASE_CHROMA],
		&gdc_fields[GDC_F_DPBCHRBASE], s_frame->addr.cb);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_PXC_DPB_2BIT_BASE_LUM],
		&gdc_fields[GDC_F_DPB2BITLUMBASE], s_frame->addr.y_2bit);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_PXC_DPB_2BIT_BASE_CHROMA],
		&gdc_fields[GDC_F_DPB2BITCHRBASE], s_frame->addr.cbcr_2bit);
}

void camerapp_hw_gdc_set_format(struct pablo_mmio *pmio,
	struct gdc_frame *s_frame, struct gdc_frame *d_frame)
{
	u32 input_pix_format, input_10bit_format;
	u32 output_pix_format, output_10bit_format;
	u32 input_yuv_format, output_yuv_format;
	u32 mono_format;

	/* PIXEL_FORMAT:	0: NV12 (2plane Y/UV order), 1: NV21 (2plane Y/VU order) */
	/* YUV bit depth:	0: 8bit, 1: P010 , 2: 8+2, 3: packed 10bit(src only) */
	/* YUV format:		0: YUV422, 1: YUV420 */
	/* MONO format:		0: YCbCr, 1: Y only(ignore UV related SFRs) */

	/* Check MONO format */
	mono_format = (s_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_GREY)
					&& (d_frame->gdc_fmt->pixelformat == V4L2_PIX_FMT_GREY);
	if (mono_format) {
		gdc_dbg("gdc mono_mode enabled\n");

		GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_YUV_FORMAT],
			&gdc_fields[GDC_F_MONO_MODE], mono_format);

		return;
	}

	/* input */
	switch (s_frame->gdc_fmt->pixelformat) {
	case V4L2_PIX_FMT_NV12M_P010:
	case V4L2_PIX_FMT_NV21M_P010:
	case V4L2_PIX_FMT_NV16M_P210:
	case V4L2_PIX_FMT_NV61M_P210:
		input_10bit_format = 1;
		break;
	case V4L2_PIX_FMT_NV12M_S10B:
	case V4L2_PIX_FMT_NV21M_S10B:
	case V4L2_PIX_FMT_NV16M_S10B:
	case V4L2_PIX_FMT_NV61M_S10B:
		input_10bit_format = 2;
		break;
	default:
		input_10bit_format = 0;
		break;
	}

	switch (s_frame->gdc_fmt->pixelformat) {
	case V4L2_PIX_FMT_NV21M:
	case V4L2_PIX_FMT_NV21:
	case V4L2_PIX_FMT_NV61:
	case V4L2_PIX_FMT_NV61M:
	case V4L2_PIX_FMT_NV21M_P010:
	case V4L2_PIX_FMT_NV61M_P210:
	case V4L2_PIX_FMT_NV21M_S10B:
	case V4L2_PIX_FMT_NV61M_S10B:
		input_pix_format = 1;
		break;
	default:
		input_pix_format = 0;
		break;
	}

	switch (s_frame->gdc_fmt->pixelformat) {
	case V4L2_PIX_FMT_NV12M:
	case V4L2_PIX_FMT_NV21M:
	case V4L2_PIX_FMT_NV12:
	case V4L2_PIX_FMT_NV21:
	case V4L2_PIX_FMT_NV12M_P010:
	case V4L2_PIX_FMT_NV21M_P010:
	case V4L2_PIX_FMT_NV12M_S10B:
	case V4L2_PIX_FMT_NV21M_S10B:
		input_yuv_format = 1;
		break;
	default:
		input_yuv_format = 0;
		break;
	}

	/* output */
	switch (d_frame->gdc_fmt->pixelformat) {
	case V4L2_PIX_FMT_NV12M_P010:
	case V4L2_PIX_FMT_NV21M_P010:
	case V4L2_PIX_FMT_NV16M_P210:
	case V4L2_PIX_FMT_NV61M_P210:
		output_10bit_format = 1;
		break;
	case V4L2_PIX_FMT_NV12M_S10B:
	case V4L2_PIX_FMT_NV21M_S10B:
	case V4L2_PIX_FMT_NV16M_S10B:
	case V4L2_PIX_FMT_NV61M_S10B:
		output_10bit_format = 2;
		break;
	default:
		output_10bit_format = 0;
		break;
	}

	switch (d_frame->gdc_fmt->pixelformat) {
	case V4L2_PIX_FMT_NV21M:
	case V4L2_PIX_FMT_NV21:
	case V4L2_PIX_FMT_NV61:
	case V4L2_PIX_FMT_NV61M:
	case V4L2_PIX_FMT_NV21M_P010:
	case V4L2_PIX_FMT_NV61M_P210:
	case V4L2_PIX_FMT_NV21M_S10B:
	case V4L2_PIX_FMT_NV61M_S10B:
		output_pix_format = 1;
		break;
	default:
		output_pix_format = 0;
		break;
	}

	switch (d_frame->gdc_fmt->pixelformat) {
	case V4L2_PIX_FMT_NV12M:
	case V4L2_PIX_FMT_NV21M:
	case V4L2_PIX_FMT_NV12:
	case V4L2_PIX_FMT_NV21:
	case V4L2_PIX_FMT_NV12M_P010:
	case V4L2_PIX_FMT_NV21M_P010:
	case V4L2_PIX_FMT_NV12M_S10B:
	case V4L2_PIX_FMT_NV21M_S10B:
		output_yuv_format = 1;
		break;
	default:
		output_yuv_format = 0;
		break;
	}

	gdc_dbg("gdc format (10bit, pix, yuv) : in(%d, %d, %d), out(%d, %d, %d), mono(%d)\n",
		input_10bit_format, input_pix_format, input_yuv_format,
		output_10bit_format, output_pix_format, output_yuv_format,
		mono_format);

	/* IN/OUT Format */
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_YUV_FORMAT],
		&gdc_fields[GDC_F_DST_PIXEL_FORMAT], output_pix_format);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_YUV_FORMAT],
		&gdc_fields[GDC_F_DST_10BIT_FORMAT], output_10bit_format);

	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_YUV_FORMAT],
		&gdc_fields[GDC_F_SRC_PIXEL_FORMAT], input_pix_format);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_YUV_FORMAT],
		&gdc_fields[GDC_F_SRC_10BIT_FORMAT], input_10bit_format);

	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_YUV_FORMAT],
		&gdc_fields[GDC_F_GDC_YUV_FORMAT], input_yuv_format);

}

void camerapp_hw_gdc_set_pixel_minmax(struct pablo_mmio *pmio,
		u32 luma_min, u32 luma_max, u32 chroma_min, u32 chroma_max)
{
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_LUMA_MINMAX],
		&gdc_fields[GDC_F_GDC_LUMA_MIN], luma_min);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_LUMA_MINMAX],
		&gdc_fields[GDC_F_GDC_LUMA_MAX], luma_max);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_CHROMA_MINMAX],
		&gdc_fields[GDC_F_GDC_CHROMA_MIN], chroma_min);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_CHROMA_MINMAX],
		&gdc_fields[GDC_F_GDC_CHROMA_MAX], chroma_max);
}

int camerapp_hw_check_sbwc_fmt(u32 pixelformat)
{
	return -EINVAL;
}

int camerapp_hw_get_sbwc_constraint(u32 pixelformat, u32 width, u32 height, u32 type)
{
	return 0;
}

static int camerapp_hw_get_comp_rate(struct gdc_dev *gdc, u32 pixfmt)
{
	return (pixfmt == V4L2_PIX_FMT_NV12M_SBWCL_8B) ?
		GDC_LOSSY_COMP_RATE_50 : GDC_LOSSY_COMP_RATE_60;
}

bool camerapp_hw_gdc_has_comp_header(u32 comp_extra)
{
	return false;
}

int camerapp_hw_get_comp_buf_size(struct gdc_dev *gdc, struct gdc_frame *frame,
	u32 width, u32 height, u32 pixfmt, enum gdc_buf_plane plane, enum gdc_sbwc_size_type type)
{
	int size;

	frame->comp_rate = camerapp_hw_get_comp_rate(gdc, pixfmt);

	switch (pixfmt) {
	case V4L2_PIX_FMT_NV12M_SBWCL_8B:
		size = plane ?
			SBWCL_8B_CBCR_SIZE(width, height, frame->comp_rate) :
			SBWCL_8B_Y_SIZE(width, height, frame->comp_rate);
		break;
	case V4L2_PIX_FMT_NV12M_SBWCL_10B:
		size = plane ?
			SBWCL_10B_CBCR_SIZE(width, height, frame->comp_rate) :
			SBWCL_10B_Y_SIZE(width, height, frame->comp_rate);
		break;
	default:
		v4l2_err(&gdc->m2m.v4l2_dev, "not supported format values\n");
		size = -EINVAL;
		break;
	}

	return size;
}

void camerapp_hw_gdc_set_initialization(struct pablo_mmio *pmio)
{
}

void camerapp_hw_gdc_update_param(struct pablo_mmio *pmio, struct gdc_dev *gdc)
{
	struct gdc_frame *d_frame, *s_frame;
	struct gdc_crop_param *crop_param;
	u32 input_dither = 0, output_dither = 0;
	u32 luma_min, luma_max, chroma_min, chroma_max;

	s_frame = &gdc->current_ctx->s_frame;
	d_frame = &gdc->current_ctx->d_frame;
	crop_param = &gdc->current_ctx->crop_param[gdc->current_ctx->cur_index];

	/* Dither : 0 = off, 1 = on => read(inverse_dither), write (dither) */
	if (s_frame->pixel_size == CAMERA_PIXEL_SIZE_8_2BIT)
		input_dither = 1;
	if (d_frame->pixel_size == CAMERA_PIXEL_SIZE_8_2BIT)
		output_dither = 1;
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_DITHER],
		&gdc_fields[GDC_F_DST_DITHER], output_dither);
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_DITHER],
		&gdc_fields[GDC_F_SRC_INVDITHER], input_dither);

	/* interpolation type */
	/* 0 -all closest / 1 - Y bilinear, UV closest / 2 - all bilinear / 3 - Y bi-cubic, UV bilinear */
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_INTERPOLATION],
		&gdc_fields[GDC_F_GDC_INTERP_TYPE], 3);
	/* Clamping type: 0 - none / 1 - min/max / 2 - near pixels min/max */
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_INTERPOLATION],
		&gdc_fields[GDC_F_GDC_CLAMP_TYPE], 2);

	/* gdc grid scale setting */
	camerapp_hw_gdc_update_scale_parameters(pmio, s_frame, d_frame, crop_param);
	/* gdc grid setting */
	camerapp_hw_gdc_update_grid_param(pmio, crop_param);

	/* dma buffer size & address setting */
	camerapp_hw_gdc_set_dma_address(pmio, s_frame, d_frame);
	camerapp_hw_gdc_update_dma_size(pmio, s_frame, d_frame);

	/* in / out data Format */
	camerapp_hw_gdc_set_format(pmio, s_frame, d_frame);

	/* output pixcel min/max value clipping */
	luma_min = chroma_min = 0;
	luma_max = chroma_max = 0x3ff;
	camerapp_hw_gdc_set_pixel_minmax(pmio, luma_min, luma_max, chroma_min, chroma_max);

	/* The values are multiples of 32, value : 32 ~ 512 */
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_PRO_SIZE], &gdc_fields[GDC_F_GDC_PRO_WIDTH], 64);
	/* The values are multiples of 8, value : 8 ~ 512 */
	GDC_SET_F(pmio, &gdc_regs[GDC_R_GDC_PRO_SIZE], &gdc_fields[GDC_F_GDC_PRO_HEIGHT], 64);
	GDC_SET_R(pmio, &gdc_regs[GDC_R_GDC_PROCESSING], 1);
}


void camerapp_hw_gdc_status_read(struct pablo_mmio *pmio)
{
	u32 ReqCntLum;
	u32 HitCntLum;
	u32 isPixelBusy;
	u32 isPortBusy;
	u32 cacheStatus0;
	u32 hitFifoStatus0;
	u32 missFifoStatus0;
	u32 isPending;

	/* read Total request count of luminance cache channel #0 */
	ReqCntLum = GDC_GET_R(pmio, &gdc_regs[GDC_R_PXC_REQ_CNT_LUM_0]);
	HitCntLum = GDC_GET_R(pmio, &gdc_regs[GDC_R_PXC_HIT_CNT_LUM_0]);
	isPixelBusy = GDC_GET_F(pmio, &gdc_regs[GDC_R_PXC_DEBUG_BUSY], &gdc_fields[GDC_F_PXCBUSYLUM]);
	isPortBusy = GDC_GET_F(pmio, &gdc_regs[GDC_R_PXC_DEBUG_BUSY], &gdc_fields[GDC_F_PORT0BUSY]);
	cacheStatus0 = GDC_GET_F(pmio,
		&gdc_regs[GDC_R_PXC_DEBUG_LUM0], &gdc_fields[GDC_F_CHROMACACHEST0]);
	hitFifoStatus0 = GDC_GET_F(pmio,
		&gdc_regs[GDC_R_PXC_DEBUG_LUM0], &gdc_fields[GDC_F_HITFIFOST0]);
	missFifoStatus0 = GDC_GET_F(pmio,
		&gdc_regs[GDC_R_PXC_DEBUG_LUM0], &gdc_fields[GDC_F_MISSFIFOST0]);

	isPending = GDC_GET_R(pmio, &gdc_regs[GDC_R_PEND]);

	pr_info("[GDC_Status] = %x /%x /%x /%x /%x /%x /%x /%x\n",
		ReqCntLum, HitCntLum,
		isPixelBusy, isPortBusy,
		cacheStatus0, hitFifoStatus0, missFifoStatus0,
		isPending);

}

void camerapp_hw_gdc_votf_enable(struct pablo_mmio *pmio, u8 rw)
{
	return;
}

void camerapp_gdc_sfr_dump(void __iomem *base_addr)
{
	u32 i = 0;
	u32 reg_value = 0;

	gdc_dbg("gdc v2.1");

	for (i = 0; i < GDC_REG_CNT; i++) {
		reg_value = readl(base_addr + gdc_regs[i].sfr_offset);
		pr_info("[DUMP] reg:[%s][0x%04X], value:[0x%08X]\n",
			gdc_regs[i].reg_name, gdc_regs[i].sfr_offset, reg_value);
	}
}

u32 camerapp_hw_gdc_g_reg_cnt(void)
{
	return GDC_REG_CNT;
}

void camerapp_hw_gdc_init_pmio_config(struct gdc_dev *gdc)
{
	struct pmio_config *cfg = &gdc->pmio_config;

	cfg->mmio_base = gdc->regs_base;
	cfg->fields = NULL;
	cfg->num_fields = 0;
}
