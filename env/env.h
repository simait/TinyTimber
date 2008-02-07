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

#ifndef ENV_H_
#define ENV_H_

#if ! defined TT_MANGLED
#	if defined ENV_POSIX
#		include "posix/env.h"
#	elif defined ENV_POSIX_SRP
#		include "posix_srp/env.h"
#	elif defined ENV_MSP430
#		include "msp430/env.h"
#	elif defined ENV_MSP430_SRP
#		include "msp430_srp/env.h"
#	elif defined ENV_AVR5
#		include "avr5/env.h"
#	elif defined ENV_PIC18
#		include "pic18/env.h"
#	elif defined ENV_ARM7
#		include "arm7/env.h"
#	elif defined ENV_MIPS
#		include "mips/env.h"
#	elif defined ENV_M16C
#		include "m16c/env.h"
#	elif defined ENV_M16C_SRP
#		include "m16c_srp/env.h"
#	elif defined ENV_SKEL
#		include "skel/env.h"
#	else
#		error Unknown environment.
#	endif
#endif

/* ************************************************************************** */
/* ********************************** DEFAULT ******************************* */
/* ************************************************************************** */

/**
 * \brief Environment inline macro.
 *
 * If the environment did not supply a definition for this then default to
 * nothing. Not all environments allow for the inline keyword(it's C99 not C89).
 */
#ifndef ENV_INLINE
#	define ENV_INLINE
#endif

/**
 * \brief Environment priority reset macro.
 *
 * Not always required by the environment so should evaluate to a void
 * expression if not defined.
 */
#ifndef ENV_INTERRUPT_PRIORITY_RESET
#	define ENV_INTERRUPT_PRIORITY_RESET() ((void)0)
#endif

/**
 * \brief Environment fast code macro.
 *
 * If the environment does not supply a defenition for this the default is
 * nothing. This really only makes sense on the arm7 where we want to force
 * the linking of the functions into SRAM since it's the only thing that is
 * read in 1 cycle.
 */
#ifndef ENV_CODE_FAST
#	define ENV_CODE_FAST
#endif

/**
 * \brief Environment debug macro.
 *
 * The debug macro is NOT neccesary but can be helpfull.
 */
  
#ifndef ENV_DEBUG
#	define ENV_DEBUG(msg) ((void)0)
#endif

/* ************************************************************************** */
/* *********************************** SANITY ******************************* */
/* ************************************************************************** */

#ifndef ENV_INIT
#	error Environment did not define ENV_INIT().
#endif

#ifndef ENV_PROTECT
#	error Environment did not define ENV_PROTECT().
#endif

#ifndef ENV_ISPROTECTED
#	error Environment did not define ENV_PROTECTED().
#endif

#ifndef ENV_PANIC
#	error Environment did not define ENV_PANIC().
#endif

#ifndef ENV_IDLE
#	error Environment did not define ENV_IDLE()
#endif

#ifndef ENV_TIMER_START
#	error Environment did not define ENV_TIMER_START().
#endif

#ifndef ENV_TIMER_SET
#	error Environment did not define ENV_TIMER_SET().
#endif

#ifndef ENV_TIMER_GET
#	error Environment did not define ENV_TIMER_GET().
#endif

#ifndef ENV_TIMESTAMP
#	error Environment did not define ENV_TIMESTAMP().
#endif

#ifndef ENV_USEC
#	error Environment did not define ENV_USEC().
#endif

#ifndef ENV_MSEC
#	error Environment did not define ENV_MSEC().
#endif

#ifndef ENV_SEC
#	error Environment did not define ENV_SEC().
#endif

#ifndef ENV_STARTUP
#	error Environment did not define ENV_STARTUP().
#endif

/* ************************************************************************** */
/* ***************************** SANITY (NON-SRP) *************************** */
/* ************************************************************************** */

#if defined ENV_SRP

#	ifdef ENV_NUM_THREADS
#		warning Environment defined ENV_NUM_THREADS?!?
#	endif

#	ifdef ENV_CONTEXT_INIT
#		warning Environment defined ENV_CONTEXT_INIT?!?
#	endif

#	ifdef ENV_CONTEXT_DISPATCH
#		warning Environment defined ENV_CONTEXT_DISPATCH?!?
#	endif

#else

#	ifndef ENV_NUM_THREADS
#		error Environment did not define ENV_NUM_THREADS.
#	endif

#	ifndef ENV_CONTEXT_INIT
#		error Environment did not define ENV_CONTEXT_INIT().
#	endif

#	ifndef ENV_CONTEXT_DISPATCH
#		error Environment did not define ENV_CONTEXT_DISPATCH().
#	endif

#endif

#endif
