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

#include <env.h>
#include <types.h>

#include <kernel.h>

#include <string.h>

#include <gba/gba.h>

/* ************************************************************************** */

/**
 * \cond
 *
 * Any internal crap goes here.
 */

/* ************************************************************************** */

/**
 * End of internal crap is _HERE_.
 *
 * \endcond
 */

/* ************************************************************************** */

/**
 * \brief GBA init function.
 *
 * Should land the processor in a fully usable state.
 */
void gba_init(void)
{
}

/* ************************************************************************** */

/**
 * \brief GBA print function.
 *
 * \param msg The message that should be printed.
 */
void gba_print(const char *msg)
{
	ARM7_BREAK();
}

/* ************************************************************************** */

/**
 * \brief GBA panic function.
 *
 * \param msg The message to print.
 */
void gba_panic(const char *msg)
{
	arm7_protect(1);
	ARM7_BREAK();
	/*gba_print(msg);*/

	/* Stop _ALL_ system clocks. */
#if 0
	AT91C_BASE_PMC->PMC_SCDR =
		AT91C_PMC_PCK |
		AT91C_PMC_PCK0 |
		AT91C_PMC_PCK1 |
		AT91C_PMC_PCK2 |
		AT91C_PMC_UDP;
#else
	for (;;);
#endif
}

/* ************************************************************************** */

/**
 * \brief GBA idle function.
 *
 * Initializes the tt_current(idle thread) context and enters an
 * idle state if nescessary.
 */
void gba_idle(void)
{
	unsigned int sp;

	/* Make sure we haven't overrun the stack. */
	asm volatile("mov	%0, sp\n" : "=r" (sp));
	if (sp <= (unsigned int)&arm7_stack[ARM7_STACKSIZE-ENV_STACKSIZE_IDLE])
		gba_panic("arm7_idle(): Stack overflow.\n");

	/* Reset the sp to the top of the stack, we don't ever plan to return.  */
	asm volatile("mov	%0, sp\n" :: "r" (&arm7_stack[ARM7_STACKSIZE]));

	/* Setup the current context. */
	tt_current->cookie = (void *)&arm7_stack[ARM7_STACKSIZE-ENV_STACKSIZE_IDLE];
	*tt_current->cookie = ARM7_CONTEXT_COOKIE;

	/* Leave protected mode. */
	arm7_protect(0);

	for (;;);
}

/* ************************************************************************** */

/**
 * \brief GBA timer set function.
 *
 * Sets the "time" when the next baseline expires.
 *
 * \param next The "time" when the next baseline expires.
 */
void gba_timer_set(env_time_t next)
{
}

/* ************************************************************************** */

void gba_timer_start(void)
{
}

/* ************************************************************************** */

env_time_t gba_timer_get(void)
{
	return 0;
}

/* ************************************************************************** */

env_time_t gba_timestamp(void)
{
	return 0;
}
