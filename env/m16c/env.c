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

#include <env.h>
#include <types.h>

#include <tT.h>
#include <kernel.h>

/** \cond */

unsigned char m16c_stack[M16C_STACKSIZE];
static unsigned int m16c_stack_offset = M16C_STACKSIZE-ENV_STACKSIZE_IDLE;

env_time_t m16c_timer_next = 0;
env_time_t m16c_timer_base = 0;
env_time_t m16c_timer_timestamp = 0;
static char m16c_timer_active = 0;

/*
 * Epoch schedule funtion, handles scheduling of interrupts in the middle
 * of each epoch (between 2 timer underflows).
 */
static inline void m16c_timer_epoch_schedule(void) {
	signed long diff;

	/* Only schedule when there is an active baseline. */
	if (!m16c_timer_active)
		return;

	diff = m16c_timer_next - m16c_timer_base;
	if (diff < M16C_TIMER_COUNT) {
		if (diff <= (M16C_TIMER_COUNT - TA0)) {
			/* Must use mov instruction for this register. */
			asm ("mov.w	#1, %0\n" :: "m" (TA1));
		} else {
			asm (
				"mov.w	%0, %1\n"
				::
				"r" ((unsigned short)diff),
				"m" (TA1)
				);
		}
		ONSF.BIT.TA1OS = 1;
	}
}

/*
 * The timer 0 interrupt, only handles the updating of the abolute time counted
 * by m16c_timer_base.
 */
ENV_EXT_INTERRUPT_RAW(M16C_TMRA0) {
	m16c_timer_base += M16C_TIMER_COUNT;
	m16c_timer_epoch_schedule();
}


/*
 * Timer 1 interrupt, will occur when a baseline has expired and will notify
 * the kernel of this.
 */
void m16c_timera1_interrupt(void) {
	m16c_timer_active = 0;

	tt_expired(m16c_timer_next);
	tt_schedule();
}
ENV_INTERRUPT(M16C_TMRA1, m16c_timera1_interrupt); /* Install handler. */

/** \endcond */

/* ************************************************************************** */

/**
 * \brief M16C Initalization.
 *
 * Takes care of serial and timer setup, should render the environment in
 * a usable state.
 */
void m16c_init(void) {
	/* Setup USART1 at 57600. */
	U1MR.BYTE = 0x05;
	U1C0.BYTE = 0x10;
	UCON.BYTE = 0x00;
	U1BRG     = 0x0a; /* brg = f/(16*baud)-1 */
	U1TB.WORD = 0x0000;
	U1C1.BYTE = 0x01; /* Effectivly enable transmisson only. */

	/*
	 * Ok, setting up TimerA0 and TimerA1. TimerA0 will stand for the absolute
	 * time and TimerA1 will be used to generate pseudo accurate interrupts
	 * during between the TimerA0 underflows. Not perfect but since this is
	 * a crappy platform this is probably as good as it gets :/
	 */

	/* TimerA0: continous down count timer. */
	TA0MR.BYTE = 0x80;
	/* Must use mov instruction for this register. */
	asm ("mov.w	#0xffff, %0\n" :: "m" (TA0));
	TA0IC.BYTE = 0x01; /* Interrupt prio 1. */

	/* TimerA1: one-shot down count timer. */
	TA1MR.BYTE = 0x82;
	TA1IC.BYTE = 0x01; /* Interrupt prio 1. */
	TABSR.BIT.TA1S = 1; /* Start, but do not trigger. */
}

/* ************************************************************************** */

/**
 * \brief M16C Print Function.
 *
 * Prints a given string to USART1.
 *
 * \param str The string that should be printed.
 */
void m16c_print(const char *str) {
	while (*str) {
		while (!(U1C1.BYTE & 0x02));
		U1TB.BYTE.U1TBL = *str++;
	}
	while (!(U1C1.BYTE & 0x02));
}

/* ************************************************************************** */

/**
 * \brief M16C Print hex.
 *
 * Will print the dexadecimal value of the given variable.
 *
 * \param val The value to print.
 */
void m16c_print_hex(unsigned long val) {
	static char hex[16] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'a', 'b',
		'c', 'd', 'e', 'f'
	};
	char fmt[] = "0x00000000\n";
	char *tmp = &fmt[sizeof(fmt)-3];

	while (val) {
		*tmp-- = hex[val&0x0f];
		val >>= 4;
	}
	m16c_print(fmt);
}

/* ************************************************************************** */

/**
 * \brief M16C Panic function.
 *
 * Prints the given message and enters an infiite loop processor.
 *
 * \param str The string that should be printed.
 */
void m16c_panic(const char *str) {
	m16c_protect(1);
	m16c_print(str);
	for (;;);
}

/* ************************************************************************** */

/**
 * \brief M16c Idle function.
 *
 * Should initialize the idle thread and place the environment in an idle state.
 */
void m16c_idle(void) {
	/* Don't forget to setup the context cookie. */
	tt_current->context.cookie =
		(void *)&m16c_stack[M16C_STACKSIZE-ENV_STACKSIZE_IDLE];
	*tt_current->context.cookie = M16C_CONTEXT_COOKIE;

	m16c_protect(0);
	for (;;)
		asm("wait\n");
}

