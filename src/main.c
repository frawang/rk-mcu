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

int main(void)
{
	while (1) ;

	return 0;
}
