/*
 * drivers/media/platform/exynos/mfc/mfc_core_sync.h
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __MFC_CORE_SYNC_H
#define __MFC_CORE_SYNC_H __FILE__

#include "base/mfc_common.h"

#define need_to_dpb_flush(core_ctx)		\
	((core_ctx->state == MFCINST_FINISHING) ||	\
	 (core_ctx->state == MFCINST_RUNNING)	||	\
	 (core_ctx->state == MFCINST_RES_CHANGE_END))
#define need_to_wait_nal_abort(core_ctx)		 \
	(core_ctx->state == MFCINST_ABORT_INST)
#define need_to_special_parsing(core_ctx)		\
	((core_ctx->state == MFCINST_GOT_INST) ||	\
	 (core_ctx->state == MFCINST_HEAD_PARSED))
#define need_to_special_parsing_nal(core_ctx)	\
	(core_ctx->state == MFCINST_RUNNING)
#define ready_to_get_crop(core_ctx)			\
	((core_ctx->state == MFCINST_HEAD_PARSED) ||		\
	(core_ctx->state == MFCINST_RUNNING) ||		\
	(core_ctx->state == MFCINST_SPECIAL_PARSING) ||	\
	(core_ctx->state == MFCINST_SPECIAL_PARSING_NAL) ||	\
	(core_ctx->state == MFCINST_FINISHING))

void mfc_get_corelock_ctx(struct mfc_ctx *ctx);
void mfc_release_corelock_ctx(struct mfc_ctx *ctx);
int mfc_get_corelock_migrate(struct mfc_ctx *ctx);
void mfc_release_corelock_migrate(struct mfc_ctx *ctx);

int mfc_wait_for_done_core(struct mfc_core *core, int command);
int mfc_wait_for_done_core_ctx(struct mfc_core_ctx *core_ctx, int command);
void mfc_wake_up_core(struct mfc_core *core, unsigned int reason,
		unsigned int err);
void mfc_wake_up_core_ctx(struct mfc_core_ctx *core_ctx, unsigned int reason,
		unsigned int err);

int mfc_core_get_new_ctx(struct mfc_core *core);
int mfc_core_get_next_ctx(struct mfc_core *core);

int mfc_ctx_ready_set_bit_raw(struct mfc_core_ctx *core_ctx, unsigned long *bits, bool set);
int mfc_ctx_ready_set_bit(struct mfc_core_ctx *core_ctx, struct mfc_bits *data);
int mfc_ctx_ready_clear_bit(struct mfc_core_ctx *core_ctx, struct mfc_bits *data);

#endif /* __MFC_CORE_SYNC_H */
