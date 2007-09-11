#ifndef ENV_POSIX_TYPES_H_
#define ENV_POSIX_TYPES_H_

/* Standard C headers. */
#include <signal.h>
#include <stdint.h>

/* POSIX/UNIX headers. */
#include <pthread.h>

/**
 * \brief The internal context of a posix thread.
 */
typedef struct posix_context_t
{
	/**
	 * \brief The actual thread.
	 */
	pthread_t thread;

	/**
	 * \brief The context lock.
	 */
	pthread_mutex_t lock;

	/**
	 * \brief The signal condition variable.
	 */
	pthread_cond_t signal;

	/**
	 * \brief The state of the thread.
	 */
	sig_atomic_t run;

	/**
	 * \brief Pointer to the thread function to be run.
	 */
	void (*function)(void);
} posix_context_t;

/**
 * \brief Typedef so that tinyTimber knows what to use as context.
 */
typedef struct posix_context_t env_context_t;

/**
 * \brief Typedef so that tinyTimber knows that time is.
 */
typedef unsigned long env_time_t;

/**
 * \brief Typedef so that tinyTimber knows what a result is.
 */
typedef uintptr_t env_result_t;

/**
 * \brief POSIX interrupt handler.
 */
typedef void (*posix_ext_interrupt_handler_t)(int);

#endif
