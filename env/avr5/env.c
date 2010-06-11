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

#include <string.h>

#include <avr/io.h>
#include <avr/power.h>

/*
 * Following files should not be included in case the file is mangled.
 */
#if ! defined TT_MANGLED
#	include <env.h>
#	include <types.h>
#	include <kernel.h>
#endif

/* ************************************************************************** */

/**
 * \brief AVR5 stack.
 */
char avr5_stack[AVR5_STACKSIZE] = {0};

/* ************************************************************************** */

/** \cond */

/**
 * \brief Starting time for the timer, used for testing mainly.
 */
#define AVR5_TIME_START	0x70000000

/* ************************************************************************** */

/**
 * \brief The offset of the first free stack byte.
 */
size_t avr5_stack_offset = AVR5_STACKSIZE-ENV_STACKSIZE_IDLE;

/* ************************************************************************** */

/**
 * \brief AVR5 base time.
 */
env_time_t avr5_timer_base = AVR5_TIME_START;

/* ************************************************************************** */

/**
 * \brief AVR5 next interrupt time.
 */
env_time_t avr5_timer_next = 0;

/* ************************************************************************** */

/**
 * \brief AVR5 timestamp.
 */
env_time_t avr5_timer_timestamp = AVR5_TIME_START;

/* ************************************************************************** */

/**
 * \brief AVR5 timer active flag.
 */
char avr5_timer_active = 0;

/* ************************************************************************** */

/**
 * \brief Helper macro to enable the OCR1A interrupt.
 */
#define OCR1AON() \
do\
{\
	TIMSK1 |= (1<<OCIE1A);\
} while (0)

/* ************************************************************************** */

/**
 * \brief Helper macro to disable the OCR1A interrupt.
 */
#define OCR1AOFF() \
do\
{\
	TIMSK1 &= ~(1<<OCIE1A);\
	TIFR1  &= ~(1<<OCF1A);\
} while (0)

/* ************************************************************************** */

/* 
 * Helper function for the pseudo-return of CONTEXT_(SAVE|RESTORE)
 */
__attribute__((naked)) void avr5_interrupt_return(void)
{
	__asm__ __volatile__ ("reti");
}

/* ************************************************************************** */

/* 
 * Helper function for the pseudo-return of CONTEXT_(SAVE|RESTORE)
 */
__attribute__((naked)) void avr5_normal_return(void)
{
	__asm__ __volatile__ ("ret");
}

/** \endcond */

/* ************************************************************************** */

/**
 * \brief AVR5 init function.
 *
 * Sets up the RS-232 and timer.
 */
void avr5_init(void)
{
	/* 
	 * 208 gives 9600 baud at 16MHz fosc and double speed.
	 *
	 * double speed: baud = (fosc/((8*UBRR)+1)
	 * regular speed: baud = (fosc/((16*UBRR)+1)
	 */
	UCSR0A = (1<<U2X);
	UBRR0 = 208;

	/* async, 8 bit data, no parity 1 stop bit. */
	UCSR0B = (1<<TXEN)|(1<<RXEN);
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);

	/*
	 * User Timer1 with a prescale of 256 which gives us 62500 as
	 * frequency. We only need to enable the interrupt atm.
	 */
	TIMSK1 = (1<<TOIE1);
}

/* ************************************************************************** */

/**
 * \brief AVR5 print function.
 *
 * Simply prints a string to the first RS-232 port.
 *
 * \param str The string to print.
 */
void avr5_print(const char *str)
{
	/* Send all but the NULL termination to the RS-232. */
	while (*str)
	{
		while (!(UCSR0A & (1<<UDRE0)));
		UDR0 = *str++;
	}

	/* This gives us nice and defined behaviour. */
	while (!(UCSR0A & (1<<UDRE0)));
}

/* ************************************************************************** */

/**
 * \brief AVR5 panic function.
 * 
 * Will print the string and power down the unit.
 *
 * \param str The string to print.
 */
void avr5_panic(const char *str)
{
	avr5_protect(1);
	avr5_print(str);
	for (;;);
	{
		sleep_enable();
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();
	}
}

/* ************************************************************************** */

/**
 * \brief AVR5 context init function.
 *
 * Will initialize a non-idle context with the given function.
 *
 * \param context The context to initialize.
 * \param stacksize The size of the stack.
 * \param function The function to use.
 */
void avr5_context_init(
		avr5_context_t *context,
		size_t stacksize,
		tt_thread_function_t function
		)
{
	unsigned char i;

	/* Make sure we still have some stack left. */
	if (avr5_stack_offset < stacksize)
		avr5_panic("avr5_context_init(): Out of stack space.\n");


	/* Setup the magic cookie. */
	context->sp = &avr5_stack[avr5_stack_offset-1];
	context->cookie = (void *)&avr5_stack[avr5_stack_offset-stacksize];
	*context->cookie = AVR5_CONTEXT_COOKIE;

	/* Setup the sp and return path. */
	*context->sp-- = (unsigned char)(unsigned short)function & 0xff;
	*context->sp-- = (unsigned char)((unsigned short)function >> 8) & 0xff;
	for (i=0;i<33;i++)
		*context->sp-- = 0x00;

	/* Update the stack offset. */
	avr5_stack_offset -= stacksize;
}

