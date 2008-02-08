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
 * \brief Skeleton environment.
 *
 * Just used for compile testing and perhaps as a template for a new
 * environment. Should compile but does not run.
 */
#ifndef ENV_ARM7_ENV_H_
#define ENV_ARM7_ENV_H_

#include <arm7/types.h>
#include <arm7/sys_call.h>
#include <arm7/at91sam7x256.h>
#include <arm7/lib_at91sam7x256.h>

#include <kernel.h>

/* ************************************************************************** */

void arm7_init(void);
void arm7_panic(const char *);
void arm7_idle(void);
void arm7_context_init(arm7_context_t *, size_t, tt_thread_function_t);
void arm7_context_dispatch(env_context_t *);
void _arm7_context_dispatch(env_context_t *);
void arm7_schedule(void);
void _arm7_schedule(void);
void arm7_timer_set(env_time_t);

#if defined __thumb__
int _arm7_isprotected(void);
void _arm7_protect(int);
#endif

/* ************************************************************************** */

/**
 * \brief ARM7 context cookie.
 */
#define ARM7_CONTEXT_COOKIE 0x55aa55aa

/* ************************************************************************** */

/**
 * \brief Environment initialization.
 *
 * Should take care of initializing the environment setting up any hardware
 * required by the environment such as serial ports, timers etc.
 */
#define ENV_INIT() \
	arm7_init()

/* ************************************************************************** */

/**
 * \brief Environemnt debug macro.
 *
 * May print the string pointer to by msg but it is not required.
 */
#define ENV_DEBUG(msg) \
	arm7_print(msg)

/* ************************************************************************** */

/**
 * \brief Environment panic macro.
 *
 * Must indicate that a critical error has occured and halt or reset the
 * execution.
 */
#define ENV_PANIC(msg) \
	arm7_panic(msg)

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
 * \brief Environment idle macro.
 *
 * Should place the environment in an "idle" state. Can be busy waiting or
 * some power-saving mode. The environment must initialize the context pointerd
 * to by tt_current so that the kernel can dispatch another thread.
 */
#define ENV_IDLE() \
	arm7_idle()

/* ************************************************************************** */

/**
 * \brief Environment timer start macro.
 *
 * Should start the environment timer service.
 */
#define ENV_TIMER_START() \
	AT91C_BASE_TCB->TCB_TC0.TC_CCR = AT91C_TC_SWTRG

/* ************************************************************************** */

/**
 * \brief Environment timer set macro.
 *
 * Should set the time when tt_expired(time) should be called.
 */
#define ENV_TIMER_SET(time) \
	arm7_timer_set(time)

/* ************************************************************************** */

/**
 * \brief Environment timer get macro.
 *
 * Should evaluate to the current time.
 */
#define ENV_TIMER_GET() \
	arm7_timer_get()

/* ************************************************************************** */

/**
 * \brief Environment timestamp macro.
 *
 * Should evaluate to the time when the most recent interrupt was triggered.
 */
#define ENV_MARK() \
	arm7_timer_mark()

/* ************************************************************************** */

/**
 * \brief Environment timestamp macro.
 *
 * Should evaluate to the time when the most recent interrupt was triggered.
 */
#define ENV_TIMESTAMP() \
	arm7_timestamp()

/* ************************************************************************** */

/**
 * \brief Environment inline macro.
 *
 * Used to tag functions mainly in the tT kernel for inlining.
 */
#define ENV_INLINE \
	inline

/* ************************************************************************** */

/**
 * \brief The main clock frequency of the AT91SAM7X256.
 *
 * Used to calculate ARM7_MASTER_CLOCK.
 */
#define ARM7_MAIN_CLOCK 18432000UL

/* ************************************************************************** */

/**
 * \brief ARM7 PPL multiplier.
 *
 * Used to calculate ARM7_MASTER_CLOCK.
 */
#define ARM7_PLL_MUL 72

/* ************************************************************************** */

/**
 * \brief ARM7 PLL Fivider.
 *
 * Used to calculate ARM7_MASTER_CLOCK.
 */
#define ARM7_PLL_DIV 14

/* ************************************************************************** */

