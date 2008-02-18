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
#include <gba/arm7.h>
#include <gba/sys_call.h>

#include <kernel.h>

#include <string.h>

/* ************************************************************************** */

/** \cond */

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
 * Used in the ENV_CONTEXT_(SAVE|RESTORE) macros. Must not be static or
 * the inline asm will not see it :/
 * Makes life easier(tm).
 */
void arm7_context_panic(void)
{
	ARM7_BREAK();
}

/** \endcond */

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
	if (stacksize & 0x3) {
		ARM7_BREAK();
	}
	
	/* Make sure we have enough stack and assign some to the context. */
	if (arm7_stack_offset < stacksize) {
		ARM7_BREAK();
	}

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
void _arm7_context_dispatch(arm7_context_t *new)
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

__attribute__((naked)) void _arm7_irq(void)
{
	ARM7_CONTEXT_SAVE();

	/* 
	 * Interrupt handling code goes here, such as figuring out what
	 * happened (no vectored interrupt conteoller for the GBA).
	 */
	gba_interrupt();

	ARM7_CONTEXT_RESTORE();
}

__attribute__((naked)) void _arm7_fiq(void)
{
	/* This interrupt is not available on the GBA. */
	ARM7_BREAK();
}
