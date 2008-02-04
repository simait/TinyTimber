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
 * \brief M16C Environment.
 */
#ifndef ENV_M16C_ENV_H_
#define ENV_M16C_ENV_H_

#include <types.h>
#include <kernel.h>
#include <m16c/interrupts.h>

#include <m16c/iom16c62.h>

void m16c_init(void);
void m16c_print(const char *);
void m16c_print_hex(unsigned long);
void m16c_panic(const char *);
void m16c_idle(void);
void m16c_timer_start(void);
void m16c_timer_set(env_time_t);
void m16c_context_init(m16c_context_t *, size_t, tt_thread_function_t);
void m16c_context_dispatch(m16c_context_t *);

/* ************************************************************************** */

/**
 * \brief Environment initialization.
 *
 * Should take care of initializing the environment setting up any hardware
 * required by the environment such as serial ports, timers etc.
 */
#define ENV_INIT() \
	m16c_init()

/* ************************************************************************** */

/**
 * \brief Environemnt debug macro.
 *
 * May print the string pointer to by msg but it is not required.
 */
#define ENV_DEBUG(msg) \
	m16c_print(msg)

/* ************************************************************************** */

/**
 * \brief M16C Environment panic macro.
 *
 * Must indicate that a critical error has occured and halt or reset the
 * execution.
 */
#define ENV_PANIC(msg) \
	m16c_panic(msg)

/* ************************************************************************** */

/**
 * \brief M16C Environment protect macro.
 *
 * Should place the environment in a protected mode when state is non-zero
 * otherwise it should leave protected mode.
 */
#define ENV_PROTECT(state) \
	m16c_protect(state)

/* ************************************************************************** */

/**
 * \brief Environment isprotected macro.
 *
 * Should evaluate to non-zero when the environment is in a protected mode
 * otherwise it should evaluate to zero.
 */
#define ENV_ISPROTECTED() \
	m16c_isprotected()

/* ************************************************************************** */

/**
 * \brief Environment number of threads macro.
 *
 * The number of threads supported by this environment.
 */
#define ENV_NUM_THREADS 5

/* ************************************************************************** */

/**
 * \brief M16C Environment stack size.
 *
 * The size of each worker thread.
 */
#define ENV_STACKSIZE 256

/* ************************************************************************** */

/**
 * \brief M16C Environment stack size.
 *
 * The size of each worker thread.
 */
#define ENV_STACKSIZE_IDLE 256

/* ************************************************************************** */

/**
 * \brief M16C Total stacksize.
 *
 * The size of all the m16c stacks.
 */
#define M16C_STACKSIZE (ENV_STACKSIZE*ENV_NUM_THREADS+ENV_STACKSIZE_IDLE)

/* ************************************************************************** */

/**
 * \brief Environment context init macro.
 *
 * Should initialize a context with a certain amount of stack and running the
 * function. The definition here is just because we wish to compile the
 * skeleton environment without warnings.
 */
#define ENV_CONTEXT_INIT(context, stacksize, function) \
	m16c_context_init(context, stacksize, function)

/* ************************************************************************** */

/**
 * \brief Environment context dispatch macro.
 *
 * Should suspend the current context and start executing the specified
 * context. Must change the tt_current variable.
 */
#define ENV_CONTEXT_DISPATCH(context) \
	m16c_context_dispatch((m16c_context_t *)context)

/* ************************************************************************** */

/**
 * \brief M16C Environment context cookie.
 */
#define M16C_CONTEXT_COOKIE 0x55aa

/* ************************************************************************** */

/**
 * \brief M16C Environment context save macro.
 *
 * NOTE:
 * 	Will leave the stack mode in interrupt stack. Also we do NOT save the
 * 	registers in bank 1.
 */
