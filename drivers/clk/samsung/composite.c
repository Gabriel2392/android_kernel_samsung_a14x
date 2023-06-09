/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This file contains the utility functions for composite clocks.
 */

#include <linux/syscore_ops.h>
#include <linux/errno.h>
#include <linux/log2.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <soc/samsung/cal-if.h>

#include "composite.h"

#define PLL_STAT_OSC	(0x0)
#define PLL_STAT_PLL	(0x1)
#define PLL_STAT_CHANGE	(0x4)
#define PLL_STAT_MASK	(0x7)

#define to_comp_pll(_hw) container_of(_hw, struct samsung_composite_pll, hw)
#define to_comp_mux(_hw) container_of(_hw, struct samsung_composite_mux, hw)
#define to_comp_divider(_hw) container_of(_hw, struct samsung_composite_divider, hw)
#define to_usermux(_hw) container_of(_hw, struct clk_samsung_usermux, hw)
#define to_vclk(_hw) container_of(_hw, struct samsung_vclk, hw)

#define BIT32(nr)			(1 << (nr))

static DEFINE_SPINLOCK(lock);
extern bool clk_exynos_skip_hw;

/* setup the essentials required to support clock lookup using ccf */
struct samsung_clk_provider * samsung_clk_init(struct device_node *np,
			void __iomem *base, unsigned long nr_clks)
{
	struct samsung_clk_provider *ctx = NULL;
	struct clk **clk_table;
	int i;

	if (!np)
		return ctx;

	ctx = kzalloc(sizeof(struct samsung_clk_provider), GFP_KERNEL);
	if (!ctx)
		panic("could not allocate clock provider context.\n");

	clk_table = kcalloc(nr_clks, sizeof(struct clk *), GFP_KERNEL);
	if (!clk_table)
		panic("could not allocate clock lookup table\n");

	for (i = 0; i < (unsigned int)nr_clks; ++i)
		clk_table[i] = ERR_PTR(-ENOENT);

	ctx->reg_base = base;
	ctx->clk_data.clks = clk_table;
	ctx->clk_data.clk_num = (unsigned int)nr_clks;
	spin_lock_init(&ctx->lock);

	return ctx;
}

void  samsung_clk_of_add_provider(struct device_node *np,
				struct samsung_clk_provider *ctx)
{
	if (np) {
		if (of_clk_add_provider(np, of_clk_src_onecell_get,
					&ctx->clk_data))
			panic("could not register clk provider\n");
	}
}

/* add a clock instance to the clock lookup table used for dt based lookup */
static void samsung_clk_add_lookup(struct samsung_clk_provider *ctx, struct clk *clk,
					unsigned int id)
{
	if (ctx->clk_data.clks && id)
		ctx->clk_data.clks[id] = clk;
}

/* register a list of fixed clocks */
void  samsung_register_fixed_rate(struct samsung_clk_provider *ctx,
		struct samsung_fixed_rate *list, unsigned int nr_clk)
{
	struct clk *clk;
	unsigned int idx, ret;

	for (idx = 0; idx < nr_clk; idx++, list++) {
		clk = clk_register_fixed_rate(NULL, list->name,
			list->parent_name, list->flags, list->fixed_rate);
		if (IS_ERR(clk)) {
			pr_err("%s: failed to register clock %s\n", __func__,
				list->name);
			continue;
		}

		samsung_clk_add_lookup(ctx, clk, list->id);

		/*
		 * Unconditionally add a clock lookup for the fixed rate clocks.
		 * There are not many of these on any of Samsung platforms.
		 */
		ret = clk_register_clkdev(clk, list->name, NULL);
		if (ret)
			pr_err("%s: failed to register clock lookup for %s",
				__func__, list->name);
	}
}

/* register a list of fixed factor clocks */
void  samsung_register_fixed_factor(struct samsung_clk_provider *ctx,
		struct samsung_fixed_factor *list, unsigned int nr_clk)
{
	struct clk *clk;
	unsigned int idx, ret;

	for (idx = 0; idx < nr_clk; idx++, list++) {
		clk = clk_register_fixed_factor(NULL, list->name,
			list->parent_name, list->flags, list->mult, list->div);
		if (IS_ERR(clk)) {
			pr_err("%s: failed to register clock %s\n", __func__,
				list->name);
			continue;
		}

		samsung_clk_add_lookup(ctx, clk, list->id);

		ret = clk_register_clkdev(clk, list->name, NULL);
		if (ret)
			pr_err("%s: failed to register clock lookup for %s",
				__func__, list->name);
	}
}

/*
 * obtain the clock speed of all external fixed clock sources from device
 * tree and register it
 */
void  samsung_register_of_fixed_ext(struct samsung_clk_provider *ctx,
			struct samsung_fixed_rate *fixed_rate_clk,
			unsigned int nr_fixed_rate_clk,
			struct of_device_id *clk_matches)
{
	const struct of_device_id *match;
	struct device_node *np;
	u32 freq;

	for_each_matching_node_and_match(np, clk_matches, &match) {
		if (of_property_read_u32(np, "clock-frequency", &freq))
			continue;
		fixed_rate_clk[(unsigned long)match->data].fixed_rate = freq;
	}
	samsung_register_fixed_rate(ctx, fixed_rate_clk, nr_fixed_rate_clk);
}

/* operation functions for pll clocks */
static const struct samsung_pll_rate_table *samsung_get_pll_settings(
				struct samsung_composite_pll *pll, unsigned long rate)
{
	const struct samsung_pll_rate_table  *rate_table = pll->rate_table;
	int i;

	for (i = 0; i < pll->rate_count; i++) {
		if (rate == rate_table[i].rate)
			return &rate_table[i];
	}

	return NULL;
}

static long samsung_pll_round_rate(struct clk_hw *hw,
			unsigned long drate, unsigned long *prate)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	const struct samsung_pll_rate_table *rate_table = pll->rate_table;
	int i;

	/* Assumming rate_table is in descending order */
	for (i = 0; i < pll->rate_count; i++) {
		if (drate >= rate_table[i].rate)
			return rate_table[i].rate;
	}

	/* return minimum supported value */
	return rate_table[i - 1].rate;
}

