/*
 * operations related mcu arch.
 *
 * Copyright (C) 2016 Fuzhou Rockchip Electronics Co., Ltd
 *
 * Frank Wang <frank.wang@rock-chips.com>
 */

#ifndef _MCU_H
#define _MCU_H

#ifndef NULL
#define NULL				((void *)0)
#endif

#define SCS_BASE			(0xE000E000UL)
#define NVIC_BASE			(SCS_BASE +  0x0100UL)
#define NVIC				((NVIC_TYPE *) NVIC_BASE)
#define NVIC_ST_CTRL    (*((volatile unsigned int *)0xE000E010))
#define NVIC_ST_RELOAD  (*((volatile unsigned int *)0xE000E014))
#define NVIC_ST_CURRENT (*((volatile unsigned int *)0xE000E018))

/**
 * NVIC structure
 * @ISER: Offset: 0x000 (R/W)  Interrupt Set Enable Register
 * @ICER: Offset: 0x080 (R/W)  Interrupt Clear Enable Register
 * @ISPR: Offset: 0x100 (R/W)  Interrupt Set Pending Register
 * @ICPR: Offset: 0x180 (R/W)  Interrupt Clear Pending Register
 * @IP: Offset: 0x300 (R/W)  Interrupt Priority Register
 */
typedef struct
{
	volatile unsigned int ISER[1];
	unsigned int RESERVED0[31];
	volatile unsigned int ICER[1];
	unsigned int RSERVED1[31];
	volatile unsigned int ISPR[1];
	unsigned int RESERVED2[31];
	volatile unsigned int ICPR[1];
	unsigned int RESERVED3[31];
	unsigned int RESERVED4[64];
	volatile unsigned int IP[8];
}  NVIC_TYPE;

static inline void NVIC_EnableIRQ(int IRQn)
{
	NVIC->ISER[0] = (1 << ((unsigned int)(IRQn) & 0x1F));
}

static inline void NVIC_DisableIRQ(int IRQn)
{
	NVIC->ICER[0] = (1 << ((unsigned int)(IRQn) & 0x1F));
}

static inline void NVIC_ClearPendingIRQ(int IRQn)
{
  NVIC->ICPR[0] = (1 << ((unsigned int)(IRQn) & 0x1F));
}

#define readl(c)	({unsigned int __v = (*(unsigned int *) (c)); __v;})
#define writel(v, c)	((*(unsigned int *) (c)) = (v))

void mdelay(unsigned int);
int main(void);

#endif
