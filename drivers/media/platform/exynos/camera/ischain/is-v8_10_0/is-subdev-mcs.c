/*
 * Samsung Exynos5 SoC series FIMC-IS driver
 *
 * exynos5 fimc-is video functions
 *
 * Copyright (c) 2018 Samsung Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "is-device-ischain.h"
#include "is-subdev-ctrl.h"
#include "is-config.h"
#include "is-param.h"
#include "is-video.h"
#include "is-type.h"

#ifdef USE_VRA_OTF
static int is_ischain_mcs_bypass(struct is_subdev *leader,
	void *device_data,
	struct is_frame *frame,
	bool bypass)
{
	int ret = 0;
	struct is_group *group;
	struct param_mcs_output *output;
	struct is_device_ischain *device;
	struct is_subdev *subdev = NULL, *subdev_temp = NULL, *temp;
	IS_DECLARE_PMAP(pmap);

	device = (struct is_device_ischain *)device_data;

	FIMC_BUG(!leader);
	FIMC_BUG(!device);

	group = &device->group_mcs;
	IS_INIT_PMAP(pmap);

	list_for_each_entry_safe(subdev_temp, temp, &group->subdev_list, list) {
		if (subdev_temp == &group->leader)
			continue;

		if (test_bit(IS_SUBDEV_START, &subdev_temp->state)) {
			subdev = subdev_temp;
			goto find_subdev;
		}
	}

find_subdev:
	if (!subdev) {
		mgerr("subdev is NULL\n", device, group);
		goto p_err;
	}

	output = is_itf_g_param(device, frame, subdev->param_dma_ot);

	if (bypass)
		output->otf_cmd = OTF_OUTPUT_COMMAND_DISABLE;
	else
		output->otf_cmd = OTF_OUTPUT_COMMAND_ENABLE;

	set_bit(subdev->param_dma_ot, pmap);

	ret = is_itf_s_param(device, frame, pmap);
	if (ret) {
		mrerr("is_itf_s_param is fail(%d)", device, frame, ret);
		goto p_err;
	}

	minfo("VRA is connected at %s\n", device, group->junction->name);

p_err:
	return ret;
}
#endif

void is_ischain_mcs_stripe_cfg(struct is_subdev *subdev,
		struct is_frame *frame,
		struct is_crop *incrop,
		struct is_crop *otcrop)
{
	u32 stripe_w;
	u32 region_id = frame->stripe_info.region_id;

	if (!frame->stripe_info.region_id) {
		/* Left region */
		stripe_w = frame->stripe_info.in.h_pix_num[region_id];
	} else if (frame->stripe_info.region_id < frame->stripe_info.region_num - 1) {
		/* Middle region */
		stripe_w = frame->stripe_info.in.h_pix_num[region_id] - frame->stripe_info.in.h_pix_num[region_id - 1];
		stripe_w += STRIPE_MARGIN_WIDTH;
	} else {
		/* Right region */
		stripe_w = frame->stripe_info.in.h_pix_num[region_id] - frame->stripe_info.in.h_pix_num[region_id - 1];
	}
	stripe_w += STRIPE_MARGIN_WIDTH;
	incrop->w = stripe_w;

	/**
	 * Output crop configuration.
	 * No crop & scale.
	 */
	otcrop->x = 0;
	otcrop->y = 0;
	otcrop->w = incrop->w;
	otcrop->h = incrop->h;

	msrdbgs(3, "stripe_in_crop[%d][%d, %d, %d, %d]\n", subdev, subdev, frame,
			region_id,
			incrop->x, incrop->y, incrop->w, incrop->h);
	msrdbgs(3, "stripe_ot_crop[%d][%d, %d, %d, %d]\n", subdev, subdev, frame,
			region_id,
			otcrop->x, otcrop->y, otcrop->w, otcrop->h);
}

