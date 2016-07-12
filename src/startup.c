/*
 * RK3399 pmu-m0 startup code.
 *
 * Copyright (C) 2016 Fuzhou Rockchip Electronics Co., Ltd
 *
 * Frank Wang <frank.wang@rock-chips.com>
 */

#include "rk3399.h"

/* Stack configuration */
#define STACK_SIZE	0x00000100
__attribute__ ((section(".co_stack")))
unsigned long pstack[STACK_SIZE];

/* Macro definition */
#define WEAK __attribute__ ((weak))

/* System exception vector handler */
__attribute__ ((used))
void WEAK  reset_handler(void);
void WEAK  nmi_handler(void);
void WEAK  hardware_fault_handler(void);
void WEAK  svc_handler(void);
void WEAK  pend_sv_handler(void);
void WEAK  systick_handler(void);

/* Symbols defined in linker script */
/* start address for the initialization values of the .data section */
extern unsigned long _sidata;
/* start address for the .data section */
extern unsigned long _sdata;
/* end address for the .data section */
extern unsigned long _edata;
/* start address for the .bss section */
extern unsigned long _sbss;
/* end address for the .bss section */
extern unsigned long _ebss;
/* end address for ram */
extern void _eram;

/* Function prototypes */
static void default_reset_handler(void);
static void default_handler(void);

/**
  * The minimal vector table for a Cortex M3.  Note that the proper constructs
  * must be placed on this to ensure that it ends up at physical address
  * 0x00000000.
  */
__attribute__ ((used,section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{	
	/* core Exceptions */
	(void *)&pstack[STACK_SIZE], /* the initial stack pointer */
	reset_handler,
	nmi_handler,
	hardware_fault_handler,
	0,0,0,0,0,0,0,
	svc_handler,
	0,0,
	pend_sv_handler,
	systick_handler,

	/* external exceptions */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/**
  * This is the code that gets called when the processor first
  * starts execution following a reset event. Only the absolutely
  * necessary set is performed, after which the application
  * supplied main() routine is called.
  */
static void default_reset_handler(void)
{
	/* initialize data and bss */
	unsigned long *psrc, *pdst;

	/* copy the data segment initializers from flash to SRAM */
	psrc = &_sidata;
	for(pdst = &_sdata; pdst < &_edata; )
		*(pdst++) = *(psrc++);
  
	/*
	 * zero fill the bss segment.  This is done with inline assembly
	 * since this will clear the value of pdst if it is not kept
	 * in a register.
	 */
	__asm("  ldr	r0, =_sbss  \n"
	      "  ldr	r1, =_ebss  \n"
	      "  mov	r2, #0      \n"
	      "  b	zero_loop   \n"
	      "  .thumb_func        \n"
	      "fill_zero:           \n"
	      "  str	r2, [r0]    \n"
	      "  add	r0, r0, #4  \n"
	      "  .thumb_func        \n"
	      "zero_loop:           \n"
	      "  cmp	r0, r1      \n"
	      "  bcc	fill_zero  ");

	/* set mcujtag iomux before enter main */
	writel(MCUJTAG_IOMUX, (unsigned int *)MCUJTAG_IOMUX_ADDR);

	/* call the application's entry point */
	main();
}

/**
  * Provide weak aliases for each Exception handler to the Default_Handler.
  * As they are weak aliases, any function with the same name will override
  * this definition.
  */
#pragma weak reset_handler = default_reset_handler
#pragma weak nmi_handler = default_handler
#pragma weak hardware_fault_handler = default_handler
#pragma weak svc_handler = default_handler
#pragma weak pend_sv_handler = default_handler
#pragma weak systick_handler = default_handler

/**
  * This is the code that gets called when the processor receives
  * an unexpected interrupt.  This simply enters an infinite loop, 
  * preserving the system state for examination by a debugger.
  */
static void default_handler(void) 
{
    /* go into an infinite loop. */
    while (1) ;
}