/* ************************************************************************** */

/**
 * \brief AVR5 context cookie panic.
 *
 * Used in the context_(save|restore) macros.
 */
void avr5_context_cookie_panic(void)
{
	avr5_panic("Context cookie corrupted.\n");
}

/* ************************************************************************** */

/**
 * \brief AVR5 context dispatch function.
 *
 * Should save the current context and restore the new context.
 */
__attribute__((naked)) void avr5_context_dispatch(avr5_context_t *context)
{
	/* Save current context. */
	AVR5_CONTEXT_SAVE(avr5_normal_return);

	/* 
	 * Change the context with inline asm since this is the only thing
	 * we can be sure does things the "right" way, -O0 will use the frame
	 * pointer for this and mess things up badly.
	 */
	/*tt_current = context;*/
	__asm__ __volatile__(
		"sts	%0, r24\n"
		"sts	%0+1, r25\n"
		:: "m" (tt_current)
		);

	/* Restore new context. */
	AVR5_CONTEXT_RESTORE();
}

/* ************************************************************************** */

/**
 * \brief AVR5 idle function.
 *
 * Will place the environment in an idle state.
 */
void avr5_idle(void)
{
	/* Check for idle stack overflow and set the context cookie.*/
	if (SP < (unsigned short)&avr5_stack[AVR5_STACKSIZE-ENV_STACKSIZE_IDLE])
		avr5_panic("avr5_idle(): Idle stack overflow during init.\n");

	/* Reset the sp to the top of the stack, we don't ever plan to return.  */
	SP = (unsigned short)&avr5_stack[AVR5_STACKSIZE-1];
	/* TODO: Fix the Y register as frame pointer... */

	tt_current->context.cookie =
		(void *)&avr5_stack[AVR5_STACKSIZE-ENV_STACKSIZE_IDLE];
	*tt_current->context.cookie = AVR5_CONTEXT_COOKIE;

	avr5_print("Entering idle-mode\n");

	/* Leave protected mode before we go to sleep. */
	avr5_protect(0);

	for (;;)
	{
		/*
		 * This is the prefered way of entering sleep mode. The reason for the
		 * cli is simply to make sure we don't miss any interrupts. The
		 * instruction following the sei is guaranteed to be exacuted thus we
		 * will not miss any interrupt.
		 *
		 * One might argue that we should use sleep_enable() and sleep_disable()
		 * but that would only provide a false sense of security since we might
		 * not return to the sleep_disable() instruction for a very long time
		 * since will probably switch task upon an interrupt.
		 */
		cli();
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_enable();
		sei();
		sleep_cpu();
		sleep_disable();
	}
}

/* ************************************************************************** */

/**
 * \brief AVR5 epoch schedule function.
 *
 * Will take care of any interrupt that should be generated during this
 * epoch.
 */
static void avr5_timer_epoch_schedule(void)
{
	signed long diff;

	/* First of all make sure that the timer is "active". */
	if (!avr5_timer_active)
		return;

	/* Always disable the compare interrupt. */
	OCR1AOFF();

	/* 
	 * Figure out if an interrupt has occured or will occur during
	 * this epoch.
	 */
	diff = avr5_timer_next - avr5_timer_base;
	if (diff < ENV_TIMER_COUNT)
	{
		/* If the time already passed then generate ASAP. */
		if (diff <= TCNT1)
			OCR1A = TCNT1 + 1;
		else
			OCR1A = diff;

		/* Enable the compare interrupt again. */
		OCR1AON();

		/* Make sure we didn't miss the interrupt. */
		while (OCR1A <= TCNT1 && !(TIFR1 & (1<<OCF1A)))
			OCR1A = TCNT1 + 1;
	}
}

/* ************************************************************************** */

/**
 * \brief AVR5 timer set function.
 *
 * Will set the new next time for the environment and also schedule any
 * interrupt with epoch_schedule().
 *
 * \param next When the next baseline expires.
 */
void avr5_timer_set(env_time_t next)
{
	avr5_timer_next = next;
	avr5_timer_active = 1;
	avr5_timer_epoch_schedule();
}

/* ************************************************************************** */

/**
 * \brief AVR5 overflow ISR.
 *
 * Will add ENV_TIMER_COUNT to the avr5_timer_base value.
 */
ISR(TIMER1_OVF_vect)
{
	avr5_timer_base += ENV_TIMER_COUNT;
	avr5_timer_epoch_schedule();
}

/* ************************************************************************** */

/**
 * \brief AVR5 ouput compare ISR.
 *
 * This is used to generate any interrupt/scheduling to the tinyTimber kernel.
 */
ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	/* Save context and make sure that the correct return is used. */
	AVR5_CONTEXT_SAVE(avr5_interrupt_return);

	/* 
	 * Timer is no longer active, will become active when the kernel
	 * calls avr5_timer_set().
	 */
	avr5_timer_active = 0;
	OCR1AOFF();
	
	/* Baseline might have expired... */
	tt_expired(avr5_timer_get());
	tt_schedule();

	/* Restore the active context. */
	AVR5_CONTEXT_RESTORE();
}
