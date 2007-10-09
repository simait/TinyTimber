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

/* POSIX/UNIX headers. */
#include <pthread.h>

/**
 * \brief The internal context of a posix thread.
 */
typedef struct posix_context_t
{
	/**
	 * \brief The actual thread.
	 */
	pthread_t thread;

	/**
	 * \brief The context lock.
	 */
	pthread_mutex_t lock;

	/**
	 * \brief The signal condition variable.
	 */
	pthread_cond_t signal;

	/**
	 * \brief The state of the thread.
	 */
	sig_atomic_t run;

	/**
	 * \brief Pointer to the thread function to be run.
	 */
	void (*function)(void);
} posix_context_t;

/**
 * \brief Typedef so that tinyTimber knows what to use as context.
 */
typedef struct posix_context_t env_context_t;

/**
 * \brief Typedef so that tinyTimber knows that time is.
 */
typedef unsigned long env_time_t;

/**
 * \brief Typedef so that tinyTimber knows what a result is.
 */
typedef uintptr_t env_result_t;

/**
 * \brief POSIX interrupt handler.
 */
typedef void (*posix_ext_interrupt_handler_t)(int);

#endif
