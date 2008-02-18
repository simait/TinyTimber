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

#ifndef GBA_TIMER_H_
#define GBA_TIMER_H_

#ifndef GBA_INTERNAL_
#	error Do not include gba_timer.h directly, use gba.h.
#endif

typedef struct gba_timer_t
{
	union
	{
		gba_reg32_t	both;
		struct
		{
			gba_reg16_t	cnt;
			gba_reg16_t	ctrl;
		};
	};
} gba_timer_t;

#define GBA_TMR0 ((gba_timer_t*)0x4000100)
#define GBA_TMR1 ((gba_timer_t*)0x4000104)
#define GBA_TMR2 ((gba_timer_t*)0x4000108)
#define GBA_TMR3 ((gba_timer_t*)0x400010c)

/* The difference prescalevalues for the timer. */
#define GBA_TMR_S1		(0<<0)
#define GBA_TMR_S32		(1<<0)
#define GBA_TMR_S256	(2<<0)
#define GBA_TMR_S1024	(3<<0)

/* Count up mode, ie. when tmrX-1 overflow update tmrX. */
#define GBA_TMR_CU		(1<<2)

/* Interrupt / Count enable flags. */
#define GBA_TMR_IEN		(1<<6)
#define GBA_TMR_ENABLE	(1<<7)

/* Interrupt flags in GBA_IE/GBA_IF. */
#define GBA_INT_TMR0	(1<<3)
#define GBA_INT_TMR1	(1<<4)
#define GBA_INT_TMR2	(1<<5)
#define GBA_INT_TMR3	(1<<6)

/* The number of ticks of the counter. */
#define GBA_TMR_COUNT	0x10000
#define GBA_TMR_BITS	0x0ffff

#endif