static int is_ischain_mcs_cfg(struct is_subdev *leader,
	void *device_data,
	struct is_frame *frame,
	struct is_crop *incrop,
	struct is_crop *otcrop,
	IS_DECLARE_PMAP(pmap))
{
	int i;
	int ret = 0;
	struct is_fmt *format;
	struct is_group *group;
	struct is_queue *queue;
	struct param_mcs_input *input;
	struct param_stripe_input *stripe_input;
	struct param_control *control;
	struct is_device_ischain *device;
	struct is_crop incrop_cfg, otcrop_cfg;
#ifdef USE_VRA_OTF
	struct is_subdev *subdev = NULL, *subdev_temp = NULL, *temp;
#endif

	device = (struct is_device_ischain *)device_data;

	FIMC_BUG(!leader);
	FIMC_BUG(!device);
	FIMC_BUG(!device->sensor);
	FIMC_BUG(!incrop);

	group = &device->group_mcs;
	incrop_cfg = *incrop;
	otcrop_cfg = *otcrop;

	queue = GET_SUBDEV_QUEUE(leader);
	if (!queue) {
		merr("queue is NULL", device);
		ret = -EINVAL;
		goto p_err;
	}

	if (IS_ENABLED(CHAIN_STRIPE_PROCESSING) && frame && frame->stripe_info.region_num)
		is_ischain_mcs_stripe_cfg(leader, frame,
				&incrop_cfg, &otcrop_cfg);

	stripe_input = is_itf_g_param(device, frame, PARAM_MCS_STRIPE_INPUT);
	if (frame && frame->stripe_info.region_num) {
		stripe_input->index = frame->stripe_info.region_id;
		stripe_input->total_count = frame->stripe_info.region_num;
		if (!frame->stripe_info.region_id) {
			stripe_input->left_margin = 0;
			stripe_input->right_margin = STRIPE_MARGIN_WIDTH;
			stripe_input->stripe_roi_start_pos_x[0] = 0;
			for (i = 1; i <= frame->stripe_info.region_num; ++i)
				stripe_input->stripe_roi_start_pos_x[i] = frame->stripe_info.in.h_pix_num[i - 1];
		} else if (frame->stripe_info.region_id < frame->stripe_info.region_num - 1) {
			stripe_input->left_margin = STRIPE_MARGIN_WIDTH;
			stripe_input->right_margin = STRIPE_MARGIN_WIDTH;
		} else {
			stripe_input->left_margin = STRIPE_MARGIN_WIDTH;
			stripe_input->right_margin = 0;
		}
		stripe_input->full_width = incrop->w;
		stripe_input->full_height = incrop->h;
	} else {
		stripe_input->index = 0;
		stripe_input->total_count = 0;
		stripe_input->left_margin = 0;
		stripe_input->right_margin = 0;
		stripe_input->full_width = 0;
		stripe_input->full_height = 0;
	}
	set_bit(PARAM_MCS_STRIPE_INPUT, pmap);

	input = is_itf_g_param(device, frame, PARAM_MCS_INPUT);
	if (test_bit(IS_GROUP_OTF_INPUT, &group->state)) {
		input->otf_cmd = OTF_INPUT_COMMAND_ENABLE;
		input->dma_cmd = DMA_INPUT_COMMAND_DISABLE;
		input->width = incrop_cfg.w;
		input->height = incrop_cfg.h;
	} else {
		input->otf_cmd = OTF_INPUT_COMMAND_DISABLE;
		input->dma_cmd = DMA_INPUT_COMMAND_ENABLE;
		input->width = leader->input.canv.w;
		input->height = leader->input.canv.h;
		input->dma_crop_offset_x = leader->input.canv.x + incrop_cfg.x;
		input->dma_crop_offset_y = leader->input.canv.y + incrop_cfg.y;
		input->dma_crop_width = incrop_cfg.w;
		input->dma_crop_height = incrop_cfg.h;

		format = queue->framecfg.format;
		input->dma_format = format->hw_format;
		input->dma_bitwidth = format->hw_bitwidth;
		input->dma_order = format->hw_order;
		input->plane = format->hw_plane;

		/*
		 * HW spec: DMA stride should be aligned by 16 byte.
		 */
		input->dma_stride_y = ALIGN(max(input->dma_crop_width * format->bitsperpixel[0] / BITS_PER_BYTE,
					queue->framecfg.bytesperline[0]), 16);
		input->dma_stride_c = ALIGN(max(input->dma_crop_width * format->bitsperpixel[1] / BITS_PER_BYTE,
					queue->framecfg.bytesperline[1]), 16);

	}

	input->otf_format = OTF_INPUT_FORMAT_YUV422;
	input->otf_bitwidth = OTF_INPUT_BIT_WIDTH_8BIT;
	input->otf_order = OTF_INPUT_ORDER_BAYER_GR_BG;

	input->stripe_in_start_pos_x = stripe_input->stripe_roi_start_pos_x[stripe_input->index]
								- stripe_input->left_margin;
	input->stripe_roi_start_pos_x = stripe_input->stripe_roi_start_pos_x[stripe_input->index];
	input->stripe_roi_end_pos_x = stripe_input->stripe_roi_start_pos_x[stripe_input->index + 1];
	input->stripe_in_end_pos_x = stripe_input->stripe_roi_start_pos_x[stripe_input->index + 1]
								+ stripe_input->right_margin;
	set_bit(PARAM_MCS_INPUT, pmap);

	/* Configure Control */
	control = is_itf_g_param(device, frame, PARAM_MCS_CONTROL);
	control->cmd = CONTROL_COMMAND_START;
#ifdef ENABLE_DNR_IN_MCSC
	control->buffer_address = device->minfo->dvaddr_mcsc_dnr;
#endif
	set_bit(PARAM_MCS_CONTROL, pmap);

	leader->input.crop = *incrop;

	if (test_bit(IS_ISCHAIN_REPROCESSING, &device->state)) {
		mswarn("reprocessing cannot connect to VRA\n", device, leader);
		goto p_err;
	}

#ifdef USE_VRA_OTF
	list_for_each_entry_safe(subdev_temp, temp, &group->subdev_list, list) {
		if (subdev_temp == &group->leader)
			continue;

		if (test_bit(IS_SUBDEV_START, &subdev_temp->state)) {
			subdev = subdev_temp;
			goto find_subdev;
		}
	}

find_subdev:
	if (!subdev) {
		mgerr("subdev is NULL\n", device, group);
		goto p_err;
	}

	CALL_SOPS(subdev, cfg, device, frame, incrop, NULL, pmap);
#endif

p_err:
	return ret;
}

