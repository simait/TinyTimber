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

#ifndef ENV_MSP430_SRP_ENV_H_
#define ENV_MSP430_SRP_ENV_H_

/*
 * Following files should not be included in case the file is mangled.
 */
#if ! defined TT_MANGLED
#	include <types.h>
#endif

#include <debug.h>
#include <signal.h>
#include <string.h>
#include <msp430x16x.h>

/* ************************************************************************** */

/**
 * \brief MSP430 SRP is an SRP environment.
 */
#define ENV_SRP 1

/* ************************************************************************** */

void msp430_srp_init(void);
void msp430_srp_print(const char *);
void msp430_srp_print_hex(unsigned long);
void msp430_srp_panic(const char *);
void msp430_srp_idle(void);
void msp430_srp_timer_set(env_time_t);

/* ************************************************************************** */

/**
 * \brief MSP430 inline hint macro.
 *
 * Expands to the inline keyword.
 */
#define ENV_INLINE inline

/* ************************************************************************** */

/**
 * \brief MSP430 init macro.
 */
#define ENV_INIT() \
	msp430_srp_init()

/* ************************************************************************** */

/**
 * \brief MSP430 protect macro.
 *
 * Used to enable/disable protected kernel mode. When passed non-zero enter
 * protected mode otherwise leave protected kernel mode.
 */
#define ENV_PROTECT(state) \
	msp430_srp_protect(state)

/* ************************************************************************** */

/**
 * \brief MSP430 isprotected macro.
 *
 * Returns non-zero if we are in protected mode, otherwise zero.
 */
#define ENV_ISPROTECTED() \
	(!(READ_SR & GIE))


/* ************************************************************************** */

/**
 * \brief MSP430 debug macro.
 */
#define ENV_DEBUG(msg) \
	msp430_srp_print(msg)

/* ************************************************************************** */

/**
 * \brief MSP430 debug variable macro.
 */
#define ENV_DEBUG_VAR(var) \
do {\
	msp430_srp_print(#var ": 0x");\
	msp430_srp_print_hex((unsigned long)(var));\
	msp430_srp_print("\n");\
} while (0)

/* ************************************************************************** */

/**
 * \brief MSP430 panic macro.
 */
#define ENV_PANIC(msg) \
	msp430_srp_panic(msg)

/* ************************************************************************** */

/**
 * \brief MSP430 idle macro.
 *
 * Will place the environment in the "idle" state.
 */
#define ENV_IDLE() \
	msp430_srp_idle()

/* ************************************************************************** */

/**
 * \brief MSP430 timer resolution macro.
 */
#define ENV_TIMER_HZ() \
	0x8000UL /* 32768 */

/* ************************************************************************** */

/**
 * \brief MSP430 start timer macro.
 *
 * Will start running the environment timer.
 */
#define ENV_TIMER_START() \
	msp430_srp_timer_start()

/* ************************************************************************** */

/**
 * \brief MSP430 stop timer macro.
 *
 * Will stop the environment timer, not needed and should probably
 * not be used but supplied for completeness.
 */
#define ENV_TIMER_STOP() \
	msp430_srp_timer_start()

/* ************************************************************************** */

/**
 * \brief MSP430 timer get macro.
 */
#define ENV_TIMER_GET() \
	msp430_srp_timer_get()

/* ************************************************************************** */

/**
 * \brief MSP430 timer set macro.
 */
#define ENV_TIMER_SET(time) \
	msp430_srp_timer_set(time)

/* ************************************************************************** */

/**
 * \brief MSP430 timer timestamp macro.
 */
#define ENV_TIMESTAMP() \
	msp430_srp_timestamp()

/* ************************************************************************** */

/**
 * \brief MSP430 timer usec macro.
 */
#define ENV_USEC(val) \
	(((env_time_t)val*ENV_TIMER_HZ())/1000000UL)

/* ************************************************************************** */

/**
 * \brief MSP430 timer msec macro.
 */
#define ENV_MSEC(val) \
	(((env_time_t)val*ENV_TIMER_HZ())/1000UL)

/* ************************************************************************** */

/**
 * \brief MSP430 timer sec macro.
 */
#define ENV_SEC(val) \
	((env_time_t)val*ENV_TIMER_HZ())

/* ************************************************************************** */

/**
 * \brief The interval of the MSP430 timer.
 */
#define MSP430_SRP_TIMER_COUNT		0x10000UL /* 65536 */

/* ************************************************************************** */

/**
 * \brief
 * 	MSP430 macro to define what function should be run when
 * 	the device is starting.
 */
#define ENV_STARTUP(function) \
int main(void) \
{\
	tt_init();\
	function();\
	tt_run();\
	return 0;\
} extern char dummy

/* ************************************************************************** */

/**
 * \brief MSP430 SRP Interrupt macro.
 */
#define ENV_INTERRUPT(vec, function) \
interrupt(vec) ___##function(void) \
{\
	extern env_time_t msp430_srp_timer_timestamp;\
	msp430_srp_timer_timestamp = msp430_srp_timer_get();\
	function();\
	tt_schedule();\
} extern char dummy


/* ************************************************************************** */

/**
 * \brief inline static function to protect the kernel.
 *
 * \param state If nonzero the enter protected mode, otherwise nop.
 */
static inline void msp430_srp_protect(int state)
{
	if (state) 	{
		dint();
	} else {
		eint();
	}
}

/* ************************************************************************** */

/**
 * \brief The msp430 Timer start function.
 *
 * Will start the timer used for tinyTimber on msp430.
 */
static inline void msp430_srp_timer_start(void)
{
	/* Mode of TimerA should always be Continous. */
	TACTL |= MC_2;
}

/* ************************************************************************** */

/**
 * \brief The msp430 Timer stop function.
 *
 * Will start the timer used for tinyTimber on msp430.
 */
static inline void msp430_srp_timer_stop(void)
{
	/* Clear mode control bits 0 and 1(4 and 5 in reality). */
	TACTL &= ~(MC0|MC1);
}

/* ************************************************************************** */

/**
 * \brief The msp430 Timer get function.
 *
 * Will return the current timer value.
 */
static inline env_time_t msp430_srp_timer_get(void)
{
	extern env_time_t msp430_srp_timer_base;
#if 1
	env_time_t time;

	/* 
	 * The value of the timer is the internal counter value +
	 * the timer counter value. Take overflow into account.
	 */
	time = msp430_srp_timer_base + TAR;
	if (TACTL & TAIFG) {
		time = msp430_srp_timer_base + MSP430_SRP_TIMER_COUNT + TAR;
	}

	return time;
#else
	return msp430_srp_timer_base + TAR;
#endif
}

/* ************************************************************************** */

/**
 * \brief The msp430 Timer timestamp function.
 *
 * Will return the last timestamp.
 */
static inline env_time_t msp430_srp_timestamp(void)
{
	extern env_time_t msp430_srp_timer_timestamp;
	return msp430_srp_timer_timestamp;
}

#endif