static int samsung_composite_pll_is_enabled(struct clk_hw *hw)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	int set = pll->pll_flag & PLL_BYPASS ? 0 : 1;
	unsigned int reg;

	reg = readl(pll->enable_reg);

	return (((reg >> pll->enable_bit) & 1) == set) ? 1 : 0;
}

static int samsung_composite_pll_enable(struct clk_hw *hw)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	int set = pll->pll_flag & PLL_BYPASS ? 0 : 1;
	unsigned int reg;

	/* Setting Enable register */
	reg = readl(pll->enable_reg);
	if (set)
		reg |= (1 << pll->enable_bit);
	else
		reg &= ~(1 << pll->enable_bit);
	writel(reg, pll->enable_reg);

	/* setting CTRL mux register to 1 */
	reg = readl(pll->sel_reg);
	reg |= (1 << pll->sel_bit);
	writel(reg, pll->sel_reg);

	/* check status for mux setting */
	do {
		cpu_relax();
		reg = readl(pll->stat_reg);
	} while (((reg >> pll->stat_bit) & PLL_STAT_MASK) != PLL_STAT_PLL);

	return 0;
}

static void samsung_composite_pll_disable(struct clk_hw *hw)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	int set = pll->pll_flag & PLL_BYPASS ? 0 : 1;
	unsigned int reg;

	/* setting CTRL mux register to 0 */
	reg = readl(pll->sel_reg);
	reg &= ~(1 << pll->sel_bit);
	writel(reg, pll->sel_reg);

	/* check status for mux setting */
	do {
		cpu_relax();
		reg = readl(pll->stat_reg);
	} while (((reg >> pll->stat_bit) & PLL_STAT_MASK) != PLL_STAT_OSC);

	/* Setting Register */
	reg = readl(pll->enable_reg);
	if (set)
		reg &= ~(1 << pll->enable_bit);
	else
		reg |= (1 << pll->enable_bit);

	writel(reg, pll->enable_reg);
}

static unsigned long samsung_pll145xx_recalc_rate(struct clk_hw *hw,
				unsigned long parent_rate)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	u32 mdiv, pdiv, sdiv, pll_con;
	u64 fvco = parent_rate;

	pll_con = readl(pll->con_reg);
	mdiv = (pll_con >> PLL145XX_MDIV_SHIFT) & PLL145XX_MDIV_MASK;
	pdiv = (pll_con >> PLL145XX_PDIV_SHIFT) & PLL145XX_PDIV_MASK;
	sdiv = (pll_con >> PLL145XX_SDIV_SHIFT) & PLL145XX_SDIV_MASK;
	/* Do calculation */
	fvco *= mdiv;
	do_div(fvco, (pdiv << sdiv));

	return (unsigned long)fvco;
}

static inline bool samsung_pll145xx_mp_check(u32 mdiv, u32 pdiv, u32 pll_con)
{
	return ((mdiv != ((pll_con >> PLL145XX_MDIV_SHIFT) & PLL145XX_MDIV_MASK)) ||
		(pdiv != ((pll_con >> PLL145XX_PDIV_SHIFT) & PLL145XX_PDIV_MASK)));
}

static int samsung_pll145xx_set_rate(struct clk_hw *hw, unsigned long drate,
					unsigned long prate)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	const struct samsung_pll_rate_table *rate;
	u32 pll_con;

	rate = samsung_get_pll_settings(pll, drate);
	if (!rate) {
		pr_err("%s: Invalid rate : %lu for pll clk %s\n", __func__,
				drate, __clk_get_name(hw->clk));
		return -EINVAL;
	}

	pll_con = readl(pll->con_reg);
	if (!(samsung_pll145xx_mp_check(rate->mdiv, rate->pdiv, pll_con))) {
		if ((rate->sdiv) == ((pll_con >> PLL145XX_SDIV_SHIFT) & PLL145XX_SDIV_MASK))
			return 0;
		/* In the case of changing S value only */
		pll_con &= ~(PLL145XX_SDIV_MASK << PLL145XX_SDIV_SHIFT);
		pll_con |= rate->sdiv << PLL145XX_SDIV_SHIFT;
		writel(pll_con, pll->con_reg);

		return 0;
	}

	/* Set PLL lock time */
	writel(rate->pdiv * PLL145XX_LOCK_FACTOR, pll->lock_reg);
	/* Change PLL PMS values */
	pll_con &= ~((PLL145XX_MDIV_MASK << PLL145XX_MDIV_SHIFT) |
			(PLL145XX_PDIV_MASK << PLL145XX_PDIV_SHIFT) |
			(PLL145XX_SDIV_MASK << PLL145XX_SDIV_SHIFT));
	pll_con |= (rate->mdiv << PLL145XX_MDIV_SHIFT) |
			(rate->pdiv << PLL145XX_PDIV_SHIFT) |
			(rate->sdiv << PLL145XX_SDIV_SHIFT);
	/* To prevent instable PLL operation, preset ENABLE bit with 0 */
	pll_con &= ~BIT32(31);
	writel(pll_con, pll->con_reg);

	/* Set enable bit */
	pll_con |= BIT32(31);
	writel(pll_con, pll->con_reg);

	do {
		cpu_relax();
		pll_con = readl(pll->con_reg);
	} while (!(pll_con & (PLL145XX_LOCKED_MASK << PLL145XX_LOCKED_SHIFT)));

	return 0;
}


static unsigned long samsung_pll1460x_recalc_rate(struct clk_hw *hw,
				unsigned long parent_rate)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	u32 mdiv, pdiv, sdiv, pll_con0, pll_con1;
	s16 kdiv;
	u64 fvco = parent_rate;

	pll_con0 = readl(pll->con_reg);
	pll_con1 = readl(pll->con_reg + 4);
	mdiv = (pll_con0 >> PLL1460X_MDIV_SHIFT) & PLL1460X_MDIV_MASK;
	pdiv = (pll_con0 >> PLL1460X_PDIV_SHIFT) & PLL1460X_PDIV_MASK;
	sdiv = (pll_con0 >> PLL1460X_SDIV_SHIFT) & PLL1460X_SDIV_MASK;
	kdiv = (s16)((pll_con1 >> PLL1460X_KDIV_SHIFT) & PLL1460X_KDIV_MASK);
	/* Do calculation */
	fvco *= (mdiv << 16) + kdiv;
	do_div(fvco, (pdiv << sdiv));
	fvco >>= 16;

	return (unsigned long)fvco;
}

