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

/*
 * Following files should not be included in case the file is mangled.
 */
#if ! defined TT_MANGLED
#	include <env.h>
#	include <types.h>
#	include <kernel.h>
#endif

/* ************************************************************************** */

/**
 * \brief The initial timer value, used for testing mainly.
 */
#define MSP430_TIME_START 0xfff00000

/* ************************************************************************** */

/**
 * \brief The current value of the internal time counter.
 */
env_time_t msp430_timer_base = MSP430_TIME_START;

/* ************************************************************************** */

/**
 * \brief The timer active flag.
 */
int msp430_timer_active = 0;

/* ************************************************************************** */

/**
 * \brief The timestamp of the last interrupt.
 */
env_time_t msp430_timer_timestamp = MSP430_TIME_START;

/* ************************************************************************** */

/**
 * \brief The next timer interrupt as requested by tT.
 */
env_time_t msp430_timer_next = 0;

/* ************************************************************************** */

/**
 * \brief msp430 Stack offset address.
 */
size_t msp430_stack_offset = MSP430_STACKSIZE-ENV_STACKSIZE_IDLE;

/* ************************************************************************** */

/**
 * \brief MSP430 stack space.
 */
unsigned char __attribute__((aligned(2), section(".bss"))) msp430_stack[MSP430_STACKSIZE];

#if 0
/* ************************************************************************** */

/**
 * \brief Start of the idle stack.
 */
void *msp430_stack_start = &msp430_stack[MSP430_STACKSIZE];
#endif
/* ************************************************************************** */

/**
 * \brief The msp430 environment init function.
 *
 * Will initialize the timer for the msp430 and
 * the USART0 for communication.
 */
void msp430_init(void)
{
	/* Always set the MCLK on pin 5.4(1611 specific?). */
	P5SEL |= 0x10;

	/* Step up the MCLK to maximum(on 1611 this is about 4.7MHz. */
	DCOCTL = DCO0|DCO1|DCO2;
	BCSCTL1 |= RSEL0|RSEL1|RSEL2;

	/* Enable the module. */
	U0ME |= URXE0 + UTXE0;

	/* Set the pins for USART, bits 4 and 5. */
	P3SEL |= (0x10|0x20);

	/* Disable the USART. */
	U0CTL |= SWRST;

	/* Setup the USART. */
	U0CTL = CHAR|SWRST;
	U0TCTL = SSEL0; /* Formerly TCSSEL_ACLK, i.e. ACLK. */
	U0RCTL = 0x00;

	/* Set the baud rate. */
	U0BR1 = 0x00;
	U0BR0 = 0x03;
	U0MCTL = 0x4a;

	/* Enable the USART again. */
	U0CTL &= ~SWRST;

	/* 
	 * Initialize TimerA module to use the ACLK.
	 */
	TACTL = TASSEL0|TAIE;

	/* 
	 * Setup the Capture-Compare module.
	 */
	TACCTL0 = 0x0000;
}

/* ************************************************************************** */

/**
* \brief The MSP430 print function.
*
* Will print the string to USART0.
*
* \param str String that should be printed.
*/
void msp430_print(const char *str)
{
	while (*str)
	{
		while (!(U0TCTL & TXEPT));
		U0TXBUF = *str++;
	}
	while (!(U0TCTL & TXEPT));
}

/* ************************************************************************** */

/**
 * \brief The msp430 panic function.
 *
 * Will print a message to USART0 and then enter
 * low-power mode4.
 *
 * \param msg The message to print.
 */
void msp430_panic(const char *msg)
{
	msp430_protect(1);

	msp430_print(msg);
	
	LPM4;
}

/* ************************************************************************** */

/**
 * \brief The non-maskable interrupt handler, just in case.
 */
interrupt(NMI_VECTOR) msp430_nmi(void)
{
	if (FCTL3 & ACCVIE)
		msp430_print("Flash access violation.\n");

	if (IFG1 & OFIFG)
		msp430_print("Oscillator fault.\n");

	if (IFG1 & NMIIFG)
		msp430_print("Reset.\n");

	/* "Park" the CPU here. */
	LPM4;
}

