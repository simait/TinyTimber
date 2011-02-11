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

#ifndef ENV_POSIX_ENV_H_
#define ENV_POSIX_ENV_H_

/* Standard C headers. */
#include <stdio.h>
#include <stddef.h>

/* POSIX/UNIX headers. */
#include <unistd.h>

/* Environment headers. */
#include <types.h>

/* tinyTimber headers. */
#include <tT.h>

/* ************************************************************************** */

void posix_init(void);
void posix_panic(const char * const);
void posix_context_init(posix_context_t *, void (*)(void));
void posix_protect(int);
int  posix_isprotected(void);
void posix_context_dispatch(tt_thread_t *);
void posix_idle(void);

void posix_ext_interrupt_handler(int, posix_ext_interrupt_handler_t);
void posix_ext_interrupt_generate(int);

/* ************************************************************************** */

/**
 * \brief Environemt argument buffer size.
 */
#define ENV_ARGS_BUF_SIZE 64

/* ************************************************************************** */

/**
 * \brief Environment init macro.
 */
#define ENV_INIT() \
	posix_init()

/* ************************************************************************** */

/**
 * \brief Environment protect macro.
 *
 * Used to enable/disable protected kernel mode. When passed non-zero enter
 * protected mode otherwise leave protected kernel mode.
 */
#define ENV_PROTECT(state) \
	posix_protect(state)

/* ************************************************************************** */

/**
 * \brief Environment isprotected macro.
 *
 * Returns non-zero if we are in protected mode, otherwise zero.
 */
#define ENV_ISPROTECTED() \
	posix_isprotected()

/* ************************************************************************** */

/**
 * \brief Environment panic macro.
 *
 * Will print the msg and call abort().
 */
#define ENV_PANIC(msg) \
	posix_panic(msg)

/* ************************************************************************** */

/**
 * \brief Envrionment debug macro.
 *
 * Will print the message.
 */
#define ENV_DEBUG(msg) \
	fprintf(stderr, msg)

/* ************************************************************************** */

/**
 * \brief POSIX Environment context behaviour macro.
 *
 * Tells the TT kernel that the context is not save upon interrupt entry.
 */
#define ENV_CONTEXT_NOT_SAVED 1

/* ************************************************************************** */

/**
 * \brief Environment thread initialization macro.
 *
 * Will initialize a thread context.
 */
#define ENV_CONTEXT_INIT(context, stacksize, function) \
	posix_context_init(context, function)

/* ************************************************************************** */

/**
 * \brief Environment start timer macro.
 *
 * Will start running the environment timer.
 */
#define ENV_TIMER_START() \
	((void)0)

/* ************************************************************************** */

/**
 * \brief Environment thread dispatch macro.
 *
 * Will dispatch the selected thread.
 */
#define ENV_CONTEXT_DISPATCH(thread) \
	posix_context_dispatch((thread))

/* ************************************************************************** */

/**
 * \brief Environment idle macro.
 */
#define ENV_IDLE() \
	posix_idle()

/* ************************************************************************** */

/**
 * \brief The number of thears of this environment.
 */
#define ENV_NUM_THREADS 2

/* ************************************************************************** */

/**
 * \brief Environment timer get macro.
 */
#define ENV_TIMER_GET() \
	posix_timer_get()

/* ************************************************************************** */

/**
 * \brief Environment timer set macro.
 */
#define ENV_TIMER_SET(time) \
	posix_timer_set(&(time))

/* ************************************************************************** */

/**
 * \brief Environment timer timestamp macro.
 */
#define ENV_TIMESTAMP() \
	posix_timestamp()

/* ************************************************************************** */

/**
 * \brief Environment timer usec macro.
 */
#define ENV_USEC(val) \
	posix_usec(val)

/* ************************************************************************** */

/**
 * \brief Environment timer msec macro.
 */
#define ENV_MSEC(val) \
	posix_msec(val)

/* ************************************************************************** */

/**
 * \brief Environment timer sec macro.
 */
#define ENV_SEC(val) \
	posix_sec(val)

/* ************************************************************************** */

/**
 * \brief The POSIX startup macro.
 */
#define ENV_STARTUP(function) \
int main(void)\
{\
	tt_init();\
	function();\
	tt_run();\
	return 0;\
} extern char dummy /* Force the semicolon. */

/* ************************************************************************** */

/**
 * \brief Environment extension to install interrupt handler.
 *
 * Will register the handler for a certain interrupt.
 */
#define ENV_EXT_INTERRUPT_HANDLER(id, handler) \
	posix_ext_interrupt_handler(id, handler)

/* ************************************************************************** */

/**
 * \brief Environment extension to generate an interrupt.
 *
 * Will generate an interrupt for the given interrupt id.
 */
#define ENV_EXT_INTERRUPT_GENERATE(id) \
	posix_ext_interrupt_generate(id)

/* ************************************************************************** */

static inline env_time_t posix_timer_get(void)
{
	env_time_t tmp;
	clock_gettime(CLOCK_REALTIME, &tmp);
	return tmp;
}

/* ************************************************************************** */

/**
 * \brief POSIX set timer function.
 *
 * \param next The next baseline.
 */
static inline void posix_timer_set(const env_time_t *next)
{
	extern timer_t posix_timer;
	struct itimerspec tmp = {.it_value = *next};
	timer_settime(posix_timer, TIMER_ABSTIME, &tmp, NULL);
}

/* ************************************************************************** */

/**
 * \brief POSIX get the timestamp.
 *
 * \return The timestamp of the most recent interrupt.
 */
static inline env_time_t posix_timestamp(void)
{
	extern env_time_t posix_timer_timestamp;
	return posix_timer_timestamp;
}

/* ************************************************************************** */

/**
 * \brief POSIX seconds conversion function.
 *
 * \param seconds The number of seconds.
 * \return The env_time_t representing the number of seconds specified.
 */
static inline env_time_t posix_sec(unsigned long seconds)
{
	env_time_t tmp = {.tv_sec = seconds, .tv_nsec = 0};
	return tmp;
}

/* ************************************************************************** */

/**
 * \brief POSIX milli-seconds conversion function.
 *
 * \param nseconds The number of milli-seconds.
 * \return The env_time_t representing the number of milli-seconds specified.
 */
static inline env_time_t posix_msec(unsigned long mseconds)
{
	env_time_t tmp;
	if (mseconds >= 1000UL) {
		tmp.tv_sec = mseconds / 1000UL;
		tmp.tv_nsec = (mseconds % 1000UL) * 1000UL;
	} else {
		tmp.tv_sec = 0;
		tmp.tv_nsec = mseconds * 1000000UL;
	}
	return tmp;
}

/* ************************************************************************** */

/**
 * \brief POSIX micro-seconds conversion function.
 *
 * \param useconds The number of micro-seconds.
 * \return The env_time_t representing the number of micro-seconds specified.
 */
static inline env_time_t posix_usec(unsigned long useconds)
{
	env_time_t tmp;
	if (useconds >= 1000000UL) {
		tmp.tv_sec = useconds / 1000000UL;
		tmp.tv_nsec = (useconds % 1000000UL) * 1000UL;
	} else {
		tmp.tv_sec = 0;
		tmp.tv_nsec = useconds;
	}
	return tmp;
}

#endif
