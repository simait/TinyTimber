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

#ifndef ENV_TYPES_H_
#define ENV_TYPES_H_

#if ! defined TT_MANGLED
#	if defined ENV_POSIX
#		include "posix/types.h"
#	elif defined ENV_POSIX_SRP
#		include "posix_srp/types.h"
#	elif defined ENV_MSP430
#		include "msp430/types.h"
#	elif defined ENV_MSP430_SRP
#		include "msp430_srp/types.h"
#	elif defined ENV_AVR5
#		include "avr5/types.h"
#	elif defined ENV_PIC18
#		include "pic18/types.h"
#	elif defined ENV_ARM7
#		include "arm7/types.h"
#	elif defined ENV_GBA
#		include "gba/types.h"
#	elif defined ENV_MIPS
#		include "mips/types.h"
#	elif defined ENV_M16C
#		include "m16c/types.h"
#	elif defined ENV_M16C_SRP
#		include "m16c_srp/types.h"
#	elif defined ENV_SKEL
#		include "skel/types.h"
#	else
#		error Unknown environment.
#	endif
#endif

#if defined TT_SRP
#	if ! defined ENV_RESOURCE_T
			/*
			 * Resource definition was not supplied by the environment
			 * so we supply the default definition as an unsigned long.
			 */
#		include <limits.h>
			typedef unsigned long env_resource_t;

#		define ENV_RESOURCE_MAX \
			(sizeof(env_resource_t)*CHAR_BIT)
#		define ENV_RESOURCE(id) \
			(1<<id)

#		define ENV_RESOURCE_AQUIRE(mask, resource) \
			do {mask |= resource;} while (0)

#		define ENV_RESOURCE_RELEASE(mask, resource) \
			do {mask &= ~(resource);} while (0)

#		define ENV_RESOURCE_AVAILABLE(mask, resource) \
			(!((mask) & (resource)))

#	else
			/*
			 * Resource definition was supplied, let's try to error check that
			 * the required interface was supplied.
			 */
#		ifndef ENV_RESOURCE_MAX
#			error Environment did not defined ENV_RESOURCE_MAX.
#		endif

#		ifndef ENV_RESOURCE_AQUIRE
#			error Environment did not defined ENV_RESOURCE_AQUIRE().
#		endif

#		ifndef ENV_RESOURCE_RELEASE
#			error Environment did not defined ENV_RESOURCE_RELEASE().
#		endif

#		ifndef ENV_RESOURCE_AVAILABLE
#			error Environment did not defined ENV_RESOURCE_AVAILABLE().
#		endif
#	endif
#endif

#if ! defined ENV_ARGUMENT_T
	/* If an argument type was not supplied, define it as an integer. */
	typedef int env_argument_t;

#	define ENV_ARGUMENT_COPY(from, to) ((to) = (from))
#endif

#if ! defined ENV_TIME_T
	/*
	 * If env_time_t was not supplied with the environment we'll supply the
	 * default definition as an unsigned long.
	 */

#	include <limits.h>
	typedef unsigned long env_time_t;

#	define ENV_TIME_LT(v0, v1) \
	  (((env_time_t)(v1) - (env_time_t)(v0) - (env_time_t)1) < (env_time_t)LONG_MAX)

#	define ENV_TIME_EQ(v0, v1) \
		((v0) == (v1))

#	define ENV_TIME_LE(v0, v1) \
	  (((env_time_t)(v1) - (env_time_t)(v0)) < (env_time_t)LONG_MAX)


#	define ENV_TIME_GE(v0, v1) \
		(!ENV_TIME_LT(v0, v1) || ENV_TIME_EQ(v0, v1))

#	define ENV_TIME_ADD(v0, v1) \
		((v0) + (v1))

#	define ENV_TIME_INHERIT \
		(0)

#	define ENV_TIME_INHERITED(v0) \
		((v0) == 0ul)
#else
	/*
	 * If the nevironment supplies a special time type we'lll try to
	 * check it for errors.
	 */
#	ifndef ENV_TIME_LT
#		error Environment did not define ENV_TIME_LT()
#	endif

#	ifndef ENV_TIME_LE
#		error Environment did not define ENV_TIME_LE()
#	endif

#	ifndef ENV_TIME_ADD
#		error Environment did not define ENV_TIME_ADD()
#	endif

#	ifndef ENV_TIME_INHERIT
#		error Environment did not define ENV_TIME_INHERIT
#	endif

#	ifndef ENV_TIME_INHERITED
#		error Environment did not define ENV_TIME_INHERITED()
#	endif

#endif

#endif
