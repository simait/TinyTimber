#ifndef ENV_PIC18_TYPES_H_
#define ENV_PIC18_TYPES_H_

#ifndef __18CXX
#	error Unknown or unsupported compiler.
#endif

#include <stddef.h>

/**
 * \brief The PIC18 context.
 */
typedef struct pic18_context_t
{
	/**
	 * \brief Saved stack-pointer, holds all information about thread.
	 */
	unsigned char *sp;

	/**
	 * \brief The magic cookie for this context, used to check for overflow.
	 */
	unsigned short *cookie;
} pic18_context_t;

/**
 * \brief Export the pic18 context as environment context.
 */
typedef pic18_context_t env_context_t;

/**
 * \brief Environment time.
 */
typedef unsigned long env_time_t;

/**
 * \brief Environment result.
 */
typedef unsigned long env_result_t;

/**
 * \brief Environment extension for interrupts.
 */
typedef void (*env_ext_interrupt_t)(void);

#endif