static inline bool samsung_pll1460x_mpk_check(u32 mdiv, u32 pdiv, u32 kdiv, u32 pll_con0, u32 pll_con1)
{
	return ((mdiv != ((pll_con0 >> PLL1460X_MDIV_SHIFT) & PLL1460X_MDIV_MASK)) ||
		(pdiv != ((pll_con0 >> PLL1460X_PDIV_SHIFT) & PLL1460X_PDIV_MASK)) ||
		(kdiv != ((pll_con1 >> PLL1460X_KDIV_SHIFT) & PLL1460X_KDIV_MASK)));
}

static int samsung_pll1460x_set_rate(struct clk_hw *hw, unsigned long drate,
					unsigned long parent_rate)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	u32 pll_con0, pll_con1;
	const struct samsung_pll_rate_table *rate;

	rate = samsung_get_pll_settings(pll, drate);
	if (!rate) {
		pr_err("%s: Invalid rate : %lu for pll clk %s\n", __func__,
				drate, __clk_get_name(hw->clk));
		return -EINVAL;
	}

	pll_con0 = readl(pll->con_reg);
	pll_con1 = readl(pll->con_reg + 4);
	if (!(samsung_pll1460x_mpk_check(rate->mdiv, rate->pdiv, rate->kdiv, pll_con0, pll_con1))) {
		if ((rate->sdiv) == ((pll_con0 >> PLL1460X_SDIV_SHIFT) & PLL1460X_SDIV_MASK))
			return 0;
		/* In the case of changing S value only */
		pll_con0 &= ~(PLL1460X_SDIV_MASK << PLL1460X_SDIV_SHIFT);
		pll_con0 |= (rate->sdiv << PLL1460X_SDIV_SHIFT);
		writel(pll_con0, pll->con_reg);

		return 0;
	}

	/* Set PLL lock time */
	writel(rate->pdiv * PLL1460X_LOCK_FACTOR, pll->lock_reg);

	pll_con1 &= ~(PLL1460X_KDIV_MASK << PLL1460X_KDIV_SHIFT);
	pll_con1 |= (rate->kdiv << PLL1460X_KDIV_SHIFT);
	writel(pll_con1, pll->con_reg + 4);

	pll_con0 &= ~((PLL1460X_MDIV_MASK << PLL1460X_MDIV_SHIFT) |
			(PLL1460X_PDIV_MASK << PLL1460X_PDIV_SHIFT) |
			(PLL1460X_SDIV_MASK << PLL1460X_SDIV_SHIFT));
	pll_con0 |= (rate->mdiv << PLL1460X_MDIV_SHIFT) |
			(rate->pdiv << PLL1460X_PDIV_SHIFT) |
			(rate->sdiv << PLL1460X_SDIV_SHIFT);
	/* To prevent instable PLL operation, preset ENABLE bit with 0 */
	pll_con0 &= ~BIT32(31);
	writel(pll_con0, pll->con_reg);

	/* Set enable bit */
	pll_con0 |= BIT32(31);
	writel(pll_con0, pll->con_reg);

	/* Wait lock time */
	do {
		cpu_relax();
		pll_con0 = readl(pll->con_reg);
	} while (!(pll_con0 & (PLL1460X_LOCKED_MASK << PLL1460X_LOCKED_SHIFT)));

	return 0;
}

static const struct clk_ops samsung_pll145xx_clk_ops = {
	.recalc_rate = samsung_pll145xx_recalc_rate,
	.set_rate = samsung_pll145xx_set_rate,
	.round_rate = samsung_pll_round_rate,
	.enable = samsung_composite_pll_enable,
	.disable = samsung_composite_pll_disable,
	.is_enabled = samsung_composite_pll_is_enabled,
};

static const struct clk_ops samsung_pll1460x_clk_ops = {
	.recalc_rate = samsung_pll1460x_recalc_rate,
	.set_rate = samsung_pll1460x_set_rate,
	.round_rate = samsung_pll_round_rate,
	.enable = samsung_composite_pll_enable,
	.disable = samsung_composite_pll_disable,
	.is_enabled = samsung_composite_pll_is_enabled,
};

static int samsung_composite_pll_enable_onchange(struct clk_hw *hw)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	int set = pll->pll_flag & PLL_BYPASS ? 0 : 1;
	unsigned int reg;

	/* Setting Enable register */
	reg = readl(pll->enable_reg);
	if (set)
		reg |= (1 << pll->enable_bit);
	else
		reg &= ~(1 << pll->enable_bit);
	writel(reg, pll->enable_reg);

	/* setting CTRL mux register to 1 */
	reg = readl(pll->sel_reg);
	reg |= (1 << pll->sel_bit);
	writel(reg, pll->sel_reg);

	/* check status for mux setting */
	do {
		cpu_relax();
		reg = readl(pll->stat_reg);
	} while (((reg >> pll->stat_bit) & PLL_STAT_MASK) == PLL_STAT_CHANGE);

	return 0;
}

static void samsung_composite_pll_disable_onchange(struct clk_hw *hw)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	int set = pll->pll_flag & PLL_BYPASS ? 0 : 1;
	unsigned int reg;

	/* setting CTRL mux register to 0 */
	reg = readl(pll->sel_reg);
	reg &= ~(1 << pll->sel_bit);
	writel(reg, pll->sel_reg);

	/* check status for mux setting */
	do {
		cpu_relax();
		reg = readl(pll->stat_reg);
	} while (((reg >> pll->stat_bit) & PLL_STAT_MASK) == PLL_STAT_CHANGE);

	/* Setting Register */
	reg = readl(pll->enable_reg);
	if (set)
		reg &= ~(1 << pll->enable_bit);
	else
		reg |= (1 << pll->enable_bit);

	writel(reg, pll->enable_reg);
}

