/*
 * Copyright (c) 2007, Per Lindgren, Johan Eriksson, Johan Nordlander,
 * Simon Aittamaa.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Luleå University of Technology nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * \brief ARM7 Environment Implementation.
 *
 * This is the ARM7 Environment Implementation (at91sam7x256 to be exact).
 * Needs to be fixed to support a more wide varity of ARM7 microcontrollers
 * since most of the task switching (99.9999%) is ARM7 compatible.
 */

#include <arm7/env.h>
#include <arm7/types.h>
#include <arm7/sys_call.h>

#include <kernel.h>

#include <string.h>

/* ************************************************************************** */

/**
 * \cond
 *
 * Any internal crap goes here.
 */

/* ************************************************************************** */

/*
 * The baudrate we want.
 */
#define ARM7_BAUD_RATE 115200

/* ************************************************************************** */

/*
 * The maximum count of the timer.
 */
#define ARM7_TIMER_COUNT	0x10000

/* ************************************************************************** */

/*
 * The starting time of the timer, for testing.
 */
#define ARM7_TIMER_START 0

/* ************************************************************************** */

/*
 * Interrupt flags for TC_I[EDM]R.
 */
enum
{
	TC_COVFS = 1<<0,
	TC_LOVRS = 1<<1,
	TC_CPAS  = 1<<2,
	TC_CPBS  = 1<<3,
	TC_CPCS  = 1<<4,
	TC_LDRAS = 1<<5,
	TC_LDRBS = 1<<6,
	TC_ETRGS = 1<<7
};

/* ************************************************************************** */

/*
 * The stack for the idle thread and all the other threads.
 */
unsigned char __attribute__((aligned(4))) arm7_stack[ARM7_STACKSIZE];

/* ************************************************************************** */

/*
 * This is an export so that crt.s knows where to place the user/system mode
 * stack.
 */
const unsigned int * const arm7_stack_start =
	(const unsigned int * const)&arm7_stack[ARM7_STACKSIZE];

/* ************************************************************************** */

const unsigned int arm7_context_cookie = ARM7_CONTEXT_COOKIE;

/* ************************************************************************** */

/*
 * Used later in _arm7_aic_vec_fast to be able to load the fvr
 * slightly faster.
 */
void *_arm7_aic_fvr = (void *)AT91C_AIC_FVR;

/* ************************************************************************** */

void *_arm7_aic_ivr = (void *)AT91C_AIC_IVR;

/* ************************************************************************** */

void *_arm7_aic_eoicr = (void *)AT91C_AIC_EOICR;

/* ************************************************************************** */

/*
 * The current "time".
 */
env_time_t arm7_timer_base = ARM7_TIMER_START;

/* ************************************************************************** */

/*
 * The "time" when the next baseline expires.
 */
env_time_t arm7_timer_next = ARM7_TIMER_START;

/* ************************************************************************** */

/*
 * The timestamp of the previous interrupt.
 */
env_time_t arm7_timer_timestamp = ARM7_TIMER_START;

/* ************************************************************************** */

/*
 * Flag that indicates if the timer is active or not.
 */
unsigned int arm7_timer_active = 0;

/* ************************************************************************** */

/*
 * The default irq routine, does nothing.
 */
static void arm7_default_irq(void)
{
	arm7_panic("Interrupt occured without a handler installed.\n");
}

/* ************************************************************************** */

/*
 * Spurious interrupt handler, just so we know it happens...
 *
 * NOTE:
 * 	This iunterrupt as it is now will seriously fuck up things if we ever
 * 	return from a fast interrrupt with this handler. This should not happen
 * 	as we panic in the function but you should at least knoew this...
 */
static void arm7_spurious(void)
{
	arm7_panic("Spurious interrupt triggered.\n");
}

/* ************************************************************************** */

static void sw_irq(void)
{
	/* Read the status. */
	((volatile AT91S_PIO *)AT91C_BASE_PIOB)->PIO_ISR;

	/* Print something so we know what happend... */
	arm7_print("sw_irq()\n");
}

