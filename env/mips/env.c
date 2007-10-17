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

#if ! defined TT_MANGLED
#	include <env.h>
#	include <types.h>
#	include <kernel.h>
#endif

/* ************************************************************************** */

/**
 * \brief MIPS timestamp.
 *
 * Holds the time of the last interrupt that was handled.
 */
env_time_t mips_timestamp;

/* ************************************************************************** */

/** \cond */
__attribute__((aligned(4))) unsigned char mips_stack[MIPS_STACKSIZE];
static size_t mips_stack_offset = MIPS_STACKSIZE-ENV_STACKSIZE_IDLE;
unsigned char *mips_stack_start = &mips_stack[MIPS_STACKSIZE];
unsigned char *mips_stack_end= mips_stack;

/*
 * NOTE: Timer frequency is CPU timer divided by 16.
 */

/* 
 * Syncsim CTL register layout.
 *
 * bit 0: counter enable flag.
 * bit 1: overflow interrupt enable flag.
 * bit 2: overflow interrupt flag.
 * bit 3: compare1 interrupt enable flag.
 * bit 4: compare1 interrupt flag.
 * bit 5-31: reserved for future use.
 */
enum
{
	TMR_CTL_ENABLE = (1<<0),
	TMR_CTL_OIE    = (1<<1),
	TMR_CTL_OFG    = (1<<2),
	TMR_CTL_CIE    = (1<<3),
	TMR_CTL_CFG    = (1<<4)
};
extern volatile unsigned int TMR_CTL;

/*
 * SyncSim CNT register layout.
 * bit 0-31: current value of timer.
 */
extern volatile unsigned int TMR_CNT;

/*
 * SyncSim CMP register layout.
 * bit 0-31: current compare value.
 */
extern volatile unsigned int TMR_CMP;

/*
 * Syncsim IO CTL register.
 */
extern volatile unsigned int IO_CTL;

/*
 * Syncsim IO IN register.
 */
extern volatile unsigned int IO_IN;

/*
 * Syncsim IO OUT register.
 */
extern volatile unsigned int IO_OUT;

/*
 * MIPS interrupt table, there are 6 interrupt sources but only one vector.
 * The table is usually populated manually by the user program.
 */
void mips_timer_interrupt(void);
void (*mips_interrupt_table)(void) = {mips_timer_interrupt};

/** \endcond */

/**
 * \brief MIPS init funciton.
 *
 * Should place the environment in a susable state, for now just setup
 * the timer. nothing else.
 */
void mips_init(void)
{
	int tmp;

	__asm__ __volatile (
		"mfc0	%0, $12\n"
		"ori	%0, 0xc00\n"
		"mtc0	%0, $12\n"
		: "=r" (tmp)
		);
}

/* ************************************************************************** */

/**
 * \brief MIPS panic function.
 *
 * Shold indicate that something went very wrong, let's just hang for
 * now to indicate this...
 */
void mips_panic(const char * const msg)
{
	for (;;);
}

/* ************************************************************************** */

/**
 * \brief MIPS timer start function, no-op for now.
 */
void mips_timer_start(void)
{
	TMR_CTL |= TMR_CTL_ENABLE;
}

/* ************************************************************************** */

/**
 * \brief MIPS timer set function, invoked by ENV_TIMER_SET(next).
 *
 * \param next The next point in time when an interrupt should occur.
 */
void mips_timer_set(env_time_t next)
{
	TMR_CTL &= ~(TMR_CTL_CIE|TMR_CTL_CFG);
	TMR_CMP = next;
	TMR_CTL |= TMR_CTL_CIE;
	if (TMR_CMP <= TMR_CNT && !(TMR_CTL & TMR_CTL_CFG))
		TMR_CTL |= TMR_CTL_CFG;
}

/* ************************************************************************** */

/**
 * \brief MIPS timer start function, no-op for now.
 *
 * \param context The context to initialize.
 * \param stacksize The size of the stack in bytes.
 * \param function The thread function that should be run.
 */
void mips_context_init(
		mips_context_t *context,
		size_t stacksize,
		tt_thread_function_t function
		)
{
	int i;

	if (mips_stack_offset < stacksize)
		mips_panic("mips_context_init(): Out of stack space.\n");

	context->sp = (void *)&mips_stack[mips_stack_offset-4];
	context->cookie = (void *)&mips_stack[mips_stack_offset-stacksize];
	*context->cookie = MIPS_CONTEXT_COOKIE;
	*--context->sp = (unsigned int)function;

	/* 
	 * Registers 28(global pointer) should be whatever it is now,
	 * register 30(frame pointer) should be the same as the base sp.
	 */
	*--context->sp = (unsigned int)(context->sp + 1); /* r30 */
	__asm__ __volatile ("move	%0, $28\n" : "=r" (i));
	*--context->sp = i; /* r28 */
	/* Registers 16-23 can be anything. */
	for (i=0;i<8;i++)
		*--context->sp = i;

	mips_stack_offset -= stacksize;
}

/* ************************************************************************** */

/**
 * \brief MIPS idle function.
 *
 * Idle function, for now this only hangs the current process.
 */
void mips_idle(void)
{
	unsigned int cmp, cnt, ctl, sp;
	
	__asm__ __volatile ("move %0, $29\n" : "=r" (sp));
	if (sp <= (unsigned int)&mips_stack[MIPS_STACKSIZE-ENV_STACKSIZE_IDLE])
		mips_panic("Stack overrun in mips_idle.\n");

	tt_current->cookie = (void *)&mips_stack[MIPS_STACKSIZE-ENV_STACKSIZE_IDLE];
	*tt_current->cookie = MIPS_CONTEXT_COOKIE;

	mips_protect(0);

	for (;;)
	{
		cnt = TMR_CNT;
		cmp = TMR_CMP;
		ctl = TMR_CTL;
	}
}

/* ************************************************************************** */

/**
 * \brief MIPS timer interrupt.
 */
void mips_timer_interrupt(void)
{
	if (!(TMR_CTL & TMR_CTL_CFG))
		return;
	TMR_CTL &= ~(TMR_CTL_CFG|TMR_CTL_CIE);
	tt_expired(TMR_CMP);
	tt_schedule();
}