/* ************************************************************************** */
/**
 * \brief M16C Environment timer start function.
 *
 * Will start the environment timer.
 */
void m16c_timer_start(void) {
	TABSR.BIT.TA0S = 1;
}

/* ************************************************************************** */

/**
 * \brief M16C Environment timer set function.
 *
 * Schedules the next interrupt.
 *
 * \param when When the next interrupt should occur.
 */
void m16c_timer_set(env_time_t when) {
	m16c_timer_next = when;
	m16c_timer_active = 1;

	m16c_timer_epoch_schedule();
}

/* ************************************************************************** */

/**
 * \brief M16C Context Initilzation.
 *
 * Should initialize the given thread context.
 *
 * \param context The context to initialize.
 * \param stacksize The amount of stack for this thread.
 * \param function The function this thread should run.
 */
void m16c_context_init(
		m16c_context_t *context,
		size_t stacksize,
		tt_thread_function_t function
		) {
	unsigned int i;

	/*
	 * We must save a return address (function) and a fake FLG byte, in
	 * addition there are 7 banked registers + one unbanked register to save,
	 * additionally there are 16 pseudo-registers (one byte each) to save.
	 * We also need 2 bytes for the context cookie (used to check for stack
	 * overflow/underflow).
	 *
	 * Total sum is 4 + 2 + 7*2 + 16 = 36 bytes (the sb register is first
	 * register that is saved and we save the current value for it).
	 */

	if (m16c_stack_offset < stacksize)
		m16c_panic("m16c_context_init(): Out of stack space.\n");

	context->sp = (void *)&m16c_stack[m16c_stack_offset];
	context->cookie = (void *)&m16c_stack[m16c_stack_offset-stacksize];
	*context->cookie = M16C_CONTEXT_COOKIE;

	/*
	 * Fake an interurpt return sequece, we should push in the order:
	 *
	 * 	High-Address
	 * 	FLG-Byte (lower).
	 * 	Middle-Address
	 * 	Low-Address
	 *
	 * NOTE:
	 * 	GCC use indirect function pointers to make life easier for the
	 * 	compiler (all pointer can be 16bit) so we must fake the high-address
	 * 	byte (zero should always work?).
	 */
	*(--context->sp) = 0x00; /* Fake address high byte. */
	*(--context->sp) = 0x80; /* Fake FLG byte. */
	*(--context->sp) = ((unsigned int)function >> 8) & 0xff;
	*(--context->sp) = ((unsigned int)function) & 0xff;

	/* SB, 2 bytes. */
	asm ("stc	sb, %0\n" : "=r" (i));
	*(--context->sp) = i >> 8;
	*(--context->sp) = i & 0xff;

	/* Registers, 30 bytes. */
	for (i=0;i<30;i++)
		*(--context->sp) = i;

	m16c_stack_offset -= stacksize;
}

/**
 * \brief M16C Context dispatch function.
 *
 * Saves current context to tt_current and restore the given context.
 *
 * \param context The context to restore.
 */
#if 0
void m16c_debug_stack(unsigned char *begin, unsigned char *end) {
	m16c_print("Begin: ");
	m16c_print_hex((unsigned long)begin);
	while (begin != end) {
		m16c_print("\t");
		m16c_print_hex((unsigned long)*begin++);
	}
	m16c_print("End: ");
	m16c_print_hex((unsigned long)begin);
}
void m16c_print_return(unsigned char *addr) {
	unsigned long data;
	unsigned char *tmp = addr;
	data = *tmp++;
	data = (((unsigned long)*tmp) << 8) | data;
	tmp += 2;
	data = (((unsigned long)*tmp) << 16) | data;
	m16c_print("Returning to address: ");
	m16c_print_hex(data);
	tmp = addr;
	data = *tmp++;
	data |= ((unsigned long)*tmp++) << 8;
	data |= ((unsigned long)*tmp++) << 16;
	data |= ((unsigned long)*tmp++) << 24;
	m16c_print("reit data: ");
	m16c_print_hex(data);
	m16c_print("from: ");
	m16c_print_hex(addr);
}
#endif

void m16c_context_dispatch_dummy_(tt_thread_t *context) {
	/* 
	 * NOTE:
	 * 	__attribute__((naked)) does not work on M16C/M32C so be
	 * 	carefull (i.e. check the disassembly).
	 */
	asm (
		".global _m16c_context_dispatch\n"
		"_m16c_context_dispatch:\n"
		);


	/*
	 * Always fake the return path so that we may return from interrupts at
	 * any time.
	 */
	asm (
		"fset	b\n"
		"stc	flg, r0\n"
		"bclr	4, r0\n"
		"pop.w	r1\n"
		"pop.b	r0h\n"
		"push.w	r0\n"
		"push.w	r1\n"
		"fclr	b\n"
		);

	ENV_CONTEXT_SAVE();

	/* context is in r1 and should not be messed up by the save macro. */
	asm ("mov.w	r1, %0\n":: "m" (tt_current));

	ENV_CONTEXT_RESTORE();

	/* 
	 * This we are always able to return from interrupt, when we don't return
	 * from an interrupt we have faked it (see above).
	 */
	asm ("reit\n");
}
