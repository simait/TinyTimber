#ifndef ENV_H_
#define ENV_H_

#if defined ENV_POSIX
#	include "posix/env.h"
#elif defined ENV_MSP430
#	include "msp430/env.h"
#elif defined ENV_AVR
#	include "avr/env.h"
#elif defined ENV_PIC18
#	include "pic18/env.h"
#elif defined ENV_ARM7
#	include "arm7/env.h"
#elif defined ENV_SKEL
#	include "skel/env.h"
#else
#	error Unknown environment.
#endif

/* ************************************************************************** */
/* ********************************** DEFAULT ******************************* */
/* ************************************************************************** */

/**
 * \brief Environment thread entry macro.
 *
 * If the environment did not supply this macro then supply default
 * of nothing.
 */
#ifndef ENV_THREAD_ENTRY
#	define ENV_THREAD_ENTRY
#endif

/**
 * \brief Environment inline macro.
 *
 * If the environment did not supply a definition for this then default to
 * nothing. Not all environments allow for the inline keyword(it's C99 not C89).
 */
#ifndef ENV_INLINE
#	define ENV_INLINE
#endif

/**
 * \brief Environment fast code macro.
 *
 * If the environment does not supply a defenition for this the default is
 * nothing. This really only makes sense on the arm7 where we want to force
 * the linking of the functions into SRAM since it's the only thing that is
 * read in 1 cycle.
 */
#ifndef ENV_CODE_FAST
#	define ENV_CODE_FAST
#endif

/* ************************************************************************** */
/* *********************************** SANITY ******************************* */
/* ************************************************************************** */

#ifndef ENV_INIT
#	error Environment did not define ENV_INIT().
#endif

#ifndef ENV_PROTECT
#	error Environment did not define ENV_PROTECT().
#endif

#ifndef ENV_ISPROTECTED
#	error Environment did not define ENV_PROTECTED().
#endif

#ifndef ENV_DEBUG
#	error Environment did not define ENV_DEBUG().
#endif

#ifndef ENV_PANIC
#	error Environment did not define ENV_PANIC().
#endif

#ifndef ENV_NUM_THREADS
#	error Environment did not define ENV_NUM_THREADS.
#endif

#ifndef ENV_CONTEXT_INIT
#	error Environment did not define ENV_CONTEXT_INIT().
#endif

#ifndef ENV_CONTEXT_DISPATCH
#	error Environment did not define ENV_CONTEXT_DISPATCH().
#endif

#ifndef ENV_IDLE
#	error Environment did not define ENV_IDLE()
#endif

#ifndef ENV_TIMER_START
#	error Environment did not define ENV_TIMER_START().
#endif

#ifndef ENV_TIMER_SET
#	error Environment did not define ENV_TIMER_SET().
#endif

#ifndef ENV_TIMER_GET
#	error Environment did not define ENV_TIMER_GET().
#endif

#ifndef ENV_TIMESTAMP
#	error Environment did not define ENV_TIMESTAMP().
#endif

#ifndef ENV_USEC
#	error Environment did not define ENV_USEC().
#endif

#ifndef ENV_MSEC
#	error Environment did not define ENV_MSEC().
#endif

#ifndef ENV_SEC
#	error Environment did not define ENV_SEC().
#endif

#ifndef ENV_STARTUP
#	error Environment did not define ENV_STARTUP().
#endif

#ifdef ENV_PREEMPTIVE_INTERRUPTS
#	error Environment defined ENV_PREEMPTIVE_INTERRUPTS but there \
are no guidelines for this yet.
#endif

#endif