static int is_ischain_mcs_tag(struct is_subdev *subdev,
	void *device_data,
	struct is_frame *frame,
	struct camera2_node *node)
{
	int ret = 0;
	struct is_group *group;
	struct mcs_param *mcs_param;
	struct is_crop inparm;
	struct is_crop *incrop, *otcrop;
	struct is_subdev *leader;
	struct is_device_ischain *device;
	IS_DECLARE_PMAP(pmap);

	device = (struct is_device_ischain *)device_data;

	FIMC_BUG(!subdev);
	FIMC_BUG(!device);
	FIMC_BUG(!device->is_region);
	FIMC_BUG(!frame);

	mdbgs_ischain(4, "MCSC TAG\n", device);

	incrop = (struct is_crop *)node->input.cropRegion;
	otcrop = (struct is_crop *)node->output.cropRegion;

	group = &device->group_mcs;
	leader = subdev->leader;
	IS_INIT_PMAP(pmap);
	mcs_param = &device->is_region->parameter.mcs;

	if (test_bit(IS_GROUP_OTF_INPUT, &group->state)) {
		inparm.x = 0;
		inparm.y = 0;
		inparm.w = mcs_param->input.width;
		inparm.h = mcs_param->input.height;
	} else {
		inparm.x = mcs_param->input.dma_crop_offset_x;
		inparm.y = mcs_param->input.dma_crop_offset_y;
		inparm.w = mcs_param->input.dma_crop_width;
		inparm.h = mcs_param->input.dma_crop_height;
	}

	if (IS_NULL_CROP(incrop))
		*incrop = inparm;

	if (!COMPARE_CROP(incrop, &inparm) ||
		CHECK_STRIPE_CFG(&frame->stripe_info) ||
		test_bit(IS_SUBDEV_FORCE_SET, &leader->state)) {
		ret = is_ischain_mcs_cfg(subdev,
			device,
			frame,
			incrop,
			otcrop,
			pmap);
		if (ret) {
			merr("is_ischain_mcs_start is fail(%d)", device, ret);
			goto p_err;
		}

		if (!COMPARE_CROP(incrop, &subdev->input.crop) ||
			debug_stream) {
			msrinfo("in_crop[%d, %d, %d, %d]\n", device, subdev, frame,
				incrop->x, incrop->y, incrop->w, incrop->h);
			subdev->input.crop = *incrop;
		}
	}

	ret = is_itf_s_param(device, frame, pmap);
	if (ret) {
		mrerr("is_itf_s_param is fail(%d)", device, frame, ret);
		goto p_err;
	}

p_err:
	return ret;
}

static const struct is_subdev_ops is_subdev_mcs_ops = {
#ifdef USE_VRA_OTF
	.bypass			= is_ischain_mcs_bypass,
#else
	.bypass			= NULL,
#endif
	.cfg			= is_ischain_mcs_cfg,
	.tag			= is_ischain_mcs_tag,
};

const struct is_subdev_ops *pablo_get_is_subdev_mcs_ops(void)
{
	return &is_subdev_mcs_ops;
}
KUNIT_EXPORT_SYMBOL(pablo_get_is_subdev_mcs_ops);