#define ENV_CONTEXT_SAVE() \
	asm (\
		/* Start saving registers etc. */\
		"fset	b\n"\
		"popm	r0, r1\n"\
		"fset	u\n"\
		"pushm	r0, r1\n"\
		"fclr	b\n"\
		"pushc	sb\n"\
		"pushm	r0, r1, r2, r3, a0, a1, fb\n"\
		/* Save pseudo-registers (each reg is 8bit). */\
		"push.w	mem0\n"\
		"push.w	mem2\n"\
		"push.w	mem4\n"\
		"push.w	mem6\n"\
		"push.w	mem8\n"\
		"push.w	mem10\n"\
		"push.w	mem12\n"\
		"push.w	mem14\n"\
		/* Check cookie and save sp. */\
		"mov.w	%0, a0\n"\
		"mov.w	2[a0], a1\n"\
		"cmp.w	%1, [a1]\n"\
		"jeq	.L%=\n"\
		"mov.w	%2, r1\n"\
		"jsr.a	_m16c_panic\n"\
		".L%=:\n"\
		"stc	sp, [a0]\n"\
		/* Interrupt stack. */\
		"fclr	u\n"\
		::\
			"m" (tt_current),\
			"i" (M16C_CONTEXT_COOKIE),\
			"i" ("Context cookie corruption during save.\n")\
		)

/* ************************************************************************** */

/**
 * \brief M16C Environment context restore macro.
 */
#define ENV_CONTEXT_RESTORE() \
	asm (\
		/* User stack. */\
		"fset	u\n"\
		/* Load sp from tt_current. */\
		"mov.w	%0, a0\n"\
		"mov.w	2[a0], a1\n"\
		"cmp.w	%1, [a1]\n"\
		"jeq	.L%=\n"\
		"mov.w	%2, r1\n"\
		"jsr.a	_m16c_panic\n"\
		".L%=:\n"\
		"ldc	[a0], sp\n"\
		/* Restore pseudo-registers (each reg is 8bit). */\
		"pop.w	mem14\n"\
		"pop.w	mem12\n"\
		"pop.w	mem10\n"\
		"pop.w	mem8\n"\
		"pop.w	mem6\n"\
		"pop.w	mem4\n"\
		"pop.w	mem2\n"\
		"pop.w	mem0\n"\
		/* Assume Bank 0. */\
		"popm	r0, r1, r2, r3, a0, a1, fb\n"\
		"popc	sb\n"\
		::\
			"m" (tt_current),\
			"i" (M16C_CONTEXT_COOKIE),\
			"i" ("Context cookie corruption during restore.\n")\
		)

/* ************************************************************************** */

/**
 * \brief M16C Environment idle macro.
 *
 * Should place the environment in an "idle" state. Can be busy waiting or
 * some power-saving mode. The environment must initialize the context pointerd
 * to by tt_current so that the kernel can dispatch another thread.
 */
#define ENV_IDLE() \
	m16c_idle()

/* ************************************************************************** */

/**
 * \brief M16C Environment timer start macro.
 *
 * Should start the environment timer service.
 */
#define ENV_TIMER_START() \
	m16c_timer_start()

/* ************************************************************************** */

/**
 * \brief M16C Environment timer set macro.
 *
 * Should set the time when tt_expired(time) should be called.
 */
#define ENV_TIMER_SET(value) \
	m16c_timer_set(value)

/* ************************************************************************** */

/**
 * \brief M16C Environment main clock frequency.
 */
#define M16C_MAIN_CLOCK	16000000UL

/* ************************************************************************** */

/**
 * \brief M16C Environment timer count.
 *
 * The number of time units in one epoch.
 */
#define M16C_TIMER_COUNT 0x10000

/* ************************************************************************** */

/**
 * \brief M16C Environment timer frequency.
 */
#define M16C_TIMER_HZ (M16C_MAIN_CLOCK/32/2)

/* ************************************************************************** */

/**
 * \brief Environment timer get macro.
 *
 * Should evaluate to the current time.
 */
#define ENV_TIMER_GET() \
	m16c_timer_get()

/* ************************************************************************** */

/**
 * \brief Environment timestamp macro.
 *
 * Should evaluate to the time when the most recent interrupt was triggered.
 */
#define ENV_TIMESTAMP() \
	m16c_timestamp()

/* ************************************************************************** */

