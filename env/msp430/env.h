#ifndef ENV_MSP430_ENV_H_
#define ENV_MSP430_ENV_H_

/*
 * Following files should not be included in case the file is mangled.
 */
#if ! defined TT_MANGLED
#	include <types.h>
#endif

#include <signal.h>
#include <debug.h>
#include <string.h>
#include <msp430x16x.h>

/* ************************************************************************** */

void msp430_init(void);
void msp430_print(const char *);
void msp430_panic(const char *);

void msp430_context_init(msp430_context_t *, size_t, void (*)(void));
void msp430_context_dispatch(msp430_context_t *);
void msp430_cookie_panic(void);
void msp430_idle(void);

void msp430_timer_set(env_time_t);

/* ************************************************************************** */

/**
 * \brief MSP430 inline hint macro.
 *
 * Expands to the inline keyword.
 */
#define ENV_INLINE __inline__

/* ************************************************************************** */

/**
 * \brief MSP430 init macro.
 */
#define ENV_INIT() \
	msp430_init()

/* ************************************************************************** */

/**
 * \brief MSP430 protect macro.
 *
 * Used to enable/disable protected kernel mode. When passed non-zero enter
 * protected mode otherwise leave protected kernel mode.
 */
#define ENV_PROTECT(state) \
	msp430_protect(state)

/* ************************************************************************** */

/**
 * \brief MSP430 isprotected macro.
 *
 * Returns non-zero if we are in protected mode, otherwise zero.
 */
#define ENV_ISPROTECTED() \
	(!(READ_SR & GIE))


/* ************************************************************************** */

/**
 * \brief MSP430 debug macro.
 */
#define ENV_DEBUG(msg) \
	msp430_print(msg)

/* ************************************************************************** */

/**
 * \brief MSP430 debug variable macro.
 */
