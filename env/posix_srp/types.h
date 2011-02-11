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

#ifndef ENV_POSIX_TYPES_H_
#define ENV_POSIX_TYPES_H_

/* Standard C headers. */
#include <signal.h>
#include <stdint.h>
#include <time.h>

/* ************************************************************************** */

/**
 * \brief Typedef so that TinyTimber knows what a result is.
 */
typedef uintptr_t env_result_t;

/* ************************************************************************** */

/**
 * \brief POSIX interrupt handler.
 */
typedef void (*posix_ext_interrupt_handler_t)(int);

/* ************************************************************************** */

/**
 * \brief POSIX SRP Uses special env_time_t type.
 *
 * struct timespec is used for time instead of the standard unsigned long.
 */
#define ENV_TIME_T 1

/* ************************************************************************** */

/**
 * \brief POSIX env_time_t specific type, not standard.
 */
typedef struct timespec env_time_t;

/* ************************************************************************** */

/**
 * \brief Environments time less than macro.
 */
#define ENV_TIME_LT(v0, v1) \
	(\
	 ((v0).tv_sec < (v1).tv_sec) ||\
	 (((v0).tv_sec == (v1).tv_sec) && ((v0).tv_nsec < (v1).tv_nsec))\
	 )

/* ************************************************************************** */

/**
 * \brief Environments time less than or equal to macro.
 */
#define ENV_TIME_LE(v0, v1) \
	(\
	 ((v0).tv_sec <= (v1).tv_sec) &&\
	 ((v0).tv_nsec <= (v1).tv_nsec)\
	 )

/* ************************************************************************** */

/**
 * \brief Environments time add macro.
 */
#define ENV_TIME_ADD(v0, v1) \
	posix_srp_time_add(&v0, &v1)

/* ************************************************************************** */

/**
 * \brief POSIX time inherit value.
 */
extern env_time_t posix_time_inherit;

/* ************************************************************************** */

/**
 * \brief POSIX time inherit macro.
 *
 * Used to indicate that the time should be inherited.
 */
#define ENV_TIME_INHERIT \
	posix_time_inherit

/* ************************************************************************** */

/**
 * \brief POSIX time inherited macro.
 *
 * Used to check if the time was inherited or not.
 */
#define ENV_TIME_INHERITED(v0) \
	(((v0).tv_sec == 0) && (((v0).tv_nsec) == 0))

/* ************************************************************************** */

/**
 * \brief POSIX env_time_t addition function.
 *
 * Non-standard addition of the time types, should be inlined and/or eliminated
 * whenever you compile your code since it's static inline. Not always true but
 * at least sort of true for GCC (-O1+).
 * 
 * \param v0 Frist value.
 * \param v1 Second value.
 * \return v0 + v1 with some special checks for overflow.
 */
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


#endif
