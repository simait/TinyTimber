#ifndef ENV_MIPS_TYPES_H_
#define ENV_MIPS_TYPES_H_

#include <stdint.h>
#include <stddef.h>

/* ************************************************************************** */

/**
 * \brief The mips context structure typedef.
 */
typedef struct mips_context_t
{
	/**
	 * \brief The stackpointer.
	 */
	unsigned int *sp;

	/**
	 * \brief The magic cookie pointer.
	 */
	unsigned int *cookie;
} mips_context_t;

/* ************************************************************************** */

/**
 * \brief Export the mips context as env_context_t.
 */
typedef mips_context_t env_context_t;

/* ************************************************************************** */

/**
 * \brief Export what time is to the kernel.
 */
typedef unsigned long env_time_t;

/* ************************************************************************** */

/**
 * \brief Export what a result is to the kernel.
 */
typedef uintptr_t env_result_t;

#endif
