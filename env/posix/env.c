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

/*
 * First check what library we are using. So far we only have support for
 * GLIBC but more versions will be added "later". For info about the
 * defines take a look at features.h
 */

/* Grab POSIX and XGP 5 revision. */
#define _XOPEN_SOURCE 500

/* Grab XPG and X/Open Unix extensions. */
#define _XOPEN_SOURCE_EXTENDED 1

/* Grab POSIX. */
#define _POSIX_C_SOURCE 199506L

/* Standard C headers. */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* POSIX/UNIX headers. */
#include <sched.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/select.h>

/* Environment headers. */
#include <posix/env.h>
#include <posix/types.h>
#include <posix/ack.h>

/* tinyTimber headers. */
#include <kernel.h>

/* ************************************************************************** */

/**
 * \brief POSIX number interrupt vector slots.
 */
#ifndef POSIX_NUM_INTERRUPTS
#	define POSIX_NUM_INTERRUPTS 10
#endif

/**
 * \brief POSIX interrupt hammer.
 *
 * If defined then hammer with interrupts all the time.
 */
#define POSIX_INTERRUPT_HAMMER
#undef POSIX_INTERRUPT_HAMMER

#ifdef POSIX_INTERRUPT_HAMMER
#	if POSIX_NUM_INTERRUPTS < 3
#		error POSIX_INTERRUPT_HAMMER defined but not enough interrupts.
#	endif
#endif

/**
 * \brief POSIX timer tick in usec.
 */
#ifndef POSIX_TICK
#	define POSIX_TICK	100000
#endif

/* ************************************************************************** */

/**
 * \brief Variable to keep track of what mode we are in.
 */
static pthread_key_t posix_mode;

/* ************************************************************************** */

/**
 * \brief Variable to keep track of the thread context.
 */
static pthread_key_t posix_context;

/* ************************************************************************** */

/**
 * \brief
 *	Variable to keep track of how many threads have been
 *	created.
 */
static int posix_num_threads;

/**
 * \brief POSIX thread ready ack.
 */
static ack_t *posix_thread_ready_ack;

/* ************************************************************************** */

/**
 * \brief POSIX kernel_lock.
 */
static pthread_mutex_t posix_kernel_lock = PTHREAD_MUTEX_INITIALIZER;

/* ************************************************************************** */

/**
 * \brief POSIX interrupt_lock.
 */
static pthread_mutex_t posix_interrupt_lock = PTHREAD_MUTEX_INITIALIZER;

/* ************************************************************************** */

/**
 * \brief POSIX interrupt signal.
 */
static pthread_cond_t posix_interrupt_enabled_signal = PTHREAD_COND_INITIALIZER;

#ifdef POSIX_INTERRUPT_HAMMER
/* ************************************************************************** */

/**
 * \brief POSIX timer interrupt.
 */
static pthread_t posix_hammer_interrupt0;

/* ************************************************************************** */

/**
 * \brief POSIX timer interrupt.
 */
static pthread_t posix_hammer_interrupt1;

/* ************************************************************************** */

/**
 * \brief POSIX timer interrupt.
 */
static pthread_t posix_hammer_interrupt2;
#endif

/* ************************************************************************** */

/**
 * \brief POSIX clock source.
 */
static pthread_t posix_clock_source;

/* ************************************************************************** */

/**
 * \brief POSIX clock start ack.
 */
static ack_t *posix_clock_start_ack;

/* ************************************************************************** */

/**
 * \brief POSIX interrupt start ack.
 */
static ack_t *posix_interrupt_start_ack;

/* ************************************************************************** */

/**
 * \brief POSIX clock counter.
 */
unsigned long posix_clock_tick;

/* ************************************************************************** */

/**
 * \brief POSIX clock interrupt stamp.
 */
static unsigned long posix_clock_timestamp;

/* ************************************************************************** */

/**
 * \brief POSIX next interrupt counter.
 */
unsigned long posix_clock_next;

/* ************************************************************************** */

/**
 * \brief POSIX interrupt ack.
 */
static ack_t *posix_interrupt_ack;

/* ************************************************************************** */

/**
 * \brief POSIX interrupt varible.
 */
static sig_atomic_t posix_interrupt;

/* ************************************************************************** */

/**
 * \brief POSIX interrupts ok flag.
 */
static volatile sig_atomic_t posix_interrupts_enabled;

/* ************************************************************************** */

/**
 * \brief POSIX interrupt vector.
 */
static posix_ext_interrupt_handler_t posix_interrupt_vector[POSIX_NUM_INTERRUPTS];

/* ************************************************************************** */

/**
 * \brief POSIX "interrupt" handler.
 */
