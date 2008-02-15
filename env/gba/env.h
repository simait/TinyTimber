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

#include <types.h>
#include <gba/sys_call.h>
#include <gba/arm7.h>

#undef ENV_PANIC

#include <kernel.h>

/* ************************************************************************** */

void gba_init(void);
void gba_panic(const char *);
void gba_idle(void);
void gba_timer_start(void);
void gba_timer_set(env_time_t);
env_time_t gba_timer_get(void);
env_time_t gba_timestamp(void);
void gba_print(const char *);
void gba_panic(const char *);
void gba_schedule(void);
void _gba_schedule(void);
void gba_timer_set(env_time_t);

/* ************************************************************************** */

/**
 * \brief Environment initialization.
 *
 * Should take care of initializing the environment setting up any hardware
 * required by the environment such as serial ports, timers etc.
 */
#define ENV_INIT() \
	gba_init()

/* ************************************************************************** */

/**
 * \brief Environemnt debug macro.
 *
 * May print the string pointer to by msg but it is not required.
 */
#define ENV_DEBUG(msg) \
	gba_print(msg)

/* ************************************************************************** */

/**
 * \brief Environment panic macro.
 *
 * Must indicate that a critical error has occured and halt or reset the
 * execution.
 */
#define ENV_PANIC(msg) \
	gba_panic(msg)

/* ************************************************************************** */

/**
 * \brief Environment idle macro.
 *
 * Should place the environment in an "idle" state. Can be busy waiting or
 * some power-saving mode. The environment must initialize the context pointerd
 * to by tt_current so that the kernel can dispatch another thread.
 */
#define ENV_IDLE() \
	gba_idle()

/* ************************************************************************** */

/**
 * \brief Environment timer start macro.
 *
 * Should start the environment timer service.
 */
#define ENV_TIMER_START() \
	gba_timer_start()

/* ************************************************************************** */

/**
 * \brief Environment timer set macro.
 *
 * Should set the time when tt_expired(time) should be called.
 */
#define ENV_TIMER_SET(time) \
	gba_timer_set(time)

/* ************************************************************************** */

/**
 * \brief Environment timer get macro.
 *
 * Should evaluate to the current time.
 */
#define ENV_TIMER_GET() \
	gba_timer_get()

/* ************************************************************************** */

/**
 * \brief Environment timestamp macro.
 *
 * Should evaluate to the time when the most recent interrupt was triggered.
 */
#define ENV_MARK() \
	gba_timer_mark()

/* ************************************************************************** */

/**
 * \brief Environment timestamp macro.
 *
 * Should evaluate to the time when the most recent interrupt was triggered.
 */
#define ENV_TIMESTAMP() \
	gba_timestamp()

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
 * \brief Environment timer HZ macro.
 *
 * The timer maximum resolution.
 */
#define ENV_TIMER_HZ 1

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
#define ENV_CODE_FAST

#endif