static unsigned long samsung_pll255xx_recalc_rate(struct clk_hw *hw,
				unsigned long parent_rate)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	u32 mdiv, pdiv, sdiv, pll_con;
	u64 fvco = parent_rate;

	pll_con = readl(pll->con_reg);
	mdiv = (pll_con >> PLL255XX_MDIV_SHIFT) & PLL255XX_MDIV_MASK;
	pdiv = (pll_con >> PLL255XX_PDIV_SHIFT) & PLL255XX_PDIV_MASK;
	sdiv = (pll_con >> PLL255XX_SDIV_SHIFT) & PLL255XX_SDIV_MASK;
	/* Do calculation */
	fvco *= mdiv;
	do_div(fvco, (pdiv << sdiv));

	return (unsigned long)fvco;
}

static inline bool samsung_pll255xx_mp_check(u32 mdiv, u32 pdiv, u32 pll_con)
{
	return ((mdiv != ((pll_con >> PLL255XX_MDIV_SHIFT) & PLL255XX_MDIV_MASK)) ||
		(pdiv != ((pll_con >> PLL255XX_PDIV_SHIFT) & PLL255XX_PDIV_MASK)));
}

static int samsung_pll255xx_set_rate(struct clk_hw *hw, unsigned long drate,
					unsigned long prate)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	const struct samsung_pll_rate_table *rate;
	u32 pll_con;

	rate = samsung_get_pll_settings(pll, drate);
	if (!rate) {
		pr_err("%s: Invalid rate : %lu for pll clk %s\n", __func__,
				drate, __clk_get_name(hw->clk));
		return -EINVAL;
	}

	pll_con = readl(pll->con_reg);
	if (!(samsung_pll255xx_mp_check(rate->mdiv, rate->pdiv, pll_con))) {
		if ((rate->sdiv) == ((pll_con >> PLL255XX_SDIV_SHIFT) & PLL255XX_SDIV_MASK))
			return 0;
		/* In the case of changing S value only */
		pll_con &= ~(PLL255XX_SDIV_MASK << PLL255XX_SDIV_SHIFT);
		pll_con |= rate->sdiv << PLL255XX_SDIV_SHIFT;
		writel(pll_con, pll->con_reg);

		return 0;
	}

	/* Set PLL lock time */
	writel(rate->pdiv * PLL255XX_LOCK_FACTOR, pll->lock_reg);
	/* Change PLL PMS values */
	pll_con &= ~((PLL255XX_MDIV_MASK << PLL255XX_MDIV_SHIFT) |
			(PLL255XX_PDIV_MASK << PLL255XX_PDIV_SHIFT) |
			(PLL255XX_SDIV_MASK << PLL255XX_SDIV_SHIFT));
	pll_con |= (rate->mdiv << PLL255XX_MDIV_SHIFT) |
			(rate->pdiv << PLL255XX_PDIV_SHIFT) |
			(rate->sdiv << PLL255XX_SDIV_SHIFT);

	/* To prevent unstable PLL operation, preset enable bit with 0 */
	pll_con &= ~BIT32(31);
	writel(pll_con, pll->con_reg);

	/* Set enable bit */
	pll_con |= BIT32(31);
	writel(pll_con, pll->con_reg);

	do {
		cpu_relax();
		pll_con = readl(pll->con_reg);
	} while (!(pll_con & (PLL255XX_LOCKED_MASK << PLL255XX_LOCKED_SHIFT)));

	return 0;
}

/* register function for pll clocks */
static const struct clk_ops samsung_pll255xx_clk_ops = {
	.recalc_rate = samsung_pll255xx_recalc_rate,
	.set_rate = samsung_pll255xx_set_rate,
	.round_rate = samsung_pll_round_rate,
	.enable = samsung_composite_pll_enable_onchange,
	.disable = samsung_composite_pll_disable_onchange,
	.is_enabled = samsung_composite_pll_is_enabled,
};

static unsigned long samsung_pll2650x_recalc_rate(struct clk_hw *hw,
				unsigned long parent_rate)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	u32 mdiv, pdiv, sdiv, pll_con0, pll_con1;
	s16 kdiv;
	u64 fvco = parent_rate;

	pll_con0 = readl(pll->con_reg);
	pll_con1 = readl(pll->con_reg + 4);
	mdiv = (pll_con0 >> PLL2650X_MDIV_SHIFT) & PLL2650X_MDIV_MASK;
	pdiv = (pll_con0 >> PLL2650X_PDIV_SHIFT) & PLL2650X_PDIV_MASK;
	sdiv = (pll_con0 >> PLL2650X_SDIV_SHIFT) & PLL2650X_SDIV_MASK;
	kdiv = (s16)((pll_con1 >> PLL2650X_KDIV_SHIFT) & PLL2650X_KDIV_MASK);
	/* Do calculation */
	fvco *= (mdiv << 16) + kdiv;
	do_div(fvco, (pdiv << sdiv));
	fvco >>= 16;

	return (unsigned long)fvco;
}

static inline bool samsung_pll2650x_mpk_check(u32 mdiv, u32 pdiv, u32 kdiv, u32 pll_con0, u32 pll_con1)
{
	return ((mdiv != ((pll_con0 >> PLL2650X_MDIV_SHIFT) & PLL2650X_MDIV_MASK)) ||
		(pdiv != ((pll_con0 >> PLL2650X_PDIV_SHIFT) & PLL2650X_PDIV_MASK)) ||
		(kdiv != ((pll_con1 >> PLL2650X_KDIV_SHIFT) & PLL2650X_KDIV_MASK)));
}