static void posix_interrupt_handler(int sig)
{
	sig_atomic_t interrupt;
	posix_context_t *context;

	/* Grab our context. */
	context = pthread_getspecific(posix_context);
	if (!context)
		posix_panic("Unable to aquire thread context.\n");
	if (context != tt_current)
	{
#if 0
		fprintf(stderr, "posix_interrupt_handler()\n");
		fprintf(stderr, "context: %p\n", context);
		fprintf(stderr, "tt_context: %p\n", tt_current);
#endif
		posix_panic("Wrong thread running.\n");
	}

	/* Sanity. */
	if (posix_isprotected())
		posix_panic("Erh, interrupt in protected mode?\n");

	posix_protect(1);

	/* Ok, we got the kernel lock so let's grab the interrupt id. */
	interrupt = posix_interrupt;
	ack_set(posix_interrupt_ack);
	posix_clock_timestamp = posix_clock_tick;

	/* Run the interrupt function. */
	posix_interrupt_vector[interrupt](interrupt);

	posix_protect(0);
}

/* ************************************************************************** */

/**
 * \brief POSIX thread wrapper function.
 *
 * \param data The data for the thread.
 * \return Will never return.
 */
static void *posix_thread_wrapper(void *data)
{
	sigset_t block;
	posix_context_t *context = data;

	assert(data);

	/* Set the block to block all except SIGUSR1. */
	sigfillset(&block);
	sigdelset(&block, SIGUSR1);
	if (pthread_sigmask(SIG_BLOCK, &block, NULL))
		posix_panic("Unable to set sigmask for thread.\n");

	/* Setup thread local variables. */
	if (pthread_mutex_lock(&context->lock))
		posix_panic("Unable to aquire context lock.\n");
	if (pthread_setspecific(posix_context, data))
		posix_panic("Unable to set thread specific context.\n");

	/* We are now ready to run, send ack. */
	ack_set(posix_thread_ready_ack);

	/* Wait until we're released. */
	while (!context->run)
	{
		if (pthread_cond_wait(&context->signal, &context->lock))
			posix_panic("Unable to wait for signal state change.\n");
	}

	/* Release our thread context. */
	if (pthread_mutex_unlock(&context->lock))
		posix_panic("Unable to release context lock.\n");

	posix_protect(1);

	fprintf(stderr, "And running!\n");

	/* Call the function. */
	context->function();

	/* Unless we do something stupid we will never get here. */
	posix_panic("The tinyTimber thread returned.\n");
	return NULL;
}

#ifdef POSIX_INTERRUPT_HAMMER

/* ************************************************************************** */

/**
 * \brief POSIX timer thread0.
 *
 * This will be the thread that generates the timer interrupts.
 *
 * \param data The data for the thread, not used.
 * \return Will never return.
 */
static void *posix_hammer_thread0(void *data)
{
	/* Wait until the we are allowed to run. */
	ack_wait(posix_interrupt_start_ack, 1);


	/* Hang forever. */
	for (;;)
	{
		posix_ext_interrupt_generate(POSIX_NUM_INTERRUPTS-1);
		usleep(1);
	}
}

/* ************************************************************************** */

/**
 * \brief POSIX timer thread1.
 */
static void *posix_hammer_thread1(void *data)
{
	/* Wait until the we are allowed to run. */
	ack_wait(posix_interrupt_start_ack, 1);
	for (;;)
	{
		posix_ext_interrupt_generate(POSIX_NUM_INTERRUPTS-2);
		usleep(1);
	}
}

/* ************************************************************************** */

/**
 * \brief POSIX timer thread2.
 */
static void *posix_hammer_thread2(void *data)
{
	/* Wait until the we are allowed to run. */
	ack_wait(posix_interrupt_start_ack, 1);
	for (;;)
	{
		posix_ext_interrupt_generate(POSIX_NUM_INTERRUPTS-3);
		usleep(1);
	}
}
#endif

/* ************************************************************************** */

/**
 * \brief POSIX tinyTimber clock source.
 */
void *posix_clock_source_interrupt(void *data)
{
	struct timeval timeout;

	/* Wait until the we are allowed to run. */
	ack_wait(posix_interrupt_start_ack, 1);

	/* Wait until we should start. */
	ack_wait(posix_clock_start_ack, 1);
	for (;;)
	{
		/* Set the timeout. */
		timeout.tv_sec = 0;
		timeout.tv_usec = POSIX_TICK;
		if (select(0, NULL, NULL, NULL, &timeout))
			posix_panic("Unable to wait in clock source.\n");

		/* Generate the interrupt. */
		posix_ext_interrupt_generate(0);
	}
}