#define ENV_DEBUG_VAR(var) \
do\
{\
	msp430_print(#var ": 0x");\
	msp430_print_hex((unsigned long)(var));\
	msp430_print("\n");\
} while (0)

/* ************************************************************************** */

/**
 * \brief MSP430 panic macro.
 */
#define ENV_PANIC(msg) \
	msp430_panic(msg)

/* ************************************************************************** */

/**
 * \brief MSP430 thread initialization macro.
 *
 * Will initialize a thread context.
 */
#define ENV_CONTEXT_INIT(context, stacksize, function) \
	msp430_context_init(context, stacksize, function)

/* ************************************************************************** */

/**
 * \brief MSP430 thread dispatch macro.
 *
 * Will dispatch the selected thread.
 */
#define ENV_CONTEXT_DISPATCH(thread) \
	msp430_context_dispatch((env_context_t *)(thread))

/* ************************************************************************** */

/**
 * \brief MSP430 context save macro.
 *
 * Should save the context of the current thread.
 */
#define MSP430_CONTEXT_SAVE() \
	__asm__ __volatile__ (\
		/* Save all general purpose registers. */\
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
		/* Save the sp to tt_current. */\
		"mov	&tt_current, r4\n"\
		"mov	r1, 0(r4)\n"\
		/* Check the context cookie. */\
		"mov	2(r4), r5\n"\
		"cmp	%0, 0(r5)\n"\
		"jz		.Lcookie_ok%=\n"\
		"call	msp430_cookie_panic\n"\
		".Lcookie_ok%=:\n"\
		:: "I" (MSP430_CONTEXT_COOKIE)\
	)


/* ************************************************************************** */

/**
 * \brief MSP430 context restore macro.
 *
 * Should restore the context of the current thread.
 */
#define MSP430_CONTEXT_RESTORE() \
	__asm__ __volatile__ (\
		/* Grab the current thread sp. */\
		"mov	&tt_current, r4\n"\
		"mov	0(r4), r1\n"\
		/* Check the context cookie for this context. */\
		"mov	2(r4), r5\n"\
		"cmp	%0, 0(r5)\n"\
		"jz		.Lcookie_ok%=\n"\
		"call	msp430_cookie_panic\n"\
		".Lcookie_ok%=:\n"\
		/* Restore all genral purpose registers. */\
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
		:: "I" (MSP430_CONTEXT_COOKIE)\
	)

/* ************************************************************************** */

/**
 * \brief MSP430 idle macro.
 *
 * Will place the environment in the "idle" state.
 */
#define ENV_IDLE() \
	msp430_idle()

/* ************************************************************************** */

/**
 * \brief MSP430 timer resolution macro.
 */
#define ENV_TIMER_HZ() \
	0x8000 /* 32768 */

/* ************************************************************************** */

/**
 * \brief MSP430 start timer macro.
 *
 * Will start running the environment timer.
 */
#define ENV_TIMER_START() \
	msp430_timer_start()

/* ************************************************************************** */

/**
 * \brief MSP430 stop timer macro.
 *
 * Will stop the environment timer, not needed and should probably
 * not be used but supplied for completeness.
 */
#define ENV_TIMER_STOP() \
	msp430_timer_start()

/* ************************************************************************** */

/**
 * \brief MSP430 timer get macro.
 */
#define ENV_TIMER_GET() \
	msp430_timer_get()

/* ************************************************************************** */

/**
 * \brief MSP430 timer set macro.
 */
#define ENV_TIMER_SET(time) \
	msp430_timer_set(time)

/* ************************************************************************** */

/**
 * \brief MSP430 timer timestamp macro.
 */
#define ENV_TIMESTAMP() \
	msp430_timestamp()

/* ************************************************************************** */

/**
 * \brief MSP430 timer usec macro.
 */
#define ENV_USEC(val) \
	(((env_time_t)val*ENV_TIMER_HZ())/1000000UL)

/* ************************************************************************** */

/**
 * \brief MSP430 timer msec macro.
 */
#define ENV_MSEC(val) \
	(((env_time_t)val*ENV_TIMER_HZ())/1000UL)

/* ************************************************************************** */

/**
 * \brief MSP430 timer sec macro.
 */
#define ENV_SEC(val) \
	((env_time_t)val*ENV_TIMER_HZ())

/* ************************************************************************** */

/**
 * \brief MSP430 stack size.
 */
#define ENV_STACKSIZE 128

/* ************************************************************************** */

/**
 * \brief MSP430 idle stack size.
 */
#define ENV_STACKSIZE_IDLE 128

/* ************************************************************************** */

/**
 * \brief MSP430 number of threads.
 */
#define ENV_NUM_THREADS	5

/* ************************************************************************** */

/**
 * \brief MSP430 stack size(total).
 */
#define MSP430_STACKSIZE (ENV_NUM_THREADS*ENV_STACKSIZE+ENV_STACKSIZE_IDLE)

/* ************************************************************************** */

/**
 * \brief MSP430 context cookie.
 */
#define MSP430_CONTEXT_COOKIE	0x55aa


/* ************************************************************************** */

/**
 * \brief The interval of the MSP430 timer.
 */
#define MSP430_TIMER_COUNT		0x10000 /* 65536 */

/* ************************************************************************** */

/**
 * \brief
 * 	MSP430 macro to define what function should be run when
 * 	the device is starting.
 */
#define ENV_STARTUP(function) \
int main(void) \
{\
	extern unsigned char msp430_stack[];\
	WRITE_SP(&msp430_stack[MSP430_STACKSIZE]);\
	/*__asm__ __volatile__ (\
		"mov	%0, r1\n"\
		"mov	r1, r4\n"\
		:: "m" (msp430_stack_start)\
	);*/\
	tt_init();\
	function();\
	tt_run();\
	return 0;\
} extern char dummy

/* ************************************************************************** */

/**
 * \brief Interrupt macro.
 */
#define ENV_INTERRUPT(vec, function) \
__attribute__((naked)) interrupt(vec) ___##function(void) \
{\
	extern env_time_t msp430_timer_timestamp;\
	asm volatile ("bic.b	#0x40, &0x35\n");\
	MSP430_CONTEXT_SAVE();\
	msp430_timer_timestamp = msp430_timer_get();\
	function();\
	tt_schedule();\
	MSP430_CONTEXT_RESTORE();\
	asm volatile ("bis.b	#0x40, &0x35\n");\
	asm volatile ("reti\n");\
} extern char dummy


/* ************************************************************************** */

/**
 * \brief inline static function to protect the kernel.
 *
 * \param state If nonzero the enter protected mode, otherwise nop.
 */
static inline void msp430_protect(int state)
{
	if (state)
	{
		dint();
		asm volatile ("bic.b	#0x40, &0x35\n");\
	}
	else
	{
		asm volatile ("bis.b	#0x40, &0x35\n");\
		eint();
	}
}

/* ************************************************************************** */

/**
 * \brief The msp430 Timer start function.
 *
 * Will start the timer used for tinyTimber on msp430.
 */
static inline void msp430_timer_start(void)
{
	/* Mode of TimerA should always be Continous. */
	TACTL |= MC_2;
}

/* ************************************************************************** */

/**
 * \brief The msp430 Timer stop function.
 *
 * Will start the timer used for tinyTimber on msp430.
 */
static inline void msp430_timer_stop(void)
{
	/* Clear mode control bits 0 and 1(4 and 5 in reality). */
	TACTL &= ~(MC0|MC1);
}

/* ************************************************************************** */

/**
 * \brief The msp430 Timer get function.
 *
 * Will return the current timer value.
 */
static inline env_time_t msp430_timer_get(void)
{
	extern env_time_t msp430_timer_base;
#if 1
	env_time_t time;

	/* 
	 * The value of the timer is the internal counter value +
	 * the timer counter value. Take overflow into account.
	 */
	time = msp430_timer_base + TAR;
	if (TACTL & TAIFG)
		time = msp430_timer_base + MSP430_TIMER_COUNT + TAR;

	return time;
#else
	return msp430_timer_base + TAR;
#endif
}

/* ************************************************************************** */

/**
 * \brief The msp430 Timer timestamp function.
 *
 * Will return the last timestamp.
 */
static inline env_time_t msp430_timestamp(void)
{
	extern env_time_t msp430_timer_timestamp;
	return msp430_timer_timestamp;
}

/* ************************************************************************** */

/**
 * \brief The MSP430 print hex macro.
 *
 * Used to debug variables by printing the value in hex to the
 * RS-232 port.
 *
 * \param value The variable to print.
 */
static inline void msp430_print_hex(unsigned long value)
{
	static char hex[] = "0123456789abcdef";

	while (!(U0TCTL & TXEPT));
	U0TXBUF = hex[value>>28&0xf];
	while (!(U0TCTL & TXEPT));
	U0TXBUF = hex[value>>24&0xf];
	while (!(U0TCTL & TXEPT));
	U0TXBUF = hex[value>>20&0xf];
	while (!(U0TCTL & TXEPT));
	U0TXBUF = hex[value>>16&0xf];
	while (!(U0TCTL & TXEPT));
	U0TXBUF = hex[value>>12&0xf];
	while (!(U0TCTL & TXEPT));
	U0TXBUF = hex[value>>8&0xf];
	while (!(U0TCTL & TXEPT));
	U0TXBUF = hex[value>>4&0xf];
	while (!(U0TCTL & TXEPT));
	U0TXBUF = hex[value&0xf];
	while (!(U0TCTL & TXEPT));
}

#endif