/* ************************************************************************** */

/*
 * Schedule any baseline expiration during the current epoch.
 */
static inline void arm7_timer_epoch_schedule(void)
{
	signed int diff;
	volatile AT91S_TC *tc0 = AT91C_BASE_TC0;

	/* Make sure the timer is active. */
	if (!arm7_timer_active)
		return;

	/* Schedule the next interrupt if a baseline expires during this epoch. */
	tc0->TC_IDR = TC_CPAS;
	diff = (signed int)arm7_timer_next - (signed int)arm7_timer_base;
	if (diff <= ARM7_TIMER_COUNT)
	{
		/* 
		 * The next baseline has expired or will expired during this
		 * epoch.
		 */
		if (tc0->TC_CV > diff)
			tc0->TC_RA = tc0->TC_CV + 1;
		else
			tc0->TC_RA = diff;

		/* Enable the interrupt again. */
		tc0->TC_IER = TC_CPAS;

		/* Make sure we don't miss an interrupt. */
		while (tc0->TC_RA <= tc0->TC_CV && !(tc0->TC_SR & TC_CPAS))
			tc0->TC_RA = tc0->TC_CV + 1;
	}
}

/* ************************************************************************** */

/*
 * Interrupt handler for tc0.
 */
ENV_EXT_FORCE_RAM static void arm7_tc0_interrupt(void)
{
	unsigned int sr;
	unsigned int imr;
	volatile AT91S_TC *tc0 = AT91C_BASE_TC0;

	/* Grab the status register and in effect clear the interrupt status. */
	sr = tc0->TC_SR;
	imr = tc0->TC_IMR;

	/* Check for compare interrupt first. */
	if (sr & TC_CPAS)
	{
		tt_expired(arm7_timer_base+tc0->TC_RA);
		tt_schedule();
	}
	
	/* Then for overflow interrupt. */
	if (sr & TC_COVFS)
	{
		arm7_timer_base += ARM7_TIMER_COUNT;
		arm7_timer_epoch_schedule();
	}

#if 0
	/* Check for overflow. */
	if (sr & TC_COVFS)
	{
		arm7_timer_base += ARM7_TIMER_COUNT;
		
		/* 
		 * Before we perform any scheduling we will check for RA interrupt
		 * so that we only need to call the epoch schedule once.
		 */
		if ((imr & TC_CPAS) && (sr & TC_CPAS))
			tt_expired(arm7_timer_next);
		else
			arm7_timer_epoch_schedule();
		tt_schedule();
	}
	else if (!(imr & TC_CPAS) && (sr & TC_CPAS))
	{
		/* Tell tinyTimber what baseline expired. */
		tt_expired(arm7_timer_next);
		tt_schedule();
	}
#endif
}

/* ************************************************************************** */

/*
 * Used in the ENV_CONTEXT_(SAVE|RESTORE) macros. Must not be static or
 * the inline asm will not see it :/
 * Makes life easier(tm).
 */
void arm7_context_panic(void)
{
	arm7_panic("Context cookie corrupted.\n");
}

/* ************************************************************************** */

/**
 * End of internal crap is _HERE_.
 *
 * \endcond
 */

/* ************************************************************************** */

/**
 * \brief ARM7 init function.
 *
 * Should land the processor in a fully usable state.
 */