/* ************************************************************************** */

/**
 * \brief POSIX noop interrupt handler.
 */
static void posix_interrupt_noop(int id)
{
	/* Do nothing for now. */
}

/* ************************************************************************** */

/**
 * \brief POSIX clock source interrupt handler.
 */
static void posix_interrupt_clock(int id)
{
	/* Update the current time. */
	if (++posix_clock_tick == posix_clock_next)
	{
		tt_expired(posix_clock_tick);
		tt_schedule();
	}
}

/* ************************************************************************** */

/**
 * \brief POSIX init function.
 *
 * \note
 *	Upon failure posix_panic() will be called.
 */
void posix_init(void)
{
	int i;
	sigset_t block;
	struct sigaction sa;

	if (pthread_key_create(&posix_mode, NULL))
		posix_panic("Unable to create protected key for thread.\n");
	posix_protect(1);

	/* Set some special thread local variables for root. */
	if (pthread_key_create(&posix_context, NULL))
		posix_panic("Unable to create context key for thread.\n");
	if (pthread_setspecific(posix_mode, (void *)1))
		posix_panic("Unable to set the protected mode for root.\n");
	if (pthread_setspecific(posix_context, NULL))
		posix_panic("Unable to set the root context.\n");

	/* Install defailt interrupt handlers. */
	posix_interrupt_vector[0] = posix_interrupt_clock;
	for (i=1;i<POSIX_NUM_INTERRUPTS;i++)
		posix_interrupt_vector[i] = posix_interrupt_noop;

	posix_num_threads = 0;
	posix_thread_ready_ack = ack_new();

	posix_interrupt_ack = ack_new();
	posix_interrupt_start_ack = ack_new();

	posix_clock_start_ack = ack_new();

	/* Install the "interrupt" handler. */
	sa.sa_handler = posix_interrupt_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGUSR1, &sa, NULL))
		posix_panic("Unable to set the signal handler.\n");

	/* Block all signals for the root thread, except the SIGINT. */
	sigfillset(&block);
	sigdelset(&block, SIGINT);
	//sigdelset(&block, SIGUSR1);
	if (pthread_sigmask(SIG_BLOCK, &block, NULL))
		posix_panic("Unable to set block for root thread.\n");

#ifdef POSIX_INTERRUPT_HAMMER
	/* Create the timer0 thread. */
	if (
			pthread_create(
				&posix_hammer_interrupt0,
				NULL,
				posix_hammer_thread0,
				NULL
				)
		)
		posix_panic("Unable to create the timer interrupt thread.\n");

	/* Create the timer1 thread. */
	if (
			pthread_create(
				&posix_hammer_interrupt1,
				NULL,
				posix_hammer_thread1,
				NULL
				)
		)
		posix_panic("Unable to create the timer interrupt thread.\n");

	/* Create the timer2 thread. */
	if (
			pthread_create(
				&posix_hammer_interrupt2,
				NULL,
				posix_hammer_thread2,
				NULL
				)
		)
		posix_panic("Unable to create the timer interrupt thread.\n");
#endif

	/* Create the clock source thread. */
	if (
			pthread_create(
				&posix_clock_source,
				NULL,
				posix_clock_source_interrupt,
				NULL
				)
	   )
		posix_panic("Unable to create the clock source interrupt.\n");
}

/* ************************************************************************** */

/**
 * \brief POSIX panic function.
 *
 * \note
 *	Will never return, calls abort().
 *
 * \param msg The message to print.
 */
void posix_panic(const char * const msg)
{
	assert(msg);

	/* Print the message. */
	fprintf(stderr, msg);

	/* abort execution. */
	abort();
}

/* ************************************************************************** */

/**
 * \brief POSIX context init function.
 *
 * \note
 *	Upon failure posix_panic() will be called.
 *
 * \param context The context to initialize.
 * \param stacksize The amount of stack to allocate for stack.
 * \param func The function that should run in the thread.
 */
void posix_context_init(posix_context_t *context, void (*function)(void))
{
	assert(context);
	assert(function);

	/* Initialize the mutex. */
	if (pthread_mutex_init(&context->lock, NULL))
		posix_panic("Unable to initialize lock.\n");

	/* Initialize the signal conditioan. */
	if (pthread_cond_init(&context->signal, NULL))
		posix_panic("Unable to initialize signal.\n");

	/* Thread is not running. */
	context->run= 0;

	/* Set the context function. */
	context->function = function;

	/* Create the thread. */
	if (pthread_create(&context->thread, NULL, posix_thread_wrapper, context))
		posix_panic("Unable to create thread.\n");

	/* Increse thread counter. */
	posix_num_threads++;
}