static int samsung_pll2650x_set_rate(struct clk_hw *hw, unsigned long drate,
					unsigned long parent_rate)
{
	struct samsung_composite_pll *pll = to_comp_pll(hw);
	u32 pll_con0, pll_con1;
	const struct samsung_pll_rate_table *rate;

	rate = samsung_get_pll_settings(pll, drate);
	if (!rate) {
		pr_err("%s: Invalid rate : %lu for pll clk %s\n", __func__,
				drate, __clk_get_name(hw->clk));
		return -EINVAL;
	}

	pll_con0 = readl(pll->con_reg);
	pll_con1 = readl(pll->con_reg + 4);
	if (!(samsung_pll2650x_mpk_check(rate->mdiv, rate->pdiv, rate->kdiv, pll_con0, pll_con1))) {
		if ((rate->sdiv) == ((pll_con0 >> PLL2650X_SDIV_SHIFT) & PLL2650X_SDIV_MASK))
			return 0;
		/* In the case of changing S value only */
		pll_con0 &= ~(PLL2650X_SDIV_MASK << PLL2650X_SDIV_SHIFT);
		pll_con0 |= (rate->sdiv << PLL2650X_SDIV_SHIFT);
		writel(pll_con0, pll->con_reg);

		return 0;
	}

	/* Set PLL lock time */
	writel(rate->pdiv * PLL2650X_LOCK_FACTOR, pll->lock_reg);

	pll_con1 &= ~(PLL2650X_KDIV_MASK << PLL2650X_KDIV_SHIFT);
	pll_con1 |= (rate->kdiv << PLL2650X_KDIV_SHIFT);
	writel(pll_con1, pll->con_reg + 4);

	pll_con0 &= ~((PLL2650X_MDIV_MASK << PLL2650X_MDIV_SHIFT) |
			(PLL2650X_PDIV_MASK << PLL2650X_PDIV_SHIFT) |
			(PLL2650X_SDIV_MASK << PLL2650X_SDIV_SHIFT));
	pll_con0 |= (rate->mdiv << PLL2650X_MDIV_SHIFT) |
			(rate->pdiv << PLL2650X_PDIV_SHIFT) |
			(rate->sdiv << PLL2650X_SDIV_SHIFT);

	/* To prevent unstable PLL operation, preset enable bit with 0 */
	pll_con0 &= ~BIT32(31);
	writel(pll_con0, pll->con_reg);

	/* Set enable bit */
	pll_con0 |= BIT32(31);
	writel(pll_con0, pll->con_reg);

	/*
	 * Wait lock time
	 * unit address translation : us to ms for mdelay
	 */
	mdelay(rate->pdiv * PLL2650X_LOCK_FACTOR / 1000);

	return 0;
}

static const struct clk_ops samsung_pll2650x_clk_ops = {
	.recalc_rate = samsung_pll2650x_recalc_rate,
	.set_rate = samsung_pll2650x_set_rate,
	.round_rate = samsung_pll_round_rate,
	.enable = samsung_composite_pll_enable_onchange,
	.disable = samsung_composite_pll_disable_onchange,
	.is_enabled = samsung_composite_pll_is_enabled,
};

/* operation functions for mux clocks */
static u8 samsung_mux_get_parent(struct clk_hw *hw)
{
	struct samsung_composite_mux *mux = to_comp_mux(hw);
	u32 val;

	val = readl(mux->sel_reg) >> mux->sel_bit;
	val &= (BIT32(mux->sel_width) - 1);

	return (u8)val;
}

static int samsung_mux_set_parent(struct clk_hw *hw, u8 index)
{
	struct samsung_composite_mux *mux = to_comp_mux(hw);
	u32 val;
	unsigned long flags = 0;
	unsigned int timeout = 1000;

	if (mux->lock)
		spin_lock_irqsave(mux->lock, flags);

	val = readl(mux->sel_reg);
	val &= ~((BIT32(mux->sel_width) - 1) << mux->sel_bit);
	val |= index << mux->sel_bit;
	writel(val, mux->sel_reg);

	if (mux->stat_reg)
		do {
			--timeout;
			if (!timeout) {
				pr_err("%s: failed to set parent %s.\n",
						__func__, clk_hw_get_name(hw));
				pr_err("MUX_REG: %08x, MUX_STAT_REG: %08x\n",
						readl(mux->sel_reg), readl(mux->stat_reg));
				if (mux->lock)
					spin_unlock_irqrestore(mux->lock, flags);
				return -ETIMEDOUT;
			}
			val = readl(mux->stat_reg);
			val &= ((BIT32(mux->stat_width) - 1) << mux->stat_bit);
		} while (val != (index << mux->stat_bit));

	if (mux->lock)
		spin_unlock_irqrestore(mux->lock, flags);

	return 0;
}

static const struct clk_ops samsung_composite_mux_ops = {
	.get_parent = samsung_mux_get_parent,
	.set_parent = samsung_mux_set_parent,
};

/* operation functions for mux clocks checking status with "on changing" */

static int samsung_mux_set_parent_onchange(struct clk_hw *hw, u8 index)
{
	struct samsung_composite_mux *mux = to_comp_mux(hw);
	u32 val;
	unsigned long flags = 0;
	unsigned int timeout = 1000;

	if (mux->lock)
		spin_lock_irqsave(mux->lock, flags);

	val = readl(mux->sel_reg);
	val &= ~((BIT32(mux->sel_width) - 1) << mux->sel_bit);
	val |= index << mux->sel_bit;
	writel(val, mux->sel_reg);

	if (mux->stat_reg)
		do {
			--timeout;
			if (!timeout) {
				pr_err("%s: failed to set parent %s.\n",
						__func__, clk_hw_get_name(hw));
				pr_err("MUX_REG: %08x, MUX_STAT_REG: %08x\n",
						readl(mux->sel_reg), readl(mux->stat_reg));
				if (mux->lock)
					spin_unlock_irqrestore(mux->lock, flags);
				return -ETIMEDOUT;
			}
			val = readl(mux->stat_reg);
			val &= ((BIT32(mux->stat_width) - 1) << mux->stat_bit);
		} while (val == PLL_STAT_CHANGE);

	if (mux->lock)
		spin_unlock_irqrestore(mux->lock, flags);

	return 0;
}
static const struct clk_ops samsung_composite_mux_ops_onchange = {
	.get_parent = samsung_mux_get_parent,
	.set_parent = samsung_mux_set_parent_onchange,
};

/* operation functions for divider clocks */
static unsigned long samsung_divider_recalc_rate(struct clk_hw *hw,
		unsigned long parent_rate)
{
	struct samsung_composite_divider *divider = to_comp_divider(hw);
	unsigned int val;

	val = readl(divider->rate_reg) >> divider->rate_bit;
	val &= (1 << divider->rate_width) - 1;
	val += 1;
	if (!val)
		return parent_rate;

	return parent_rate / val;
}