void arm7_init(void)
{
	int i;
	volatile AT91S_PMC *pmc = AT91C_BASE_PMC;
	volatile AT91S_AIC *aic = AT91C_BASE_AIC;
	volatile AT91S_PIO *piob = AT91C_BASE_PIOB;
	volatile AT91S_TC  *tc0 = AT91C_BASE_TC0;

	/* Disable the watchdog timer. */
	AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;

	/* We are aiming for 48Mhz operation so we need one wait-state. */
	AT91C_BASE_MC->MC_FMR = AT91C_MC_FWS_1FWS;

	/*
	 * Startup the main clock, we need to wait for it to stabilize. The
	 * OSCOUNT is calculated for the worst case of 3MHz so this can probably
	 * be done faster if speed was a concern.
	 */
	pmc->PMC_MOR = (AT91C_CKGR_OSCOUNT & (0x40<<8))|AT91C_CKGR_MOSCEN;
	while (!(pmc->PMC_SR & AT91C_PMC_MOSCS));

	/*
	 * Fire upt the PLL to ~96Mhz and set the UDP clock to ~48Mhz, not
	 * used but in case we ever need it. This could probably be done faster
	 * if speed was a concern. The PLLCOUNT is just set to the maximum value
	 * so that we will never fail(hopefully).
	 */
	pmc->PMC_PLLR =
		AT91C_CKGR_USBDIV_1|AT91C_CKGR_OUT_0|AT91C_CKGR_PLLCOUNT|
		(AT91C_CKGR_MUL & ARM7_PLL_MUL << 16)|(AT91C_CKGR_DIV & ARM7_PLL_DIV);
	while (!(pmc->PMC_SR & AT91C_PMC_LOCK));
	while (!(pmc->PMC_SR & AT91C_PMC_MCKRDY));

	/* Set the Master Clock to PLL/2 or 48MHz. */
	pmc->PMC_MCKR = AT91C_PMC_PRES_CLK_2;
	while (!(pmc->PMC_SR & AT91C_PMC_MCKRDY));
	pmc->PMC_MCKR |= AT91C_PMC_CSS_PLL_CLK;
	while (!(pmc->PMC_SR & AT91C_PMC_MCKRDY));

	/* 
	 * The CPU should now be running at ARM7_MASTER_CLOCK and we can now setup
	 * any other peripherials that we might need, such as serial communication,
	 * AIC and timer(s).
	 */

	/* Setup the AIC vectors and SPU. */
	for (i=0;i<32;i++)
		aic->AIC_SVR[i] = (unsigned int)arm7_default_irq;
	aic->AIC_SPU = (unsigned int)arm7_spurious;

	/* Configure the DBGU for 115200 serial communication. */
	AT91F_DBGU_CfgPIO();
	AT91F_US_Configure(
			(AT91S_USART *)AT91C_BASE_DBGU,
			ARM7_MASTER_CLOCK,
			AT91C_US_ASYNC_MODE,
			ARM7_BAUD_RATE,
			0
			);
	AT91F_US_EnableTx((AT91S_USART *)AT91C_BASE_DBGU);

	/* Some testing, should check for interrupts on sw1. */

	/* Enable the clock for PIOB. */
	pmc->PMC_PCER = 1<<AT91C_ID_PIOB;

	/* 
	 * Pin 24 on PIOB is sw1, set it up with interrupt on change and
	 * input filtering.
	 */
	piob->PIO_PER   = AT91C_PIO_PB24;
	piob->PIO_IFER  = AT91C_PIO_PB24;
	piob->PIO_CODR  = AT91C_PIO_PB24;
	piob->PIO_IER   = AT91C_PIO_PB24;

	/* 
	 * This is only to remove the first annoying interrupt we get for
	 * a "change" that does not occur.
	 */
	piob->PIO_ISR;

	/* 
	 * Install an interrupt handler for the above mentioned switch, it should
	 * have the priority 0(lowest).
	 */
	aic->AIC_SVR[AT91C_ID_PIOB] = (unsigned int)sw_irq;
	aic->AIC_SMR[AT91C_ID_PIOB] = AT91C_AIC_SRCTYPE_HIGH_LEVEL|0;
	aic->AIC_IECR = (1<<AT91C_ID_PIOB);

	/* Make sure the clock is clocked. */
	pmc->PMC_PCER = (1<<AT91C_ID_TC0);

	/* 
	 * Start with resetting the clock since we might perform soft
	 * resets every now and then.
	 */
	tc0->TC_IDR = 0xf;
	tc0->TC_CCR = AT91C_TC_CLKDIS;
	tc0->TC_CV  = 0;
	tc0->TC_SR;

	/* TC0 should be running at MCK/1024 counting up(WAVE-mode). */
	tc0->TC_CMR =
		AT91C_TC_CLKS_TIMER_DIV5_CLOCK|
		AT91C_TC_WAVE|AT91C_TC_WAVESEL_UP;

	/* 
	 * Setup the interrupt for overflow only, since we always want
	 * the overflow interrupts. The interrupt generated by RA is
	 * only when a baseline expires during the current epoch.
	 */
	tc0->TC_IER = TC_COVFS;
	aic->AIC_SVR[AT91C_ID_TC0] = (unsigned int)arm7_tc0_interrupt;
	aic->AIC_SMR[AT91C_ID_TC0] = AT91C_AIC_SRCTYPE_HIGH_LEVEL;
	aic->AIC_IECR = (1<<AT91C_ID_TC0);

	/* Enable the clock. */
	tc0->TC_CCR = AT91C_TC_CLKEN;

	/* Setup the reset button to work properly. */
	AT91C_BASE_RSTC->RSTC_RMR = 0xa5000401;
}

