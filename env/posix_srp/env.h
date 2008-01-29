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
void posix_srp_panic(const char * const);
//void posix_srp_timer_start(void);
//void posix_srp_timer_set(const env_time_t *);
//env_time_t posix_srp_timer_get(void);
//env_time_t posix_srp_timestamp(void);
void posix_srp_protect(int);
int posix_srp_isprotected(void);
void posix_srp_idle(void);

/* ************************************************************************** */

#define ENV_INLINE inline

/* ************************************************************************** */

#define ENV_CODE_FAST

/* ************************************************************************** */

#define ENV_INIT() \
	posix_srp_init()

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

#define ENV_TIME_ADD(v0, v1) \
	posix_srp_time_add(&v0, &v1)

/* ************************************************************************** */

#define ENV_TIME_LE(v0, v1) \
	(((v0).tv_sec <= (v1).tv_sec) && ((v0).tv_nsec <= (v1).tv_nsec))

/* ************************************************************************** */

#define ENV_TIME_LT(v0, v1) \
	(((v0).tv_sec < (v1).tv_sec) || (((v0).tv_sec == (v1).tv_sec) && ((v0).tv_nsec < (v1).tv_nsec)))

/* ************************************************************************** */

#define ENV_PROTECT(state) \
	posix_srp_protect(state)

/* ************************************************************************** */

#define ENV_ISPROTECTED() \
	posix_srp_isprotected()

/* ************************************************************************** */

#define ENV_RESOURCE(id) \
	(1<<id)

/* ************************************************************************** */

#define ENV_RESOURCE_AQUIRE(mask, resource) \
	do {mask |= resource;} while (0)

/* ************************************************************************** */

#define ENV_RESOURCE_RELEASE(mask, resource) \
	do {mask &= ~(resource);} while (0)

/* ************************************************************************** */

#define ENV_RESOURCE_AVAILABLE(mask, resource) \
	(!((mask) & (resource)))

/* ************************************************************************** */

#define ENV_RESOURCE_MAX \
	(sizeof(env_resource_t)*CHAR_BIT)

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
} char dummy /* Force semi-colon at end of macro. */

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

static inline env_time_t posix_srp_time_add(
		env_time_t *v0,
		env_time_t *v1
		)
{
	env_time_t tmp;
	if ((v0->tv_nsec + v1->tv_nsec) > 1000000000UL) {
		tmp.tv_sec = v0->tv_sec + v1->tv_sec + 1;
		tmp.tv_nsec = v0->tv_nsec + v1->tv_nsec - 1000000000UL;
	} else {
		tmp.tv_sec = v0->tv_sec + v1->tv_sec;
		tmp.tv_nsec = v0->tv_nsec + v1->tv_nsec;
	}
	return tmp;
}

/* ************************************************************************** */

static inline env_time_t posix_srp_sec(unsigned long seconds)
{
	env_time_t tmp = {.tv_sec = seconds, .tv_nsec = 0};
	return tmp;
}

/* ************************************************************************** */

static inline env_time_t posix_srp_msec(unsigned long useconds)
{
	env_time_t tmp;
	if (useconds >= 1000UL) {
		tmp.tv_sec = useconds / 1000UL;
		tmp.tv_nsec = useconds % 1000UL;
	} else {
		tmp.tv_sec = 0;
		tmp.tv_nsec = useconds;
	}
	return tmp;
}

/* ************************************************************************** */

static inline env_time_t posix_srp_usec(unsigned long nseconds)
{
	env_time_t tmp;
	if (nseconds >= 1000000UL) {
		tmp.tv_sec = nseconds / 1000000UL;
		tmp.tv_nsec = nseconds % 1000000UL;
	} else {
		tmp.tv_sec = 0;
		tmp.tv_nsec = nseconds;
	}
	return tmp;
}

#endif
