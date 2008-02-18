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

#ifndef ARM7_H_
#define ARM7_H_

#include <types.h>
#include <kernel.h>

/* ************************************************************************** */

void arm7_context_init(env_context_t *, size_t, tt_thread_function_t);
void arm7_context_dispatch(env_context_t *);
void _arm7_context_dispatch(env_context_t *);

#if defined __thumb__
int _arm7_isprotected(void);
void _arm7_protect(int);
#endif

extern unsigned char arm7_stack[];

/* ************************************************************************** */

#define ARM7_BREAK() \
	asm volatile (".word 0xe1200070\n")
	/*asm volatile (".word 0xe7f000f0\n") GDB style*/

/* ************************************************************************** */

/**
 * \brief ARM7 number of threads.
 */
#define ENV_NUM_THREADS	5

/* ************************************************************************** */

/**
 * \brief ARM7 stack size.
 */
#define ENV_STACKSIZE 256

/* ************************************************************************** */

/**
 * \brief ARM7 idle stack size.
 */
#define ENV_STACKSIZE_IDLE 128

/* ************************************************************************** */

/**
 * \brief ARM7 context cookie.
 */
#define ARM7_CONTEXT_COOKIE 0x55aa55aa

/* ************************************************************************** */

/**
 * \brief Environment protect macro.
 *
 * Should place the environment in a protected mode when state is non-zero
 * otherwise it should leave protected mode.
 */
#if ! defined __thumb__
#	define ENV_PROTECT(state) \
		arm7_protect(state)
#else
#	define ENV_PROTECT(state) \
		_arm7_protect(state)
#endif

/* ************************************************************************** */

/**
 * \brief Environment isprotected macro.
 *
 * Should evaluate to non-zero when the environment is in a protected mode
 * otherwise it should evaluate to zero.
 */
#if ! defined __thumb__
#	define ENV_ISPROTECTED() \
		arm7_isprotected()
#else
#	define ENV_ISPROTECTED() \
		_arm7_isprotected()
#endif

/* ************************************************************************** */

/**
 * \brief Environment context init macro.
 *
 * Should initialize a context with a certain amount of stack and running the
 * function. The definition here is just because we wish to compile the
 * skeleton environment without warnings.
 */
#define ENV_CONTEXT_INIT(context, stacksize, function) \
	arm7_context_init(context, stacksize, function)

/* ************************************************************************** */

/**
 * \brief Environment context dispatch macro.
 *
 * Should suspend the current context and start executing the specified
 * context. Must change the tt_current variable.
 */
#define ENV_CONTEXT_DISPATCH(context) \
	arm7_context_dispatch((env_context_t *)context)

/* ************************************************************************** */

/**
 * \brief Environment context save macro.
 *
 * Saves the current user/system context to tt_current.
 */
#define ARM7_CONTEXT_SAVE() \
	__asm__ __volatile__ (\
		/* r0 & r1 is used as a temporary register, we need lr in swi mode. */\
		"stmfd	sp!, {r0,r1,lr}\n"\
		/* Get the user mode stack pointer value. */\
		"stmdb	sp, {sp}^\n"\
		"nop\n"\
		"ldmdb	sp, {r0}\n"\
		/* Store the return address at the first/highest address. */\
		"sub	lr, lr, #4\n"\
		"stmfd	r0!, {lr}\n"\
		/* Start using lr and restore old r0. */\
		"mov	lr, r0\n"\
		"ldmfd	sp, {r0}\n"\
		/* Save all user mode registers. */\
		"stmfd	lr, {r0-r14}^\n"\
		"nop\n"\
		"sub	lr, lr, #60\n"\
		/* Save the saved process register. */\
		"mrs	r0, SPSR\n"\
		"stmfd	lr!, {r0}\n"\
		/* Save the stack pointer to the the current variable. */\
		"ldr	r0, =tt_current\n"\
		"ldr	r0, [r0]\n"\
		"str	lr, [r0]\n"\
		/* Check the context cookie. */\
		"ldr	r0, [r0, #4]\n"\
		"ldr	r0, [r0]\n"\
		"ldr	r1, =arm7_context_cookie\n"\
		"ldr	r1, [r1]\n"\
		"cmp	r0, r1\n"\
		"bne	arm7_context_panic\n"\
		/* \
		 * Restore the old r0 _again_, this is so we can use this macro\
		 * in the software interrupt as well. Don't forget the original\
		 * lr as well for interrupt id.\
		 */\
		"ldmfd	sp!, {r0,r1,lr}\n"\
		)

