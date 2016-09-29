/*
 * RK3399 pmu-m0 generic head file.
 *
 * Copyright (C) 2016 Fuzhou Rockchip Electronics Co., Ltd
 *
 * Frank Wang <frank.wang@rock-chips.com>
 */

#ifndef _RK3399_H
#define _RK3399_H

#define MCU_BASE			0x40000000
#define PWM_BASE			(MCU_BASE + 0x07420000)

#define UART_DBG_BASE			(MCU_BASE + 0x071a0000)
#define PMU_BASE			(MCU_BASE + 0x07310000)
#define PMU_GRF_BASE			(MCU_BASE + 0x07320000)

#define CRU_BASE			(MCU_BASE + 0x07760000)
#define CRU_GLB_SRST_FST		0x0500
#define GLB_SRST_FST_CFG_VAL		0xfdb9

#define MCUJTAG_IOMUX_ADDR		(PMU_GRF_BASE + 0x00014)
#define MCUJTAG_IOMUX			(0xf << (2 + 16) | 0x5 << 2)

#include "mcu.h"

#define M0_INT_ARB_MASKN(n)		(MCU_BASE + 0x0779c000 + (n) * 4)
#define M0_INT_ARB_FLAGN(n)		(MCU_BASE + 0x0779c080 + (n) * 4)

/* set the mask of m0 interrupt arbiter */
static inline void M0_INT_ARB_SET_MASK(int IRQn)
{
	unsigned int val = readl(M0_INT_ARB_MASKN(IRQn/8));
	val |= 1 << (IRQn % 8);
	writel(val, M0_INT_ARB_MASKN(IRQn/8));
}

/* get the flag of m0 interrupt arbiter */
static inline unsigned int M0_INT_ARB_GET_FLAG(int IRQ)
{
	unsigned int val = (readl(M0_INT_ARB_FLAGN(IRQ))) & 0xff;

	return val;
}

void system_reset(void);
#endif