/* ************************************************************************** */

/**
 * \brief POSIX protect function.
 *
 * \param protect If we should enter protected mode.
 */
void posix_protect(int protect)
{
	int isprotected = posix_isprotected();

	/* If we are not protected and wish to become protected. */
	if (protect && !isprotected)
	{
		/* Aquire. */
		if (pthread_mutex_lock(&posix_kernel_lock))
			posix_panic("Unable to lock the kernel_lock.\n");

		/* Set the mode to protected. */
		if (pthread_setspecific(posix_mode, (void *)1))
			posix_panic("Unable to set protected mode.\n");

		/* Disable interrupts. */
		posix_interrupts_enabled = 0;

#if 0
		fprintf(
				stderr,
				"%p: Entering protected mode(locking).\n",
				pthread_getspecific(posix_context)
				);
#endif
	}

	/* If we are protected and wish to become unprotected. */
	else if (!protect && isprotected)
	{
#if 0
		fprintf(
				stderr,
				"%p: Leaving protected mode(unlocking).\n",
				pthread_getspecific(posix_context)
				);
#endif

		/* Enable interrupts. */
		posix_interrupts_enabled = 1;

		/* Set the mode to unprotected. */
		if (pthread_setspecific(posix_mode, (void *)0))
			posix_panic("Unable to set unprotected mode.\n");

		/* Awake any interrupts. */
		if (pthread_cond_broadcast(&posix_interrupt_enabled_signal))
			posix_panic("Unable to signal interrupts enabled.\n");

		/* Release. */
		if (pthread_mutex_unlock(&posix_kernel_lock))
			posix_panic("Unable to unlock the kernel_lock.\n");

#if 0
		/* Make sure that any interrut is actually run. */
		if (sched_yield())
			posix_panic("Unable to yield thread when dropping protection.\n");
#endif
	}
}

/* ************************************************************************** */

/**
 * \brief POSIX isprotected function.
 *
 * \return non-zero if protected, otherwise zero.
 */
int posix_isprotected(void)
{
	/* If posix_mode is != 0 then we are protected. */
	return !!pthread_getspecific(posix_mode);
}

/* ************************************************************************** */

/**
 * \brief POSIX timer start function.
 */
void posix_timer_start(void)
{
	/* Send the to the clock source. */
	ack_set(posix_clock_start_ack);
}

/* ************************************************************************** */

/**
 * \brief Idle function.
 */
void _posix_idle(void)
{
	/* Hang without using CPU. */
	for (;;)
		pause();
}

/**
 * \brief POSIX idle function.
 *
 * Will place the environment into an idle state.
 */
void posix_idle(void)
{
	/*
	 * We must initialize the idle context since the kernel does not
	 * do this for us.
	 */
	posix_context_init(tt_current, _posix_idle);

	/* Leave protected mode. */
	posix_protect(0);

	/* Make sure the interrupts start running. */
	ack_set(posix_interrupt_start_ack);

	/* Hang the root thread. */
	for (;;)
		pause();
}

/* ************************************************************************** */

/**
 * \brief POSIX dispatch function.
 *
 * \param The thread to dispatch.
 */
void posix_context_dispatch(posix_context_t *thread)
{
	int err;
	posix_context_t *context;

	/* Grab our context. */
	context = pthread_getspecific(posix_context);

	/* Sanity check. */
	if (context && context != tt_current)
		posix_panic("Non-current context running.\n");
	else if (!context)
	{
		/*
		 * We are root, let's wait for all threads before we
		 * go any further.
		 */
		ack_wait(posix_thread_ready_ack, posix_num_threads);
	}

	/* Sanity check, we must be in protected mode. */
	if (!posix_isprotected())
		posix_panic("posix_dispatch() in nonprotected context.\n");

	/* Stupid sanity check, had a slight bug before... */
	if (!pthread_mutex_trylock(&posix_kernel_lock))
		posix_panic("erh, why can we lock the kernel here?\n");

	/* Change the tt_current. */
	tt_current = thread;

	/* Grab the new current thread context lock. */
	if ((err = pthread_mutex_lock(&tt_current->lock)))
	{
		fprintf(stderr, "%d: %s\n", err, strerror(err));
		posix_panic("Unable to aquire new current context lock.\n");
	}

#if 0
	/* Sanity. */
	fprintf(stderr, "dispatch()\n");
	fprintf(stderr, "old: %p\n", context);
	fprintf(stderr, "new: %p\n", tt_current);
#endif

	/* Set the new context state. */
	tt_current->run = 1;

	/* Release the current lock and in effect start the thread. */
	if (pthread_mutex_unlock(&tt_current->lock))
		posix_panic("Unable to release new current context lock.\n");

	/* Wake up the new current thread. */
	if (pthread_cond_signal(&(tt_current->signal)))
		posix_panic("Unable to signal new current state change.\n");

	/* We are no longer running. */
	context->run = 0;

	/* Unlock the context so that someone could aquire it. */
	if (pthread_mutex_unlock(&context->lock))
		posix_panic("Unable to release context lock.\n");

	/* Until we should run again. */
	while (!context->run)
	{
		/* Wait until we are ready to run again. */
		if (pthread_cond_wait(&context->signal, &posix_kernel_lock))
			posix_panic("Unable to wait until we are released again.\n");

		if (!pthread_mutex_trylock(&posix_kernel_lock))
			fprintf(stderr, "WTF?!\n");
	}

	/* Aquire our context lock again. */
	if (pthread_mutex_lock(&context->lock))
		posix_panic("Unable to aquire context lock.\n");
}