static int samsung_divider_bestdiv(struct clk_hw *hw, unsigned long rate,
			unsigned long *best_parent_rate)
{
	struct samsung_composite_divider *divider = to_comp_divider(hw);
	int i, bestdiv = 0;
	unsigned long parent_rate, now, best = 0;
	unsigned long parent_rate_saved = *best_parent_rate;
	int maxdiv;

	if (!rate)
		rate = 1;

	maxdiv = ((1 << (divider->rate_width)) - 1) + 1;

	if (!(clk_hw_get_flags(hw) & CLK_SET_RATE_PARENT)) {
		parent_rate = *best_parent_rate;
		bestdiv = (parent_rate + rate - 1) / rate;
		bestdiv = bestdiv == 0 ? 1 : bestdiv;
		bestdiv = bestdiv > maxdiv ? maxdiv : bestdiv;
		return bestdiv;
	}

	maxdiv = min(UINT_MAX / (unsigned int)rate, (unsigned int)maxdiv);

	for (i = 1; i <= maxdiv; i++) {
		if (rate * i == parent_rate_saved) {
			/*
			 * It's the most ideal case if the requested rate can be
			 * divided from parent clock without needing to change
			 * parent rate, so return the divider immediately.
			 */
			*best_parent_rate = parent_rate_saved;
			return i;
		}
		parent_rate = clk_round_rate(clk_get_parent(hw->clk),
				((rate * i) + i - 1));
		now = parent_rate / i;
		if (now <= rate && now > best) {
			bestdiv = i;
			best = now;
			*best_parent_rate = parent_rate;
		}
	}

	if (!bestdiv) {
		bestdiv = ((1 << (divider->rate_width)) - 1) + 1;
		*best_parent_rate = clk_round_rate(clk_get_parent(hw->clk), 1);
	}

	return bestdiv;
}

static long samsung_divider_round_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long *prate)
{
	int div = 1;

	div = samsung_divider_bestdiv(hw, rate, prate);
	if (div == 0) {
		pr_err("%s: divider value should not be %d\n", __func__, div);
		div = 1;
	}

	return *prate / div;
}

static int samsung_divider_set_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	struct samsung_composite_divider *divider = to_comp_divider(hw);
	unsigned int div;
	u32 val;
	unsigned long flags = 0;
	unsigned int timeout = 1000;

	div = (parent_rate / rate) - 1;

	if (div > ((1 << divider->rate_width) - 1))
		div = (1 << divider->rate_width) - 1;

	if (divider->lock)
		spin_lock_irqsave(divider->lock, flags);

	val = readl(divider->rate_reg);
	val &= ~(((1 << divider->rate_width) - 1) << divider->rate_bit);
	val |= div << divider->rate_bit;
	writel(val, divider->rate_reg);

	if (divider->stat_reg)
		do {
			--timeout;
			if (!timeout) {
				pr_err("%s: faild to set rate %s.\n",
						__func__, clk_hw_get_name(hw));
				pr_err("DIV_REG: %08x, MUX_STAT_REG: %08x\n",
						readl(divider->rate_reg), readl(divider->stat_reg));
				if (divider->lock)
					spin_unlock_irqrestore(divider->lock, flags);
				return -ETIMEDOUT;
			}
			val = readl(divider->stat_reg);
			val &= BIT32(divider->stat_width - 1) << divider->stat_bit;
		} while (val);

	if (divider->lock)
		spin_unlock_irqrestore(divider->lock, flags);

	return 0;
}

static const struct clk_ops samsung_composite_divider_ops = {
	.recalc_rate = samsung_divider_recalc_rate,
	.round_rate = samsung_divider_round_rate,
	.set_rate = samsung_divider_set_rate,
};

/**
 * Operations for virtual clock used in cal
 * When cal is used to set clocks, following operations will be executed.
 */
int cal_vclk_enable(struct clk_hw *hw)
{
	struct samsung_vclk *vclk = to_vclk(hw);
	unsigned long flags = 0;
	int ret = 0;

	if (clk_exynos_skip_hw)
		return 0;

	if ((vclk->flags & VCLK_GATE) && !(vclk->flags & VCLK_QCH_DIS))
		return 0;

	if (vclk->lock)
		spin_lock_irqsave(vclk->lock, flags);

	if ((vclk->flags & VCLK_GATE) && (vclk->flags & VCLK_QCH_DIS))
		cal_qch_init(vclk->id, 0);

	dbg_snapshot_clk(hw, __func__, 1, DSS_FLAG_IN);
	/* Call cal api to enable virtual clock */
	ret = cal_clk_enable(vclk->id);
	if (ret) {
		pr_err("[CAL]%s failed %d %d.\n", __func__, vclk->id, ret);
		dbg_snapshot_clk(hw, __func__, 1, DSS_FLAG_ON);
		if (vclk->lock)
			spin_unlock_irqrestore(vclk->lock, flags);
		return -EAGAIN;
	}
	dbg_snapshot_clk(hw, __func__, 1, DSS_FLAG_OUT);

	if (vclk->lock)
		spin_unlock_irqrestore(vclk->lock, flags);

	return 0;
}

void cal_vclk_disable(struct clk_hw *hw)
{
	struct samsung_vclk *vclk = to_vclk(hw);
	unsigned long flags = 0;
	int ret = 0;

	if (clk_exynos_skip_hw)
		return ;

	if ((vclk->flags & VCLK_GATE) && !(vclk->flags & VCLK_QCH_DIS))
		return ;

	if (vclk->lock)
		spin_lock_irqsave(vclk->lock, flags);

	if ((vclk->flags & VCLK_GATE) && (vclk->flags & VCLK_QCH_DIS))
		cal_qch_init(vclk->id, 0);

	dbg_snapshot_clk(hw, __func__, 0, DSS_FLAG_IN);
	/* Call cal api to disable virtual clock */
	ret = cal_clk_disable(vclk->id);
	if (ret) {
		pr_err("[CAL]%s failed %d %d.\n", __func__, vclk->id, ret);
		dbg_snapshot_clk(hw, __func__, 0, DSS_FLAG_ON);
		if (vclk->lock)
			spin_unlock_irqrestore(vclk->lock, flags);
		return;
	}
	dbg_snapshot_clk(hw, __func__, 0, DSS_FLAG_OUT);

	if (vclk->lock)
		spin_unlock_irqrestore(vclk->lock, flags);
}

