#ifndef ENV_MSP430_TYPES_H_
#define ENV_MSP430_TYPES_H_

#include <stdint.h>
#include <stddef.h>

/**
 * \brief The msp430 context structure typedef.
 */
typedef struct msp430_context_t
{
	/**
	 * \brief The stackpointer.
	 */
	unsigned short *sp;

	/**
	 * \brief The magic cookie pointer.
	 */
	unsigned short *cookie;
} msp430_context_t;


/**
 * \brief Export the msp430 context as env_context_t.
 */
typedef msp430_context_t env_context_t;

/**
 * \brief Export what time is to the kernel.
 */
typedef unsigned long env_time_t;

/**
 * \brief Export what a result is to the kernel.
 */
typedef uintptr_t env_result_t;

#endif
