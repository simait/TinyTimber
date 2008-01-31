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

#ifndef POSIX_SRP_ENV_H_
#define POSIX_SRP_ENV_H_

#include <stdio.h>
#include <stddef.h>
#include <limits.h>

#include <types.h>

/* ************************************************************************** */

void posix_srp_init(void);
void posix_srp_print(const char * const);
void posix_srp_panic(const char * const);
void posix_srp_protect(int);
void posix_srp_idle(void);

/* ************************************************************************** */

#define ENV_SRP 1

/* ************************************************************************** */

#define ENV_INLINE inline

/* ************************************************************************** */

#define ENV_CODE_FAST

/* ************************************************************************** */

#define ENV_INIT() \
	posix_srp_init()

/* ************************************************************************** */

#define ENV_DEBUG(msg) \
	posix_srp_print(msg)

/* ************************************************************************** */

#define ENV_PANIC(msg) \
	posix_srp_panic(msg)

/* ************************************************************************** */

#define ENV_TIMER_START() \
	((void)0)

/* ************************************************************************** */

#define ENV_TIMER_GET() \
	posix_srp_timer_get()

/* ************************************************************************** */

#define ENV_TIMER_SET(time) \
	posix_srp_timer_set(&(time))

/* ************************************************************************** */

#define ENV_TIMESTAMP() \
	posix_srp_timestamp()

/* ************************************************************************** */

#define ENV_PROTECT(state) \
	posix_srp_protect(state)

/* ************************************************************************** */

#define ENV_ISPROTECTED() \
	posix_srp_isprotected()

/* ************************************************************************** */

#define ENV_IDLE() \
	posix_srp_idle()

/* ************************************************************************** */

#define ENV_SEC(seconds) \
	posix_srp_sec(seconds)

/* ************************************************************************** */

#define ENV_MSEC(nseconds) \
	posix_srp_nsec(nseconds)

/* ************************************************************************** */

#define ENV_USEC(useconds) \
	posix_srp_usec(seconds)

/* ************************************************************************** */

#define ENV_STARTUP(function) \
int main(void) {\
	tt_init();\
	function();\
	tt_run();\
	return 0;\
} extern char dummy /* Force semi-colon at end of macro. */

/* ************************************************************************** */

static inline int posix_srp_isprotected(void)
{
	extern int posix_srp_protected;
	return posix_srp_protected;
}

/* ************************************************************************** */

static inline void posix_srp_timer_set(const env_time_t *next)
{
	extern timer_t posix_srp_timer;
	struct itimerspec tmp = {.it_value = *next};
	timer_settime(posix_srp_timer, TIMER_ABSTIME, &tmp, NULL);
}

/* ************************************************************************** */

static inline env_time_t posix_srp_timer_get()
{
	env_time_t tmp;
	clock_gettime(CLOCK_REALTIME, &tmp);
	return tmp;
}

/* ************************************************************************** */

static inline env_time_t posix_srp_timestamp(void)
{
	extern env_time_t posix_srp_timer_timestamp;
	return posix_srp_timer_timestamp;
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
static inline env_time_t posix_srp_sec(unsigned long seconds)
{
	env_time_t tmp = {.tv_sec = seconds, .tv_nsec = 0};
	return tmp;
}

/* ************************************************************************** */

/**
 * \brief POSIX SRP milli-seconds conversion function.
 *
 * \param nseconds The number of milli-seconds.
 * \return The env_time_t representing the number of milli-seconds specified.
 */
static inline env_time_t posix_srp_msec(unsigned long mseconds)
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
 * \brief POSIX SRP micro-seconds conversion function.
 *
 * \param useconds The number of micro-seconds.
 * \return The env_time_t representing the number of micro-seconds specified.
 */
static inline env_time_t posix_srp_usec(unsigned long useconds)
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
