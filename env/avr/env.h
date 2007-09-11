#ifndef ENV_AVR_ENV_H_
#define ENV_AVR_ENV_H_

#include <tT/kernel.h>

#include <string.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

/* ************************************************************************** */

void avr_init(void);
void avr_print(const char *);
void avr_panic(const char *);

void avr_timer_set(env_time_t);

void avr_context_init(avr_context_t *, size_t, tt_thread_function_t);
void avr_context_dispatch(avr_context_t *);

void avr_interrupt_return(void);
void avr_normal_return(void);

void avr_idle(void);

/* ************************************************************************** */

/**
 * \brief inline is a valid hint in on this target.
 */
#define ENV_INLINE	__inline__

/* ************************************************************************** */

/**
 * \brief The AVR init macro.
 */
#define ENV_INIT() \
	avr_init()

/* ************************************************************************** */

/**
 * \brief The AVR debug macro.
 */
#define ENV_DEBUG(msg) \
	avr_print(msg)

/* ************************************************************************** */

/**
 * \brief The AVR panic macro.
 */
#define ENV_PANIC(msg) \
	avr_panic(msg)

/* ************************************************************************** */

/**
 * \brief The AVR protect macro.
 */
#define ENV_PROTECT(state) \
	avr_protect(state)

/* ************************************************************************** */

/**
 * \brief The AVR isprotected macro.
 */
#define ENV_ISPROTECTED() \
	avr_isprotected()

/* ************************************************************************** */

/**
 * \brief The AVR number of threads.
 */
#define ENV_NUM_THREADS 5

/* ************************************************************************** */

/**
 * \brief The AVR stack size.
 */
#define ENV_STACKSIZE 128

/* ************************************************************************** */

/**
 * \brief The AVR idle stack size.
 */
#define ENV_STACKSIZE_IDLE 128

/* ************************************************************************** */

/**
 * \brief AVR stack size(total).
 */
#define AVR_STACKSIZE (ENV_NUM_THREADS*ENV_STACKSIZE+ENV_STACKSIZE_IDLE)

/* ************************************************************************** */

/**
 * \brief The AVR context init macro.
 */
#define ENV_CONTEXT_INIT(context, stacksize, function) \
	avr_context_init(context, stacksize, function)

/* ************************************************************************** */

/**
 * \brief The AVR dispatch macro.
 */
#define ENV_CONTEXT_DISPATCH(context) \
	avr_context_dispatch((avr_context_t *)(context))\

/* ************************************************************************** */

/**
 * \brief AVR context cookie.
 */
#define AVR_CONTEXT_COOKIE 0x55AA

/* ************************************************************************** */

/**
 * \brief AVR context save macro.
 *
 * Saves the current context.
 */
#define AVR_CONTEXT_SAVE(return_addr) \
	__asm__ __volatile__ (\
		/* First save some storage for the pseudo return.  */\
		"push	r0\n"\
		"push	r0\n"\
		/* Save all general purpose registers. */\
		"push	r0\n"\
		"push	r1\n"\
		"push	r2\n"\
		"push	r3\n"\
		"push	r4\n"\
		"push	r5\n"\
		"push	r6\n"\
		"push	r7\n"\
		"push	r8\n"\
		"push	r9\n"\
		"push	r10\n"\
		"push	r11\n"\
		"push	r12\n"\
		"push	r13\n"\
		"push	r14\n"\
		"push	r15\n"\
		"push	r16\n"\
		"push	r17\n"\
		"push	r18\n"\
		"push	r19\n"\
		"push	r20\n"\
		"push	r21\n"\
		"push	r22\n"\
		"push	r23\n"\
		"push	r24\n"\
		"push	r25\n"\
		"push	r26\n"\
		"push	r27\n"\
		"push	r28\n"\
		"push	r29\n"\
		"push	r30\n"\
		"push	r31\n"\
		/* Save the status register. */\
		"in		r31, %0\n"\
		"push	r31\n"\
		/* \
		 * Save the stack pointer, and while we're at it save the\
		 * pseudo return address. The code for this is a bit of an ugly\
		 * hack but that's life.\
		 */\
		"in		r28, %1\n"\
		"in		r29, %2\n"\
		"ldi	r30, lo8(%5)\n"\
		"ldi	r31, hi8(%5)\n"\
		"std	Y+35, r30\n"\
		"std	Y+34, r31\n"\
		"lds	r30, tt_current\n"\
		"lds	r31, tt_current+1\n"\
		"st		Z, r28\n"\
		"std	Z+1, r29\n"\
		"ldd	r28, Z+2\n"\
		"ldd	r29, Z+3\n"\
		"ld		r26, Y\n"\
		"ldd	r27, Y+1\n"\
		"cpi	r26, %3\n"\
		"brne	.Lcontext_save_fail%=\n"\
		"cpi	r27, %4\n"\
		"brne	.Lcontext_save_fail%=\n"\
		"jmp	.Lcontext_save_ok%=\n"\
		".Lcontext_save_fail%=:\n"\
		"call	avr_context_cookie_panic\n"\
		".Lcontext_save_ok%=:\n"\
		::\
			"I" (_SFR_IO_ADDR(SREG)),\
			"I" (_SFR_IO_ADDR(SPL)),\
			"I" (_SFR_IO_ADDR(SPH)),\
			"M" (AVR_CONTEXT_COOKIE&0xff),\
			"M" ((AVR_CONTEXT_COOKIE>>8)&0xff),\
			"i" (return_addr)\
	)

/* ************************************************************************** */

/**
 * \brief AVR context restore macro.
 *
 * Should restore the current context.
 */
