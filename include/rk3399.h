/*
 * RK3399 pmu-m0 generic head file.
 *
 * Copyright (C) 2016 Fuzhou Rockchip Electronics Co., Ltd
 *
 * Frank Wang <frank.wang@rock-chips.com>
 */

#define MCU_BASE			0x40000000

#define UART_DBG_BASE			(MCU_BASE + 0x071a0000)
#define PMU_GRF_BASE			(MCU_BASE + 0x07320000)

#define CRU_BASE			(MCU_BASE + 0x07760000)
#define CRU_GLB_SRST_FST		0x0500
#define GLB_SRST_FST_CFG_VAL		0xfdb9

#define MCUJTAG_IOMUX_ADDR		(PMU_GRF_BASE + 0x00014)
#define MCUJTAG_IOMUX			(0xf << (2 + 16) | 0x5 << 2)

#define readl(c)	({unsigned int __v = (*(unsigned int *) (c)); __v;})
#define writel(v, c)	((*(unsigned int *) (c)) = (v))

static inline void system_reset(void)
{
	writel(GLB_SRST_FST_CFG_VAL, (CRU_BASE + CRU_GLB_SRST_FST));
}

int main(void);
