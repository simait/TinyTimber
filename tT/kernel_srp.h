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
 * \file
 * \brief The TinyTimber Kernel environment interface (SRP).
 *
 * If possible avoid exporting this interface to the user via the environment.
 */

#ifndef KERNEL_H_
#define KERNEL_H_

/*
 * Following files should not be included in case the file is mangled.
 */
#if ! defined TT_MANGLED
#	include <tT.h>
#	include <types.h>
#endif

/*
 * Just so that the user is aware that he gone did something stupid.
 */
#if ! defined ENV_SRP
#	error Compiling SRP TinyTimber against regular environment.
#endif

/* ************************************************************************** */

/**
 * \brief TinyTimber message typedef.
 */
typedef struct tt_message_t tt_message_t;

/* ************************************************************************** */

void tt_expired(env_time_t);

/* ************************************************************************** */

#ifndef TT_NUM_MESSAGES
	/**
	 * \brief The number of messages that the kernel facillitates.
	 */
#	define TT_NUM_MESSAGES	10
#endif

/* ************************************************************************** */

#ifndef TT_ARGS_SIZE
	/**
	 * \brief The size of the variable argument buffer.
	 */
#	define TT_ARGS_SIZE		8
#endif

/* ************************************************************************** */

#ifdef TT_KERNEL_SANITY
	/** \cond */
#	define _STR(str) #str
#	define STR(str) _STR(str)
	/** \endcond */
	
	/**
	 * \brief The kernel sanity macro.
	 *
	 * Should only be used for debugging purposes. Used whenever
	 * TT_KERNEL_SANITY is defined. Will fail whenever the expression
	 * supplied is not evaluated as true.
	 */
#	define TT_SANITY(expr) \
		if (!(expr))\
			ENV_PANIC(\
					__FILE__ ":" STR(__LINE__) " ("\
					#expr ") failed sanity check.\n"\
					)
#else
#	define TT_SANITY(expr)
#endif

#endif
