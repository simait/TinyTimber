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

int gba_timer_active = 0;
env_time_t gba_timer_base = 0;
env_time_t gba_timer_next = 0;
env_time_t gba_timer_timestamp = 0;

/* ************************************************************************** */

static void gba_timer_epoch_schedule(void)
{
	env_time_t now, tmp;

	if (!gba_timer_active) {
		return;
	}

	tmp = gba_timer_next - gba_timer_base;
	if (ENV_TIME_LE(tmp, GBA_TMR_COUNT)) {
		now = gba_timer_get();
		if (ENV_TIME_LE(gba_timer_next, now)) {
			tmp = 0xffff;
		} else {
			tmp = gba_timer_next - now;
			tmp = GBA_TMR_COUNT - tmp;
		}

		if (tmp & 0xffff0000) {
			gba_panic("Erh? You should really fix your brain.\n");
		}

		GBA_IE |= GBA_INT_TMR3;

		/* Update both ctrl and cnt to force reload at start. */
		GBA_TMR3->both = tmp|((GBA_TMR3->ctrl|GBA_TMR_ENABLE)<<16);
	}
}

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
	/* GBA_TMR2 is continous time counter. */
	GBA_TMR2->cnt = 0;
	GBA_TMR2->ctrl = GBA_TMR_S256|GBA_TMR_IEN;
	GBA_IE |= GBA_INT_TMR2;

	/* GBA_TMR3 is epoch counter. */
	GBA_TMR3->cnt = 0;
	GBA_TMR3->ctrl = GBA_TMR_S256|GBA_TMR_IEN;
	GBA_IE |= GBA_INT_TMR3;

	/* 
	 * Enable interrupts in the master control register, we still block
	 * interrupts in CPSR.
	 */
	GBA_IME = 1;
}

/* ************************************************************************** */

/**
 * \brief GBA print function.
 *
 * Mappy VM style printing, will only appear in the emulator.
 *
 * \param msg The message that should be printed.
 */
void gba_print(const char *msg)
{
	asm volatile (
		"mov	r2, %0\n"
		"ldr	r0, =0xC0DED00D\n"
		"mov	r1, #0\n"
		"and	r0, r0, r0\n"
		:
		: "r" (msg)
		: "r0", "r1", "r2"
		);
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
	gba_print(msg);
	for (;;);
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
 * \brief GBA interrupt function.
 */
void gba_interrupt(void)
{
	/* GBA_TMR3 is during epoch. */
	if (GBA_IF & GBA_INT_TMR3) {
		GBA_IE &= ~GBA_INT_TMR3;
		GBA_IF &= ~GBA_INT_TMR3;
		gba_timer_active = 0;
		tt_expired(gba_timer_next);
		tt_schedule();
	}

	/* GBA_TMR2 handles the continous time, TMR3 is used during epoch. */
	if (GBA_IF & GBA_INT_TMR2) {
		GBA_IF &= ~GBA_INT_TMR2;
		gba_timer_base += GBA_TMR_COUNT;
		if (gba_timer_active) {
			gba_timer_epoch_schedule();
		}
	}

	/* Update the timestamp (after timer handling). */
	gba_timer_timestamp = gba_timer_get();

	/* TODO: handle other interrupts. */
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
	gba_timer_next = next;
	gba_timer_active = 1;
	gba_timer_epoch_schedule();
}

/* ************************************************************************** */

/**
 * \brief GBA timer start function.
 *
 * Starts GBA_TMR2.
 */
void gba_timer_start(void)
{
	GBA_TMR2->ctrl |= GBA_TMR_ENABLE;
}

/* ************************************************************************** */

/**
 * \brief GBA timer get.
 *
 * \return The current time.
 */
env_time_t gba_timer_get(void)
{
	return gba_timer_base + GBA_TMR2->cnt;
}

/* ************************************************************************** */

/**
 * \brief GBA timer timestamp function.
 *
 * \return The timestamp of the last interrupt.
 */
env_time_t gba_timestamp(void)
{
	return gba_timer_timestamp;
}