int cal_vclk_is_enabled(struct clk_hw *hw)
{
	struct samsung_vclk *vclk = to_vclk(hw);
	int ret = 0;

	if (clk_exynos_skip_hw)
		return 0;
	/*
	 * Call cal api to check whether clock is enabled or not
	 * Spinlock is not needed because only read operation will
	 * be executed
	 */
	ret = cal_clk_is_enabled(vclk->id);

	return ret;
}

unsigned long cal_vclk_recalc_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	struct samsung_vclk *vclk = to_vclk(hw);
	unsigned long ret = 0;

	if (clk_exynos_skip_hw)
		return 0;

	dbg_snapshot_clk(hw, __func__, 0, DSS_FLAG_IN);
	/* Call cal api to recalculate rate */
	ret = cal_clk_getrate(vclk->id);
	dbg_snapshot_clk(hw, __func__, ret, DSS_FLAG_OUT);

	return ret;
}

unsigned long cal_vclk_gate_recalc_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	struct samsung_vclk *vclk;
	struct clk_hw *parent;
	unsigned long ret = 0;

	if (clk_exynos_skip_hw)
		return 0;

	dbg_snapshot_clk(hw, __func__, 0, DSS_FLAG_IN);
	parent = clk_hw_get_parent(hw);
	if (parent) {
		vclk = to_vclk(parent);
		/* call cal api to recalculate rate */
		ret = cal_clk_getrate(vclk->id);
	}
	dbg_snapshot_clk(hw, __func__, ret, DSS_FLAG_OUT);

	return ret;
}

long cal_vclk_round_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long *prate)
{
	/* round_rate ops is not needed when using cal */
	return (long)rate;
}

int cal_vclk_set_rate(struct clk_hw *hw, unsigned long rate, unsigned long prate)
{
	struct samsung_vclk *vclk = to_vclk(hw);
	unsigned long flags = 0;
	int ret = 0;

	if (clk_exynos_skip_hw)
		return 0;

	if (vclk->lock)
		spin_lock_irqsave(vclk->lock, flags);

	dbg_snapshot_clk(hw, __func__, rate, DSS_FLAG_IN);
	/* Call cal api to set rate of clock */
	ret = cal_clk_setrate(vclk->id, rate);
	if (ret) {
		pr_err("[CAL]%s failed %d %lu %d.\n", __func__,
			vclk->id, rate, ret);
		dbg_snapshot_clk(hw, __func__, rate, DSS_FLAG_ON);
		if (vclk->lock)
			spin_unlock_irqrestore(vclk->lock, flags);
		return -EAGAIN;
	}
	dbg_snapshot_clk(hw, __func__, rate, DSS_FLAG_OUT);

	if (vclk->lock)
		spin_unlock_irqrestore(vclk->lock, flags);

	return ret;
}

#if IS_ENABLED(CONFIG_EXYNOS_VCLK_DFS)
unsigned long cal_vclk_dfs_sw_recalc_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	struct samsung_vclk *vclk = to_vclk(hw);
	unsigned long ret = 0;

	if (clk_exynos_skip_hw)
		return 0;

	/* Call cal api to recalculate rate */
	ret = cal_dfs_cached_get_rate(vclk->id);

	return ret;
}
unsigned long cal_vclk_dfs_recalc_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	struct samsung_vclk *vclk = to_vclk(hw);
	unsigned long ret = 0;

	if (clk_exynos_skip_hw)
		return 0;

	/* Call cal api to recalculate rate */
	ret = cal_dfs_get_rate(vclk->id);

	return ret;
}

int cal_vclk_dfs_set_rate(struct clk_hw *hw, unsigned long rate, unsigned long prate)
{
	struct samsung_vclk *vclk = to_vclk(hw);
	unsigned long flags = 0;
	int ret = 0;

	if (clk_exynos_skip_hw)
		return 0;

	if (vclk->lock)
		spin_lock_irqsave(vclk->lock, flags);

	dbg_snapshot_clk(hw, __func__, rate, DSS_FLAG_IN);
	/* Call cal api to set rate of clock */
	ret = cal_dfs_set_rate(vclk->id, rate);
	if (ret) {
		pr_err("[CAL]%s failed %d %lu %d.\n", __func__,
			vclk->id, rate, ret);
		dbg_snapshot_clk(hw, __func__, rate, DSS_FLAG_ON);
		if (vclk->lock)
			spin_unlock_irqrestore(vclk->lock, flags);
		return -EAGAIN;
	}
	dbg_snapshot_clk(hw, __func__, rate, DSS_FLAG_OUT);

	if (vclk->lock)
		spin_unlock_irqrestore(vclk->lock, flags);

	return ret;
}

int cal_vclk_dfs_set_rate_switch(struct clk_hw *hw, unsigned long rate, unsigned long prate)
{
	struct samsung_vclk *vclk = to_vclk(hw);
	unsigned long flags = 0;
	int ret = 0;

	if (clk_exynos_skip_hw)
		return 0;

	if (vclk->lock)
		spin_lock_irqsave(vclk->lock, flags);

	/* Call cal api to set rate of clock */
	ret = cal_dfs_set_rate_switch(vclk->id, rate);
	if (ret) {
		pr_err("[CAL]%s failed.\n", __func__);
		if (vclk->lock)
			spin_unlock_irqrestore(vclk->lock, flags);
		return -EAGAIN;
	}

	if (vclk->lock)
		spin_unlock_irqrestore(vclk->lock, flags);

	return ret;
}
#endif

static int cal_vclk_qch_init(struct clk_hw *hw)
{
	struct samsung_vclk *vclk = to_vclk(hw);
	int ret = 0;

	if (clk_exynos_skip_hw)
		return 0;

	if (vclk->flags & VCLK_QCH_EN)
		ret = cal_qch_init(vclk->id, 1);
	else if (vclk->flags & VCLK_QCH_DIS)
		ret = cal_qch_init(vclk->id, 0);

	return ret;
}