/**
 * \brief Master clock frquency.
 *
 * The PLL clock output is:
 *
 * PLL_CLOCK = ((ARM7_MAIN_CLOCK/ARM7_PLL_DIV)*ARM7_PLL_MUL+1)
 *
 * These values will give us a PLL_CLOCK of ~96025764 which is close
 * enough to 96Mhz. We will use a USB divider of 2 to get within the +/-0.25%
 * error for full speed USB. We will also use the PLL divided by 2 as the
 * master clock.
 */
#define ARM7_MASTER_CLOCK (((ARM7_MAIN_CLOCK/ARM7_PLL_DIV)*(ARM7_PLL_MUL+1))/2)

/* ************************************************************************** */

/**
 * \brief ARM7 Timer frequency.
 *
 * The frequency the timer will be running at.
 */
#define ENV_TIMER_HZ (ARM7_MASTER_CLOCK/1024)

/* ************************************************************************** */

/**
 * \brief Environment usec macro.
 *
 * The given number of usecs should be converted to time-base of
 * the environment.
 */
#define ENV_USEC(n) ((n*ENV_TIMER_HZ)/100000)

/* ************************************************************************** */

/**
 * \brief Environment msec macro.
 *
 * The given number of msecs should be converted to time-base of
 * the environment.
 */
#define ENV_MSEC(n) ((n*ENV_TIMER_HZ)/1000)

/* ************************************************************************** */

/**
 * \brief Environment sec macro.
 *
 * The given number of secs should be converted to time-base of
 * the environment.
 */
#define ENV_SEC(n) (n*ENV_TIMER_HZ)

/* ************************************************************************** */

/**
 * \brief ARM7 number of threads.
 */
#define ENV_NUM_THREADS	5

/* ************************************************************************** */

/**
 * \brief ARM7 stack size.
 */
#define ENV_STACKSIZE 128

/* ************************************************************************** */

/**
 * \brief ARM7 idle stack size.
 */
#define ENV_STACKSIZE_IDLE 128

/* ************************************************************************** */

/**
 * \brief Environment startup macro.
 *
 * Should make the environment run the given function upon startup and/or
 * reset.
 */
#define ENV_STARTUP(function) \
int main(void)\
{\
	extern const unsigned int * const arm7_stack_start;\
	asm(\
		"ldr	r0, %0\n"\
		"ldr	sp, [r0]\n"\
	   :: "m" (arm7_stack_start)\
	   );\
	tt_init();\
	function();\
	tt_run();\
	return 0;\
} extern char dummy /* Force semi-colon. */

/* ************************************************************************** */

/**
 * \brief ARM7 environment extension to force something into the ram.
 *
 * Usually used for interrupt functions and any code that must be in ARM
 * mode and run at full speed.
 */
#define ENV_EXT_FORCE_RAM __attribute__((section(".forced_ram")))

/* ************************************************************************** */

/**
 * \brief ARM7 environment fast call.
 *
 * Just force it into the ram with ENV_EXT_FORCE_RAM.
 */
#define ENV_CODE_FAST ENV_EXT_FORCE_RAM


/* ************************************************************************** */

/**
 * \brief ARM7 stack size(total).
 */
#define ARM7_STACKSIZE (ENV_NUM_THREADS*ENV_STACKSIZE+ENV_STACKSIZE_IDLE)

/* ************************************************************************** */

/**
 * \brief ARM7 print function.
 *
 * \param msg The message to print.
 */
static inline void arm7_print(const char *msg)
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

/* ************************************************************************** */

/**
 * \brief ARM7 timer get function.
 *
 * \return The current time.
 */
static inline env_time_t arm7_timer_get(void)
{
	extern env_time_t arm7_timer_base;
	return arm7_timer_base + AT91C_BASE_TC0->TC_CV;
}

/* ************************************************************************** */

/**
 * \brief ARM7 timer timestamp function.
 *
 * \return The timestamp of the previous interrupt.
 */
static inline void arm7_timer_mark(void)
{
	extern env_time_t arm7_timer_base;
	extern env_time_t arm7_timer_timestamp;
	arm7_timer_timestamp = arm7_timer_base + AT91C_BASE_TC0->TC_CV;
}

/* ************************************************************************** */

/**
 * \brief ARM7 timer timestamp function.
 *
 * \return The timestamp of the previous interrupt.
 */
static inline env_time_t arm7_timestamp(void)
{
	extern env_time_t arm7_timer_timestamp;
	return arm7_timer_timestamp;
}

#endif
