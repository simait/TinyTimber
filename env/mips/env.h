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
 * \brief MIPS Environment.
 *
 * Environment, specially for syncsim, NOT for a real MIPS.
 */
#ifndef ENV_MIPS_ENV_H_
#define ENV_MIPS_ENV_H_

void mips_init(void);
void mips_idle(void);
void mips_context_init(mips_context_t *, size_t, tt_thread_function_t);
void mips_context_dispatch(mips_context_t *);
void mips_timer_start(void);

/* ************************************************************************** */

/**
 * \brief Macro that indicates that the context is never saved, we must
 * always invoke ENV_CONTEXT_DISPATCH().
 */
#define ENV_CONTEXT_NOT_SAVED 1

/* ************************************************************************** */

/**
 * \brief MIPS init macro.
 *
 * Shuold leave the environment in a fully usuable state.
 */
#define ENV_INIT() \
	mips_init()

/* ************************************************************************** */

/**
 * \brief MIPS protect macro.
 *
 * Should place the MIPS processor in a protected state.
 */
#define ENV_PROTECT(state) \
	mips_protect(state)

/* ************************************************************************** */

/**
 * \brief MIPS isprotected macro.
 */
#define ENV_ISPROTECTED() \
	mips_isprotected()

/* ************************************************************************** */

/**
 * \brief  MIPS panic macro.
 *
 * Should indicate that something went horribly wrong.
 */
#define ENV_PANIC(msg) \
	mips_panic(msg)

/* ************************************************************************** */

/**
 * \brief MIPS stack size.
 */
#define ENV_STACKSIZE 512

/* ************************************************************************** */

/**
 * \brief MIPS idle stack size.
 */
#define ENV_STACKSIZE_IDLE 512

/* ************************************************************************** */

/**
 * \brief MIPS number of threads.
 */
#define ENV_NUM_THREADS	2

/* ************************************************************************** */

/**
 * \brief MIPS total stack size.
 */
#define MIPS_STACKSIZE (ENV_NUM_THREADS*ENV_STACKSIZE+ENV_STACKSIZE_IDLE)

/* ************************************************************************** */

/**
 * \brief MIPS context init macro.
 *
 * Should render the given context into a usable state.
 */
#define ENV_CONTEXT_INIT(context, stacksize, function) \
	mips_context_init(context, stacksize, function)

/* ************************************************************************** */

/**
 * \brief MIPS context dispatch macro.
 *
 * Should save current context and dispatch the next.
 */
#define ENV_CONTEXT_DISPATCH(context) \
	mips_context_dispatch((mips_context_t *)context)

/* ************************************************************************** */

/**
 * \brief MIPS context cookie, magic number.
 */
#define MIPS_CONTEXT_COOKIE 0x55aa55aa

/* ************************************************************************** */

/**
 * \brief MIPS idle macro.
 *
 * Should enter the idle state, enable itnerrupts etc.
 */
#define ENV_IDLE() \
	mips_idle()

/* ************************************************************************** */

/**
 * \brief MIPS startup macro.
 *
 * Should make the environment run the given function upon startup and/or
 * reset.
 */
#define ENV_STARTUP(function) \
int main(void)\
{\
	tt_init();\
	function();\
	tt_run();\
	return 0;\
} char dummy /* Force semi-colon. */

/* ************************************************************************** */

/**
 * \brief MIPS default interrupt vector.
 */


/* ************************************************************************** */

/**
 * \brief MIPS interrupt id enum.
 */
enum mips_interrupt_id
{
	/** \brief MIPS interrupt INT0. */
	MIPS_INT0 = 0x00,

	/** \brief MIPS interrupt INT1. */
	MIPS_INT1 = 0x01,

	/** \brief MIPS interrupt INT2. */
	MIPS_INT2 = 0x02,

	/** \brief MIPS interrupt INT3. */
	MIPS_INT3 = 0x03,

	/** \brief MIPS interrupt INT4. */
	MIPS_INT4 = 0x04,

	/** \brief MIPS interrupt INT4. */
	MIPS_INT5 = 0x05
};

/* ************************************************************************** */

/**
 * \brief MIPS interrupt install macro.
 */
#define ENV_INTERRUPT(vector, handler) \
do\
{\
	extern void (*mips_interrupt_table)(void)[];\
	mips_interrupt_table[vector] = handler;\
} while (0)

/* ************************************************************************** */

/**
 * \brief MIPS timer start macro.
 *
 * Simply starts the timer.
 */
#define ENV_TIMER_START() \
	mips_timer_start()

/* ************************************************************************** */

/**
 * \brief MIPS timer start get macro.
 *
 * Should return the current time.
 */
#define ENV_TIMER_GET() \
	mips_timer_get()

/* ************************************************************************** */

/**
 * \brief MIPS timer start set macro.
 *
 * Should set the next point in time when we will receive a timer
 * itnerrupt.
 */
#define ENV_TIMER_SET(time) \
	mips_timer_set(time)

/* ************************************************************************** */

/**
 * \brief MIPS timer timerstamp macro.
 *
 * Should return the time when the previous interrupt was handled.
 */
#define ENV_TIMESTAMP() \
	mips_timer_timestamp()

/* ************************************************************************** */

/**
 * \brief MIPS timer resolution macro.
 */
#define ENV_TIMER_HZ() \
	0x8000 /* 32768 */

/* ************************************************************************** */

/**
 * \brief MIPS timer usec macro.
 */
#define ENV_USEC(val) \
	(((env_time_t)val*ENV_TIMER_HZ())/1000000UL)

/* ************************************************************************** */

/**
 * \brief MIPS timer msec macro.
 */
#define ENV_MSEC(val) \
	(((env_time_t)val*ENV_TIMER_HZ())/1000UL)

/* ************************************************************************** */

/**
 * \brief MIPS timer sec macro.
 */
#define ENV_SEC(val) \
	((env_time_t)val*ENV_TIMER_HZ())

/* ************************************************************************** */

/**
 * \brief MIPS protect function, invoked by ENV_PROTECT.
 *
 * \param state zero to leave protected mode, non-zero enter protected mode.
 */
static inline void mips_protect(int state)
{
	int tmp;
	if (state)
	{
		__asm__ __volatile(
			"mfc0	%0, $12\n"
			"andi	%0, %0, 0xfffe\n"
			"mtc0	%0, $12\n"
			: "=r" (tmp)
			);
	}
	else
	{
		__asm__ __volatile (
			"mfc0	%0, $12\n"
			"ori	%0, %0, 1\n"
			"mtc0	%0, $12\n"
			: "=r" (tmp)
			);
	}
}

/* ************************************************************************** */

/**
 * \brief MIPS isprotected function, invoked by ENV_ISPROTECTED().
 */
static inline int mips_isprotected(void)
{
	int tmp;
	__asm__ __volatile (
		"mfc0	%0, $12\n"
		"andi	%0, 1\n"
		: "=r" (tmp)
		);
	return !tmp;
}

/* ************************************************************************** */

/**
 * \brief MIPS timer get function, invoked by ENV_TIMER_GET().
 *
 * \return Returns the current time.
 */
static inline env_time_t mips_timer_get(void)
{
	extern volatile unsigned int TMR_CNT;
	return TMR_CNT;
}

/* ************************************************************************** */

/**
 * \brief MIPS timer timestamp function, invoked by ENV_TIMESTAMP().
 *
 * \return Returns the time when the last interrupt was handled.
 */
static inline env_time_t mips_timer_timestamp(void)
{
	extern env_time_t mips_timestamp;
	return mips_timestamp;
}

#endif
