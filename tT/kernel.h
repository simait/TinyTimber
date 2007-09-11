#ifndef KERNEL_H_
#define KERNEL_H_

#include <tT/tT.h>
#include <env/types.h>

/* ************************************************************************** */

/**
 * \brief tinyTimber thread function typedef.
 */
typedef void (*tt_thread_function_t)(void);

/* ************************************************************************** */

/**
 * \brief tinyTimber message typedef.
 */
typedef struct tt_message_t tt_message_t;

/* ************************************************************************** */

/**
 * \brief tinyTimber thread structure.
 */
struct tt_thread_t
{
	/**
	 * \brief The context of the environment.
	 *
	 * \note
	 * 	We use the same thing as with tt_object_t, keep this as the first
	 * 	thing in the struct giving it the same address allowing the
	 * 	environment to forget about what a tt_thread_t is.
	 */
	env_context_t context;

	/**
	 * \brief The message this thread is running.
	 */
	tt_message_t *msg;

	/**
	 * \brief The object this thread is waiting for.
	 */
	tt_object_t *waits_for;

	/**
	 * \brief The next thread in the list.
	 */
	struct tt_thread_t *next;
};

/* ************************************************************************** */

/**
 * \brief The current running thread according to tinyTimber.
 */
extern env_context_t *tt_current;


/* ************************************************************************** */

void tt_interrupt(void);
void tt_expired(env_time_t);

/* ************************************************************************** */

#ifndef TT_NUM_MESSAGES
	/**
	 * \brief The number of messages that the kernel facillitates.
	 */
#	define TT_NUM_MESSAGES	10
#endif

/* ************************************************************************** */

#ifndef TT_ARGS_SIZE
	/**
	 * \brief The size of the variable argument buffer.
	 */
#	define TT_ARGS_SIZE		8
#endif

/* ************************************************************************** */

#ifdef TT_KERNEL_SANITY
	/** \cond */
#	define _STR(str) #str
#	define STR(str) _STR(str)
	/** \endcond */
	
	/**
	 * \brief The kernel sanity macro.
	 *
	 * Should only be used for debugging purposes. Used whenever
	 * TT_KERNEL_SANITY is defined. Will fail whenever the expression
	 * supplied is not evaluated as true.
	 */
#	define TT_SANITY(expr) \
		if (!(expr))\
			ENV_PANIC(\
					__FILE__ ":" STR(__LINE__) " ("\
					#expr ") failed sanity check.\n"\
					)
#else
#	define TT_SANITY(expr)
#endif

#endif
