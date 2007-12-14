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
 * \brief Skeleton environment types.
 *
 * Just used for compile testing and perhaps as a template for a new
 * environment. Should compile but does not run.
 */
#ifndef ENV_M16C_TYPES_H_
#define ENV_M16C_TYPES_H_

/**
 * \brief M16C Environment context.
 *
 * Structure representing a context.
 */
typedef struct m16c_context_t {
	unsigned char *sp;
	unsigned short *cookie;
} m16c_context_t;

/**
 * \brief M16C Environment context typedef.
 *
 * Should evalutate to the type that is used to store the context of a
 * thread.
 */
typedef m16c_context_t env_context_t;

/**
 * \brief M16C Environment time typedef.
 *
 * Should evaluate to the type that is used to represent time within the
 * environment.
 */
typedef int env_time_t;

/**
 * \brief M16C Environment result typdef.
 *
 * Should evaluate to the result of a synchronus function call.
 */
typedef int env_result_t;

#endif