/* ************************************************************************** */

/**
 * \brief ARM7 print function.
 *
 * \param msg The message to print.
 */
void arm7_print(const char *msg)
{
	while (*msg)
	{
		while (!AT91F_US_TxReady((AT91S_USART *)AT91C_BASE_DBGU));
		AT91F_US_PutChar((AT91S_USART *)AT91C_BASE_DBGU, *msg++);
	}
	while (!AT91F_US_TxReady((AT91S_USART *)AT91C_BASE_DBGU));
}

/* ************************************************************************** */

/**
 * \brief ARM7 panic function.
 *
 * \param msg The message to print.
 */
void arm7_panic(const char *msg)
{
	arm7_protect(1);
	arm7_print(msg);

	/* Stop _ALL_ system clocks. */
#if 0
	AT91C_BASE_PMC->PMC_SCDR =
		AT91C_PMC_PCK |
		AT91C_PMC_PCK0 |
		AT91C_PMC_PCK1 |
		AT91C_PMC_PCK2 |
		AT91C_PMC_UDP;
#else
	for (;;);
#endif
}

/* ************************************************************************** */

/**
 * \brief ARM7 idle function.
 *
 * Initializes the tt_current(idle thread) context and enters an
 * idle state if nescessary.
 */
void arm7_idle(void)
{
	unsigned int sp;

	/* Make sure we haven't overrun the stack. */
	asm volatile("mov	%0, sp\n" : "=r" (sp));
	if (sp <= (unsigned int)&arm7_stack[ARM7_STACKSIZE-ENV_STACKSIZE_IDLE])
		arm7_panic("arm7_idle(): Stack overflow.\n");

	/* Reset the sp to the top of the stack, we don't ever plan to return.  */
	asm volatile("mov	%0, sp\n" :: "r" (&arm7_stack[ARM7_STACKSIZE]));

	/* Setup the current context. */
	tt_current->cookie = (void *)&arm7_stack[ARM7_STACKSIZE-ENV_STACKSIZE_IDLE];
	*tt_current->cookie = ARM7_CONTEXT_COOKIE;

	/* Leave protected mode. */
	arm7_protect(0);

	/* Idle by disableing the processor clock. */
	for (;;);
		/* This line right nere will cause data-abort, WTF?. */
		/*AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;*/
}

/* ************************************************************************** */

/**
 * \brief ARM7 context init function.
 *
 * \param context The context that should be initialized.
 * \param stacksize The size of the stack that should be used.
 * \param function The function that should be called.
 */
