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

#include "iom16c62.h"

/** \cond */

unsigned char m16c_stack[M16C_STACKSIZE];
static unsigned int m16c_stack_offset = M16C_STACKSIZE-ENV_STACKSIZE_IDLE;

/** \endcond */

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
}

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
	char fmt[] = "0x00000000";
	char *tmp = &fmt[sizeof(fmt)-2];

	while (val) {
		*tmp-- = hex[val&0x0f];
		val >>= 4;
	}
	m16c_print(fmt);
}

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

/**
 * \brief M16c Idle function.
 *
 * Should initialize the idle thread and place the processor in an idle state.
 */
void m16c_idle(void) {
	/* Enable interrupts and hang until the heat death of the universe. */
	m16c_protect(0);
	for (;;)
		asm("wait\n");
}

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
	int i;

	if (m16c_stack_offset < stacksize)
		m16c_panic("m16c_context_init(): Out of stack space.\n");

	context->sp = &m16c_stack[m16c_stack_offset];
	/* TODO: context cookie check. */

	/* Fake interrupt return. */
	*(--context->sp) = 0x80;
	*(--context->sp) = 0x00;

	/*
	 * Save return address, push it in the order h:m:l.
	 * 
	 * NOTE:
	 * 	This is a fake function pointer, see GCC documentation for
	 * 	more info.
	 */
	*(--context->sp) = 0x00; /* Fake high byte. */
	*(--context->sp) = ((unsigned int)function >> 8) & 0xff;
	*(--context->sp) = ((unsigned int)function) & 0xff;

	/*
	 * There are 7 banked registers + one unbanked register to save,
	 * additionally there are 16 pseudo-registers (one byte each) to save.
	 * Total sum is 2 + 7*2*2 + 16 = 46 bytes (the sb register is first and we
	 * save the current value for it(.
	 */
	asm ("nop\nstc	sb, %0\nnop\n" : "=r" (i));
	*(--context->sp) = i;
	for (i=0;i<44;i++)
		*(--context->sp) = 0x00;

	m16c_stack_offset -= stacksize;
}

/**
 * \brief M16C Context dispatch function.
 *
 * Saves current context to tt_current and restore the given context.
 *
 * \param context The context to restore.
 */
void m16c_context_dispatch(m16c_context_t *context) {
	/* 
	 * NOTE:
	 * 	__attribute__((naked)) does not work on M16C/M32C so be
	 * 	carefull (i.e. check the disassembly).
	 */
	asm ("pushc	flg\n");
	
	ENV_CONTEXT_SAVE();

	asm ("mov.w	%0, %1\n" :: "r" (context), "m" (tt_current));
	/*tt_current = context;*/
	
	ENV_CONTEXT_RESTORE();

	asm ("reit\n");
}

__attribute__((interrupt)) void m16c_interrupt_test(void) {
	m16c_init();
}
