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
 * \brief The AVR stack.
 */
char avr_stack[AVR_STACKSIZE] = {0};

/* ************************************************************************** */

/** \cond */

/**
 * \brief Starting time for the timer, used for testing mainly.
 */
#define AVR_TIME_START	0x70000000

/* ************************************************************************** */

/**
 * \brief The offset of the first free stack byte.
 */
size_t avr_stack_offset = AVR_STACKSIZE-ENV_STACKSIZE_IDLE;

/* ************************************************************************** */

/**
 * \brief The AVR base time.
 */
env_time_t avr_timer_base = AVR_TIME_START;

/* ************************************************************************** */

/**
 * \brief The AVR next interrupt time.
 */
env_time_t avr_timer_next = 0;

/* ************************************************************************** */

/**
 * \brief The AVR timestamp.
 */
env_time_t avr_timer_timestamp = AVR_TIME_START;

/* ************************************************************************** */

/**
 * \brief The AVR timer active flag.
 */
char avr_timer_active = 0;

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
__attribute__((naked)) void avr_interrupt_return(void)
{
	__asm__ __volatile__ ("reti");
}

/* ************************************************************************** */

/* 
 * Helper function for the pseudo-return of CONTEXT_(SAVE|RESTORE)
 */
__attribute__((naked)) void avr_normal_return(void)
{
	__asm__ __volatile__ ("ret");
}

/** \endcond */

/* ************************************************************************** */

/**
 * \brief The AVR init function.
 *
 * Sets up the RS-232 and timer.
 */
void avr_init(void)
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
 * \brief The AVR print function.
 *
 * Simply prints a string to the first RS-232 port.
 *
 * \param str The string to print.
 */
void avr_print(const char *str)
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
 * \brief The AVR panic function.
 * 
 * Will print the string and power down the unit.
 *
 * \param str The string to print.
 */
void avr_panic(const char *str)
{
	avr_protect(1);
	avr_print(str);
	for (;;);
	{
		sleep_enable();
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();
	}
}

/* ************************************************************************** */

/**
 * \brief The AVR context init function.
 *
 * Will initialize a non-idle context with the given function.
 *
 * \param context The context to initialize.
 * \param stacksize The size of the stack.
 * \param function The function to use.
 */
void avr_context_init(
		avr_context_t *context,
		size_t stacksize,
		tt_thread_function_t function
		)
{
	unsigned char i;

	/* Make sure we still have some stack left. */
	if (avr_stack_offset < stacksize)
		avr_panic("avr_context_init(): Out of stack space.\n");


	/* Setup the magic cookie. */
	context->sp = &avr_stack[avr_stack_offset-1];
	context->cookie = (void *)&avr_stack[avr_stack_offset-stacksize];
	*context->cookie = AVR_CONTEXT_COOKIE;

	/* Setup the sp and return path. */
	*context->sp-- = (unsigned char)(unsigned short)function & 0xff;
	*context->sp-- = (unsigned char)((unsigned short)function >> 8) & 0xff;
	for (i=0;i<33;i++)
		*context->sp-- = 0x00;

	/* Update the stack offset. */
	avr_stack_offset -= stacksize;
}

/* ************************************************************************** */

/**
 * \brief AVR context cookie panic.
 *
 * Used in the context_(save|restore) macros.
 */
void avr_context_cookie_panic(void)
{
	avr_panic("Context cookie corrupted.\n");
}

/* ************************************************************************** */

/**
 * \brief AVR context dispatch function.
 *
 * Should save the current context and restore the new context.
 */
__attribute__((naked)) void avr_context_dispatch(avr_context_t *context)
{
	/* Save current context. */
	AVR_CONTEXT_SAVE(avr_normal_return);

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
	AVR_CONTEXT_RESTORE();
}

/* ************************************************************************** */

/**
 * \brief The AVR idle function.
 *
 * Will place the environment in an idle state.
 */
void avr_idle(void)
{
	/* Check for idle stack overflow and set the context cookie.*/
	if (SP < (unsigned short)&avr_stack[AVR_STACKSIZE-ENV_STACKSIZE_IDLE])
		avr_panic("avr_idle(): Idle stack overflow during init.\n");

	/* Reset the sp to the top of the stack, we don't ever plan to return.  */
	SP = (unsigned short)&avr_stack[AVR_STACKSIZE-1];
	/* TODO: Fix the Y register as frame pointer... */

	tt_current->cookie = (void *)&avr_stack[AVR_STACKSIZE-ENV_STACKSIZE_IDLE];
	*tt_current->cookie = AVR_CONTEXT_COOKIE;

	/* Leave protected mode before we go to sleep. */
	avr_protect(0);

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
		sei();
		sleep_mode();
	}
}

/* ************************************************************************** */

/**
 * \brief The AVR epoch schedule function.
 *
 * Will take care of any interrupt that should be generated during this
 * epoch.
 */
static void avr_timer_epoch_schedule(void)
{
	signed long diff;

	/* First of all make sure that the timer is "active". */
	if (!avr_timer_active)
		return;

	/* Always disable the compare interrupt. */
	OCR1AOFF();

	/* 
	 * Figure out if an interrupt has occured or will occur during
	 * this epoch.
	 */
	diff = avr_timer_next - avr_timer_base;
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
 * \brief The AVR timer set function.
 *
 * Will set the new next time for the environment and also schedule any
 * interrupt with epoch_schedule().
 *
 * \param next When the next baseline expires.
 */
void avr_timer_set(env_time_t next)
{
	avr_timer_next = next;
	avr_timer_active = 1;
	avr_timer_epoch_schedule();
}

/* ************************************************************************** */

/**
 * \brief The AVR overflow ISR.
 *
 * Will add ENV_TIMER_COUNT to the avr_timer_base value.
 */
ISR(TIMER1_OVF_vect)
{
	avr_timer_base += ENV_TIMER_COUNT;
	avr_timer_epoch_schedule();
}

/* ************************************************************************** */

/**
 * \brief The AVR ouput compare ISR.
 *
 * This is used to generate any interrupt/scheduling to the tinyTimber kernel.
 */
__attribute__ ((signal, used, naked)) void TIMER1_COMPA_vect(void)
{
	/* Save current context. */
	AVR_CONTEXT_SAVE(avr_interrupt_return);

	/* Disable timer and interrupt. */
	avr_timer_active = 0;
	OCR1AOFF();

	/* Notify the kernel that a baseline expired. */
	tt_expired(avr_timer_get());

	/* Schedule any new messages. */
	tt_schedule();

	/* Restore old or new context context. */
	AVR_CONTEXT_RESTORE();
}