/* ************************************************************************** */

/**
 * \brief Schedule any interrupt that should occur during this epoch.
 *
 * Epoch is defined as the time it takes the counter to wrap around or
 * MSP430_TIMER_COUNT. Inline is good since this is used in the interrupt
 * routine.
 */
static inline void msp430_timer_epoch_schedule(void)
{
	signed long diff;
	
	/* 
	 * Make sure we have any pending interrupts before we actually do
	 * any significant work.
	 */
	if (!msp430_timer_active)
		return;

	/* 
	 * Always disable the interrupt and clear the flag before we start setting
	 * something new.
	 */
	TACCTL0 &= ~(CCIE|CCIFG);

	/* 
	 * Schedule an interrupt if it has occured or if it will occur during
	 * this epoch. We use CCR1 to schedule this interrupt.
	 */
	diff = (signed long)msp430_timer_next - (signed long)msp430_timer_base;
	if (diff < MSP430_TIMER_COUNT)
	{
		/*
		 * The interrupt should occur during this epoch but the
		 * time might have already elapsed, if so schedule it ASAP.
		 */
		if (TAR >= diff)
			TACCR0 = TAR + 1;
		else
			TACCR0 = diff;

		/* Enable the interrupt so that we will receive the interrupt. */
		TACCTL0 |= CCIE;

		/*
		 * Check if we managed to miss the time. ie. if TAR became
		 * TACCR0 while we assigned it.
		 */
		while(TACCR0 <= TAR && !(TACCTL0 & CCIFG))
		{
			/*
			 * Do NOT use TACCR0++ since it might *#&"#-up when we wrap around.
			 * This might still *#&"#-up but the counter does not run amok.
			 */
			TACCR0 = TAR + 1;
		}
	}
}

/* ************************************************************************** */

/**
 * \brief The msp430 Timer set function.
 *
 * Will set the next timer interrupt that should occur.
 */
void msp430_timer_set(env_time_t next)
{
	/*
	 * The next value and the active flag is used in the epoch sheduler
	 * so let's set them before we call it? *duh*
	 */
	msp430_timer_next = next;
	msp430_timer_active = 1;

	/* Schedule interrupt of repoch if suitable. */
	msp430_timer_epoch_schedule();
}

/* ************************************************************************** */

/**
 * \brief The timera compare0 interrupt.
 *
 * This is  the interrupt used to trigger any interrupt during an
 * epoch.
 */
__attribute__((naked)) interrupt(TIMERA0_VECTOR) msp430_timera_compare0(void)
{
	/* Save context. */
	MSP430_CONTEXT_SAVE();
	
	/* 
	 * We always want to disable our own interrupt here, will be
	 * set by msp430_timer_epoch_schedule() if needed. Also make the
	 * timer inactive in case no other interrupts should be
	 * generated.
	 */
	TACCTL0 &= ~(CCIE|CCIFG);
	msp430_timer_active = 0;

	/* 
	 * Interrupt tinyTimber and pass the time that expired
	 * to the kernel.
	 */
	tt_expired(msp430_timer_get());

	/* 
	 * Make sure that if any higher priority messages expired then
	 * we will actually run them, should this be done in expired
	 * or here? Less stack usage if we do it here...
	 */
	tt_schedule();

	/* Restore the context. */
	MSP430_CONTEXT_RESTORE();

	asm volatile("reti");
}

/* ************************************************************************** */

/**
 * \brief The timera compare1 interrupt.
 *
 * This interrupt advances the base time of the software clock. Note that
 * this interrupt has a lower priority than the compare0 interrupt.
 */
interrupt(TIMERA1_VECTOR) msp430_timera_compare1(void)
{
	/* Bogus read of TAIV to clear interrupt, do NOT remove. */
	TAIV;

	/* This creates a _lot_ of overhead :/
	if (TAIV != TAIV_OVERFLOW)
		msp430_panic("Expected overflow interrupt, got something else.\n");*/

	/* One epoch has expired. */
	msp430_timer_base += MSP430_TIMER_COUNT;

	/* Schedule any interrupts for this epoch. */
	msp430_timer_epoch_schedule();
}