/**
 * \brief Environment usec macro.
 *
 * The given number of usecs should be converted to time-base of
 * the environment.
 */
#define ENV_USEC(n) \
	(((env_time_t)n*M16C_TIMER_HZ)/1000000)

/* ************************************************************************** */

/**
 * \brief Environment msec macro.
 *
 * The given number of msecs should be converted to time-base of
 * the environment.
 */
#define ENV_MSEC(n) \
	(((env_time_t)n*M16C_TIMER_HZ)/1000)

/* ************************************************************************** */

/**
 * \brief Environment sec macro.
 *
 * The given number of secs should be converted to time-base of
 * the environment.
 */
#define ENV_SEC(n) \
	((env_time_t)n*M16C_TIMER_HZ)

/* ************************************************************************** */

/**
 * \brief M16C Environment startup macro.
 *
 * Should make the environment run the given function upon startup and/or
 * reset.
 */
#define ENV_STARTUP(function) \
int main(void)\
{\
	extern unsigned char m16c_stack[];\
	asm("ldc	%0, sp\n":: "i" (m16c_stack + M16C_STACKSIZE));\
	tt_init();\
	function();\
	tt_run();\
	return 0;\
} extern char dummy /* Force semi-colon. */

/* ************************************************************************** */

/**
 * \brief M16C Interrupt macro.
 *
 * This is an ugly, ugly hack... But it works. Do NOT, I repeat NOT, try to
 * make the compiler inline the interrupt code, you WILL f*ck up things
 * by doing this... Sooner or later.
 */
/** \cond */
#define _INT_VEC(value) "__vector_" #value
#define INT_VEC(value) _INT_VEC(value)
/** \endcond */
#define ENV_INTERRUPT(id, function) \
	void _##function##_dummy_(void) {\
		asm(".global " INT_VEC(id) "\n" INT_VEC(id) ":\n");\
		ENV_CONTEXT_SAVE();\
		asm ("jsr.a	_" #function "\n");\
		ENV_CONTEXT_RESTORE();\
		asm("reit");\
	}

/* ************************************************************************** */

/**
 * \brief M16C Raw interrupt macro (extension).
 *
 * This again as with the regular interrupt macro this is a very, _very_ ugly
 * hack to get things working when using the sort of crippled GCC compiler.
 *
 * Also, we do not install a callback function but rather decalre a special
 * name for the function so that the linker script can find it and place it
 * in the correct spot. See the TimerA0 interrupt in the environment sources
 * for an example usage.
 */
#define INT_VEC_RAW(value) _vector_##value
#define ENV_EXT_INTERRUPT_RAW(id) \
	__attribute__((interrupt)) void INT_VEC_RAW(id)(void)

/* ************************************************************************** */

/**
 * \brief M16C Protect function.
 *
 * Disables all interrupts if the parameter is non-zero, otherwise enable
 * all interrupts.
 *
 * \param protect If non-zero enter protected mode, otherwise leave
 * protected mode.
 */
static inline void m16c_protect(int protect) {
	if (protect)
		asm("fclr i\n");
	else
		asm("fset i\n");
}

/* ************************************************************************** */

/**
 * \brief M16C Is-Protected function.
 *
 * Returns the current state, non-zero protected, zero protected.
 *
 * \return non-zero if protected mode, otherwise unprotected.
 */
static inline int m16c_isprotected(void) {
	int tmp;
	asm("stc flg, %0\n" : "=r" (tmp));
	return !(tmp & 0x40);
}

/* ************************************************************************** */

/**
 * \brief M16C Timer get function.
 *
 * \return The current time.
 */
static inline env_time_t m16c_timer_get(void) {
	extern env_time_t m16c_timer_base;
	return m16c_timer_base + (0xffff - TA0);
}

/* ************************************************************************** */

/**
 * \brief M16C Envrionment timer timestamp function.
 *
 * \return The timestamp of the previous interrupt.
 */
static inline env_time_t m16c_timestamp(void) {
	extern env_time_t m16c_timer_timestamp;
	return m16c_timer_timestamp;
}

#endif