#if IS_ENABLED(CONFIG_EXYNOS_VCLK_QACTIVE)
int cal_vclk_qactive_enable(struct clk_hw *hw)
{
	struct samsung_vclk *vclk = to_vclk(hw);
	unsigned long flags = 0;
	unsigned int reg;

	if (clk_exynos_skip_hw)
		return 0;

	if (!(vclk->flags & VCLK_QACTIVE))
		return 0;

	if (!vclk->addr)
		return 0;

	if (vclk->lock)
		spin_lock_irqsave(vclk->lock, flags);

	dbg_snapshot_clk(hw, __func__, 1, DSS_FLAG_IN);

	reg = readl(vclk->addr);
	reg &= ~(vclk->mask);
	reg |= vclk->val;
	writel(reg, vclk->addr);

	dbg_snapshot_clk(hw, __func__, 1, DSS_FLAG_OUT);

	if (vclk->lock)
		spin_unlock_irqrestore(vclk->lock, flags);

	return 0;
}

void cal_vclk_qactive_disable(struct clk_hw *hw)
{
	struct samsung_vclk *vclk = to_vclk(hw);
	unsigned long flags = 0;
	unsigned int reg;

	if (clk_exynos_skip_hw)
		return ;

	if (!(vclk->flags & VCLK_QACTIVE))
		return ;

	if (!vclk->addr)
		return ;;

	if (vclk->lock)
		spin_lock_irqsave(vclk->lock, flags);

	dbg_snapshot_clk(hw, __func__, 0, DSS_FLAG_IN);

	reg = readl(vclk->addr);
	reg &= ~(vclk->mask);
	writel(reg, vclk->addr);

	dbg_snapshot_clk(hw, __func__, 0, DSS_FLAG_OUT);

	if (vclk->lock)
		spin_unlock_irqrestore(vclk->lock, flags);
}
#endif

static const struct clk_ops samsung_vclk_ops = {
	.enable = cal_vclk_enable,
	.disable = cal_vclk_disable,
	.is_enabled = cal_vclk_is_enabled,
	.recalc_rate = cal_vclk_recalc_rate,
	.round_rate = cal_vclk_round_rate,
	.set_rate = cal_vclk_set_rate,
};

#if IS_ENABLED(CONFIG_EXYNOS_VCLK_DFS)
static const struct clk_ops samsung_vclk_dfs_ops = {
	.recalc_rate = cal_vclk_dfs_recalc_rate,
	.round_rate = cal_vclk_round_rate,
	.set_rate = cal_vclk_dfs_set_rate,
};

static const struct clk_ops samsung_vclk_dfs_sw_ops = {
	.recalc_rate = cal_vclk_dfs_sw_recalc_rate,
	.round_rate = cal_vclk_round_rate,
	.set_rate = cal_vclk_dfs_set_rate_switch,
};
#endif

static const struct clk_ops samsung_vclk_gate_ops = {
	.enable = cal_vclk_enable,
	.disable = cal_vclk_disable,
	.is_enabled = cal_vclk_is_enabled,
	.recalc_rate = cal_vclk_gate_recalc_rate,
};

#if IS_ENABLED(CONFIG_EXYNOS_VCLK_QACTIVE)
static const struct clk_ops samsung_vclk_qactive_ops = {
	.enable = cal_vclk_qactive_enable,
	.disable = cal_vclk_qactive_disable,
};
#endif

static struct clk *  _samsung_register_vclk(struct init_vclk *list)
{
	struct samsung_vclk *vclk;
	struct clk *clk;
	struct clk_init_data init;

	vclk = kzalloc(sizeof(struct samsung_vclk), GFP_KERNEL);
	if (!vclk) {
		pr_err("%s: could not allocate struct vclk\n", __func__);
		return ERR_PTR(-ENOMEM);
	}

	init.name = list->name;

	if (list->vclk_flags & VCLK_GATE)
		init.ops = &samsung_vclk_gate_ops;
#if IS_ENABLED(CONFIG_EXYNOS_VCLK_DFS)
	else if (list->vclk_flags & VCLK_DFS)
		init.ops = &samsung_vclk_dfs_ops;
	else if (list->vclk_flags & VCLK_DFS_SWITCH)
		init.ops = &samsung_vclk_dfs_sw_ops;
#endif
#if IS_ENABLED(CONFIG_EXYNOS_VCLK_QACTIVE)
	else if (list->vclk_flags & VCLK_QACTIVE) {
		init.ops = &samsung_vclk_qactive_ops;
		vclk->addr = ioremap(list->addr, 4);
		vclk->mask = list->mask;
		vclk->val = list->val;
	}
#endif
	else
		init.ops = &samsung_vclk_ops;
	init.flags = list->flags | (CLK_GET_RATE_NOCACHE | CLK_IGNORE_UNUSED);
	init.parent_names = (list->parent ? &list->parent : NULL);
	init.num_parents = (list->parent ? 1 : 0);
	vclk->id = list->calid;
	/* Flags for vclk are not defined yet */
	vclk->flags = list->vclk_flags;
	vclk->lock = &lock;
	vclk->hw.init = &init;
	clk = clk_register(NULL, &vclk->hw);

	if (IS_ERR(clk))
		kfree(vclk);

	return clk;
}

void  samsung_register_vclk(struct samsung_clk_provider *ctx,
			struct init_vclk *list, unsigned int nr_vclk)
{
	struct clk *clk;
	int cnt;
	unsigned int ret = 0;

	for (cnt = 0; cnt < nr_vclk; cnt++) {
		clk = _samsung_register_vclk(&list[cnt]);
		if (IS_ERR(clk)) {
			pr_err("%s: failed to register virtual clock %s\n",
				__func__, (&list[cnt])->name);
			continue;
		}

		samsung_clk_add_lookup(ctx, clk, (&list[cnt])->id);

		/* Additional array of clocks for finding struct clk */
		if ((&list[cnt])->alias) {
			ret = clk_register_clkdev(clk, (&list[cnt])->alias, NULL);
			if (ret)
				pr_err("%s: failed to register lookup %s\n",
						__func__, (&list[cnt])->alias);
		}

		cal_vclk_qch_init(__clk_get_hw(clk));
	}
}

MODULE_LICENSE("GPL");