/* ************************************************************************** */

/**
 * \brief The msp430 context init function.
 *
 * Will initialize a thread context so that we can dispatch it.
 *
 * \param context The context to initialize.
 * \param stacksize The size of the stack to use.
 * \param function The function that should be called.
 */
void msp430_context_init(
		msp430_context_t *context,
		size_t stacksize,
		void (*function)(void)
		)
{
	int i;
	unsigned short fp;

	/* Make sure we have space left for a new context. */
	if (msp430_stack_offset < stacksize)
		msp430_panic("msp430_context_init(): Out of memory.\n");

	/* Grab some space. */
	context->sp = (void *)&msp430_stack[msp430_stack_offset];
	context->cookie = (void *)&msp430_stack[msp430_stack_offset-stacksize];
	*context->cookie = MSP430_CONTEXT_COOKIE;

	/* Save the sp as fp so we can use it later. */
	fp = *context->sp;

	/* Return address and processor state. */
	*--context->sp = (unsigned short)function;
	*--context->sp = 0;

	/* Push some fake registers onto the stack. */
	*--context->sp = fp;
	for (i=5;i<16;i++)
		*--context->sp = i;

	/* Update where the next stack should go. */
	msp430_stack_offset -= stacksize;
}

/* ************************************************************************** */

/**
 * \brief MSP430 context dispatch function.
 *
 * Should dispatch the specified context after saving the current.
 */
__attribute__((naked)) void msp430_context_dispatch(tt_thread_t *context)
{
	/*
	 * Since we want to be able to treat this as interrupt cotext we
	 * must save the SR(r2) on the stack.
	 */
	__asm__ __volatile__ ("push	r2\n");

	/* Save the current context. */
	MSP430_CONTEXT_SAVE();

	/* Change the current context. */
	tt_current = context;

	/* Restore the previous context. */
	MSP430_CONTEXT_RESTORE();

	/*
	 * This will restore the thread SR(r2) and the pc. We treat all threads
	 * as in interrupt mode whenever we return to to them.
	 */
	__asm__ __volatile ("reti\n");
}

/* ************************************************************************** */

/**
 * \brief MSP430 context-panic function.
 */
void msp430_cookie_panic(void)
{
	msp430_panic("Context cookie corrupted.\n");
}

/* ************************************************************************** */

/**
 * \brief The MSP430 context idle dispatch function.
 *
 * All we do here is to set the sp to the lowest possible value and
 * enter the LPM3 mode.
 */
void msp430_idle(void)
{
	/* Make sure we didn't overrun the stack already. */
	unsigned short sp = (unsigned short)READ_SP; /* Workaround for broken READ_SP macro... */
	if (
			sp <=
			(unsigned short)&msp430_stack[
				MSP430_STACKSIZE-ENV_STACKSIZE_IDLE
			]
		) {
		msp430_panic(
			"msp430_idle(): Idle stack overrun during init.\n");
	}

	/* 
	 * Reset the sp to the top of the stack, we don't ever plan to return.
	 */
	WRITE_SP(&msp430_stack[MSP430_STACKSIZE]);

	/*
	 * Fix the context cookie.
	 */
	tt_current->context.cookie =
		(void*)&msp430_stack[ENV_STACKSIZE-ENV_STACKSIZE_IDLE];
	*tt_current->context.cookie = MSP430_CONTEXT_COOKIE;

	/* 
	 * Let's not forget to leave protected mode so that we receive
	 * interrupts.
	 */
	msp430_protect(0);

	/* 
	 * We want the ACLK to be running so that we can offer some timing
	 * services during idle time.
	 */
	/*for (;;);*/
	LPM3;

	/* 
	 * Make ABSOLUTELY sure that that a return from this function is
	 * noticed.
	 */
	msp430_panic("msp430_idle(): LPM3 returned.\n");
}