#define AVR_CONTEXT_RESTORE() \
	__asm__ __volatile__ (\
		/* Load the new stack pointer. */\
		"lds	r30, tt_current\n"\
		"lds	r31, tt_current+1\n"\
		"ldd	r28, Z+2\n"\
		"ldd	r29, Z+3\n"\
		"ld		r26, Y\n"\
		"ldd	r27, Y+1\n"\
		"cpi	r26, %3\n"\
		"brne	.Lcontext_restore_fail%=\n"\
		"cpi	r27, %4\n"\
		"brne	.Lcontext_restore_fail%=\n"\
		"jmp	.Lcontext_restore_ok%=\n"\
		".Lcontext_restore_fail%=:\n"\
		"call	avr_context_cookie_panic\n"\
		".Lcontext_restore_ok%=:\n"\
		"ld		r28, Z\n"\
		"ldd	r29, Z+1\n"\
		"out	%1, r28\n"\
		"out	%2, r29\n"\
		/* Restore the status register. */\
		"pop	r31\n"\
		"out	%0, r31\n"\
		/* Restore all general purpose registers. */\
		"pop	r31\n"\
		"pop	r30\n"\
		"pop	r29\n"\
		"pop	r28\n"\
		"pop	r27\n"\
		"pop	r26\n"\
		"pop	r25\n"\
		"pop	r24\n"\
		"pop	r23\n"\
		"pop	r22\n"\
		"pop	r21\n"\
		"pop	r20\n"\
		"pop	r19\n"\
		"pop	r18\n"\
		"pop	r17\n"\
		"pop	r16\n"\
		"pop	r15\n"\
		"pop	r14\n"\
		"pop	r13\n"\
		"pop	r12\n"\
		"pop	r11\n"\
		"pop	r10\n"\
		"pop	r9\n"\
		"pop	r8\n"\
		"pop	r7\n"\
		"pop	r6\n"\
		"pop	r5\n"\
		"pop	r4\n"\
		"pop	r3\n"\
		"pop	r2\n"\
		"pop	r1\n"\
		"pop	r0\n"\
		"ret\n"\
		::\
			"I" (_SFR_IO_ADDR(SREG)),\
			"I" (_SFR_IO_ADDR(SPL)),\
			"I" (_SFR_IO_ADDR(SPH)),\
			"M" (AVR_CONTEXT_COOKIE&0xff),\
			"M" ((AVR_CONTEXT_COOKIE>>8)&0xff)\
	)

/* ************************************************************************** */

/**
 * \brief The AVR idle macro.
 */
#define ENV_IDLE() \
	avr_idle()

/* ************************************************************************** */

/**
 * \brief The AVR timer start macro.
 */
#define ENV_TIMER_START() TCCR1B = 0x04

/* ************************************************************************** */

/**
 * \brief The AVR timer set macro.
 */
#define ENV_TIMER_SET(time) \
	avr_timer_set(time)

/* ************************************************************************** */

/**
 * \brief The AVR timer get macro.
 */
#define ENV_TIMER_GET() \
	avr_timer_get()

/* ************************************************************************** */

/**
 * \brief The AVR timer hz macro.
 */
#define ENV_TIMER_HZ 62500UL

/* ************************************************************************** */

/**
 * \brief The AVR timer count macro.
 */
#define ENV_TIMER_COUNT 0x10000

/* ************************************************************************** */

/**
 * \brief The AVR timestamp macro.
 */
#define ENV_TIMESTAMP() \
	avr_timestamp()

/* ************************************************************************** */

/**
 * \brief The AVR usec macro.
 */
#define ENV_USEC(n) \
	(((n)*ENV_TIMER_HZ)/1000000UL)

/* ************************************************************************** */

/**
 * \brief The AVR msec macro.
 */
#define ENV_MSEC(n) \
	(((n)*ENV_TIMER_HZ)/1000UL)

/* ************************************************************************** */

/**
 * \brief The AVR sec macro.
 */
#define ENV_SEC(n) \
	((n)*ENV_TIMER_HZ)

/* ************************************************************************** */

/**
 * \brief The AVR startup macro.
 */
#define ENV_STARTUP(function) \
__attribute__((__naked__)) int main(void)\
{\
	extern char avr_stack[];\
	SP = (unsigned short)&avr_stack[AVR_STACKSIZE-1];\
	tt_init();\
	function();\
	tt_run();\
	return 0;\
} extern char dummy /* Force semicolon. */

/* ************************************************************************** */

/**
 * \brief The AVR protect function.
 *
 * If state is non-zero then enter protected mode, otherwise leave protected
 * mode.
 *
 * \param state Defines if we should protect or not.
 */
static inline void avr_protect(int state)
{
	if (state)
		cli();
	else
		sei();
}

/* ************************************************************************** */

/**
 * \brief The AVR isprotected function.
 *
 * \return non-zero if protected, zero otherwise.
 */
static inline char avr_isprotected(void)
{
	return !(SREG & (1 << SREG_I));
}

/* ************************************************************************** */

/**
 * \brief The AVR timer get function.
 *
 * Return the current time, takes overflow into account.
 * 
 * \return The current time.
 */
static inline env_time_t avr_timer_get(void)
{
	extern env_time_t avr_timer_base;
#if 0
	env_time_t time;

	time = avr_timer_base + TCNT1;
	if (TIFR1 & (1<<TOV1))
		time = avr_timer_base + ENV_TIMER_COUNT + TCNT1;
	
	return time;
#endif
	return avr_timer_base + TCNT1;
}

/* ************************************************************************** */

/**
 * \brief The AVR timestamp function.
 *
 * \return The timestamp of the most recent interrupt.
 */
static inline env_time_t avr_timestamp(void)
{
	extern env_time_t avr_timer_timestamp;
	return avr_timer_timestamp;
}

#endif