/* ************************************************************************** */

/**
 * \brief Environment context restore macro.
 *
 * Restores the tt_current context and start executing it.
 */
#define ARM7_CONTEXT_RESTORE() \
	__asm__ __volatile__ (\
		/* Load the current context stack pointer. */\
		"ldr	r0, =tt_current\n"\
		"ldr	r0, [r0]\n"\
		"ldr	lr, [r0]\n"\
		/* Check the context cookie. */\
		"ldr	r0, [r0, #4]\n"\
		"ldr	r0, [r0]\n"\
		"ldr	r1, =arm7_context_cookie\n"\
		"ldr	r1, [r1]\n"\
		"cmp	r0, r1\n"\
		"bne	arm7_context_panic\n"\
		/* Restore the saved saved process status. */\
		"ldmfd	lr!, {r0}\n"\
		"msr	SPSR, r0\n"\
		/* Restore the user context. */\
		"ldmfd	lr, {r0-r14}^\n"\
		"nop\n"\
		"add	lr, lr, #60\n"\
		/* Get the return address and return(leaves interrupt..). */\
		"ldmfd	lr, {pc}^\n"\
		)

/* ************************************************************************** */

/**
 * \brief Environment startup macro.
 *
 * Should make the environment run the given function upon startup and/or
 * reset.
 */
#define ENV_STARTUP(function) \
__attribute__((naked)) int main(void)\
{\
	asm(\
		"ldr	r0, =arm7_stack_start\n"\
		"ldr	sp, [r0]\n"\
	   );\
	tt_init();\
	function();\
	tt_run();\
	return 0;\
} extern char dummy /* Force semi-colon. */

/* ************************************************************************** */

/**
 * \brief ARM7 stack size(total).
 */
#define ARM7_STACKSIZE (ENV_NUM_THREADS*ENV_STACKSIZE+ENV_STACKSIZE_IDLE)

/* ************************************************************************** */

/**
 * \brief ARM7 protect function.
 *
 * \param state non-zero to enter protected mode, zero to leave protected mode.
 */
static inline void arm7_protect(int state)
{
	/*
	 * WARNING!!!
	 * 	The use of %0 et al. is _very_ fragile and may break at any time.
	 * 	Honestly I don't have a clue why this miscompiles every now and then...
	 */
	int tmp;
	asm volatile(
		"cmp	%1, #0\n"
		"mrs	%0, CPSR\n"
		"orrne	%0, %0, #0x80|0x40\n"
		"biceq	%0, %0, #0x80|0x40\n"
		"msr	CPSR_c, %0\n"
		: "=r" (tmp)
		: "r" (state)
		);
}

/* ************************************************************************** */

/**
 * \brief ARM7 isprotected function.
 *
 * \return non-zero if protected, nonzero otherwise.
 */
static inline int arm7_isprotected(void)
{
	/*
	 * WARNING!!!
	 * 	The use of %0 et al. is _very_ fragile and may break at any time.
	 * 	Honestly I don't have a clue why this miscompiles every now and then...
	 */
	int tmp;
	asm volatile(
			"mrs	%0, CPSR\n"
			"and	%0, %0, #0x80|0x40\n"
			: "=r" (tmp)
			);
	return tmp;
}

#endif
