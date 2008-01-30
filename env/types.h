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
#	elif defined ENV_AVR
#		include "avr5/types.h"
#	elif defined ENV_PIC18
#		include "pic18/types.h"
#	elif defined ENV_ARM7
#		include "arm7/types.h"
#	elif defined ENV_MIPS
#		include "mips/types.h"
#	elif defined ENV_M16C
#		include "m16c/types.h"
#	elif defined ENV_SKEL
#		include "skel/types.h"
#	else
#		error Unknown environment.
#	endif
#	if ! defined ENV_TIME_T
		/*
		 * If env_time_t was not supplied with the environment we'll supply the
		 * default definition as an unsigned long.
		 */

		typedef unsigned long env_time_t;

#		define ENV_TIME_LT(v0, v1) \
			((((signed long)(v0) - (v1))) < 0)

#		define ENV_TIME_LE(v0, v1) \
			((((signed long)(v0) - (v1))) <= 0)

#		define ENV_TIME_ADD(v0, v1) \
			((v0) + (v1))
#	else
		/*
		 * If the nevironment supplies a special time type we'lll try to
		 * check it for errors.
		 */
#		ifndef ENV_TIME_LT
#			error Environment did not define ENV_TIME_LT()
#		endif

#		ifndef ENV_TIME_LE
#			error Environment did not define ENV_TIME_LE()
#		endif

#		ifndef ENV_TIME_ADD
#			error Environment did not define ENV_TIME_ADD()
#		endif
#	endif
#endif

#endif
