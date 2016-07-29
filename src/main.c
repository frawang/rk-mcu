/*
 * RK3399 pmu-m0 applications' main code.
 *
 * Copyright (C) 2016 Fuzhou Rockchip Electronics Co., Ltd
 *
 * Frank Wang <frank.wang@rock-chips.com>
 */

#include "rk3399.h"

#define NVIC_ST_CTRL	(*((volatile unsigned int *)0xE000E010))
#define NVIC_ST_RELOAD	(*((volatile unsigned int *)0xE000E014))
#define NVIC_ST_CURRENT	(*((volatile unsigned int *)0xE000E018))

void mdelay(unsigned int nms)
{
	unsigned int tmp;
	unsigned int reload = 1 * (CPU_FREQ) * 1000; /* 1ms */

	while (nms--) {
		NVIC_ST_RELOAD = reload;
		
		NVIC_ST_CURRENT = 0x00;
		NVIC_ST_CTRL = 0x05;

		do {
			tmp = NVIC_ST_CTRL;
		} while (tmp & 0x01 && !(tmp & (1<<16)));
	}

	NVIC_ST_CURRENT = 0x00;
	NVIC_ST_CTRL = 0x00;
}

void system_reset(void)
{
	unsigned int val = readl(PMU_BASE + 0x0024);

	/* set pmic sleep iomux */
	writel(0x0c000a00, (PMU_GRF_BASE + 0x0010));
	mdelay(2000);

	val |= 1 << 7; /* set bit7=1 */
	writel(val, (PMU_BASE + 0x0024));
	mdelay(2000);

	/* system reset */
	writel(GLB_SRST_FST_CFG_VAL, (CRU_BASE + CRU_GLB_SRST_FST));
}

int main(void)
{
	while (1) ;

	return 0;
}