void arm7_context_init(arm7_context_t *context, size_t stacksize, tt_thread_function_t function)
{
	int i;
	static unsigned int arm7_stack_offset = ARM7_STACKSIZE-ENV_STACKSIZE_IDLE;

	/* Check for alignment. */
	if (stacksize & 0x3)
		arm7_panic("arm7_context_init(): Missalligned stacksize requested.\n");
	
	/* Make sure we have enough stack and assign some to the context. */
	if (arm7_stack_offset < stacksize)
		arm7_panic("arm7_context_init(): Out of memory.\n");
	context->sp = (unsigned int *)&arm7_stack[arm7_stack_offset];
	context->cookie = (unsigned int*)&arm7_stack[arm7_stack_offset-stacksize];
	*context->cookie = ARM7_CONTEXT_COOKIE;

	i = (int)context->sp;

	/* Push the return address onto the stack. */
	*(--context->sp) = (unsigned int)function;

	/* Push a bogus link register onto the stack. */
	*(--context->sp) = (unsigned int)0x00000000;

	/* Push the stack pointer onto the stack. */
	*(--context->sp) = (unsigned int)i;

	/* Push some fake registers onto the stack. */
	for (i=13;i>0;i--)
	{
		*(--context->sp) = i-1;
	}

	/* Push the SPSR onto the stack. */
	*(--context->sp) = 0xdf; /* System mode, all interrupts disabled. */

	/* Save the offset for the next stack. */
	arm7_stack_offset -= stacksize;
}

/* ************************************************************************** */

/**
 * \brief ARM7 Context dispatch system call implementation.
 */
ENV_EXT_FORCE_RAM void _arm7_context_dispatch(arm7_context_t *new)
{
	tt_current = new;
}

/* ************************************************************************** */

/**
 * \brief ARM7 Context dispatch system call binding.
 */
ARM7_SYS_CALL_BIND(
	void arm7_context_dispatch(env_context_t *thread),
	ARM7_SYS_CALL_DISPATCH
	);

/* ************************************************************************** */

/**
 * \brief ARM7 protect function.
 *
 * Used only when protect is called from thumb mode.
 *
 * \param state non-zero to enter protected mode, zero to leave protected mode.
 */
void _arm7_protect(int state)
{
	arm7_protect(state);
}

/* ************************************************************************** */

/**
 * \brief ARM7 isprotected function.
 *
 * Used only when protect is called from thumb mode.
 *
 * \return non-zero if protected, nonzero otherwise.
 */
int _arm7_isprotected(void)
{
	return arm7_isprotected();
}

/* ************************************************************************** */

/**
 * \brief ARM7 timer set function.
 *
 * Sets the "time" when the next baseline expires.
 *
 * \param next The "time" when the next baseline expires.
 */
void arm7_timer_set(env_time_t next)
{
	arm7_timer_next = next;
	arm7_timer_active = 1;
	arm7_timer_epoch_schedule();
}

/* ************************************************************************** */

/**
 * \brief ARM7 AIC fast vetor.
 *
 * Currently not tested and will likely break, also GCC is not smart
 * enough to allocate the shadowed registers when specifying that a function
 * is a FIQ so I doubt that this will ever be usefull.
 */
__attribute__((naked)) ENV_EXT_FORCE_RAM void _arm7_aic_vec_fast(void)
{
	/*
	 * Just grab the first shadowed registers, load the address and
	 * begone.
	 */
	asm volatile(
		"ldr	r8, =_arm7_aic_fvr\n"
		"ldr	r8, [r8]\n"
		"mov	pc, r8\n"
		);
}

/* ************************************************************************** */

/**
 * \brief ARM7 AIC vector.
 */
__attribute__((naked)) ENV_EXT_FORCE_RAM void _arm7_aic_vec(void)
{
	/* Save context, not fast but makes life easier. */
	ARM7_CONTEXT_SAVE();

	/* 
	 * Do this in assembly since we don't want to mess it up by using the stack
	 * etc.
	 */
	asm volatile(
		"ldr	r0, =_arm7_aic_ivr\n"
		"ldr	r0, [r0]\n"
		"ldr	r0, [r0]\n"
		"mov	lr, pc\n"
		"bx		r0\n"
		"ldr	r0, =_arm7_aic_eoicr\n"
		"ldr	r0, [r0]\n"
		"str	r0, [r0]\n"
		);

	/* Restore context. */
	ARM7_CONTEXT_RESTORE();
}