/* ************************************************************************** */

/**
 * \brief POSIX get timer function.
 */
env_time_t posix_timer_get(void)
{
	/* Return current time. */
	return posix_clock_tick;
}

/* ************************************************************************** */

/**
 * \brief POSIX set timer function.
 */
void posix_timer_set(env_time_t time)
{
	/* Set the next timeout. */
	posix_clock_next = time;
}

/* ************************************************************************** */

/**
 * \brief POSIX get the timestamp.
 */
env_time_t posix_timestamp(void)
{
	/* Return the last timestamp. */
	return posix_clock_timestamp;
}

/* ************************************************************************** */

/**
 * \brief POSIX usec conversion function.
 */
env_time_t posix_usec(env_time_t val)
{
	env_time_t tmp = val/POSIX_TICK;

	/* Don't return 0. */
	return (tmp?tmp:1);
}

/* ************************************************************************** */

/**
 * \brief POSIX msec conversion function.
 */
env_time_t posix_msec(env_time_t val)
{
	env_time_t tmp = (val*1000)/POSIX_TICK;

	/* Don't return 0. */
	return (tmp?tmp:1);
}

/* ************************************************************************** */

/**
 * \brief POSIX sec conversion function.
 */
env_time_t posix_sec(env_time_t val)
{
	env_time_t tmp = (val*1000000)/POSIX_TICK;

	/* Don't return 0. */
	return (tmp?tmp:1);
}

/* ************************************************************************** */

/**
 * \brief POSIX interrupt handler install.
 */
void posix_ext_interrupt_handler(int id, posix_ext_interrupt_handler_t handler)
{
	assert(id < POSIX_NUM_INTERRUPTS);

	/* Install the handler. */
	posix_interrupt_vector[id] = handler;
}

/* ************************************************************************** */

/**
 * \brief POSIX interrupt generator.
 */
void posix_ext_interrupt_generate(int id)
{
	/* Aquire the interrupt lock. */
	if (pthread_mutex_lock(&posix_interrupt_lock))
		posix_panic("Unable to aquire interrupt lock.\n");

	/* NOTE: This is where we wish to set the interupt nr. */
	posix_interrupt = id;

	/* Grab the kernel lock. */
	if (pthread_mutex_lock(&posix_kernel_lock))
		posix_panic("Unable to aquire kernel lock.\n");

	/* Wait until interrupts are ok. */
	while (!posix_interrupts_enabled)
	{
		if (
			pthread_cond_wait(
				&posix_interrupt_enabled_signal,
				&posix_kernel_lock
				)
			)
			posix_panic("Unable to wait until we can run the interrupt.\n");
	}

	/* Clear the interrupt ack. */
	ack_clear(posix_interrupt_ack);

	/* Pre-empt the current thread. */
	if (pthread_kill(tt_current->thread, SIGUSR1))
		posix_panic("Unable to deliver signal to thread.\n");

	/*
	 * Ok, we have the thread lock. let's release the kernel_lock
	 * to the thread.
	 */
	if (pthread_mutex_unlock(&posix_kernel_lock))
		posix_panic("Unable to unlock kernel lock.\n");

	/* Wait for ack from thread that it has registered the interrupt. */
	ack_wait(posix_interrupt_ack, 1);

	/* Release the interrupt lock. */
	if (pthread_mutex_unlock(&posix_interrupt_lock))
		posix_panic("Unable to release the interrupt lock.\n");

	/*
	 * Yield the thread that generated the interrupt, so that other
	 * interrupts are generated aswell.
	 */
	if (sched_yield())
		posix_panic("Unable to yield the interrupt thread.\n");
}
