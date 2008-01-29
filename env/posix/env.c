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

/** \cond */

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
#define POSIX_INTERRUPT_HAMMER 1
#undef POSIX_INTERRUPT_HAMMER

#ifdef POSIX_INTERRUPT_HAMMER
#	if POSIX_NUM_INTERRUPTS < 3
#		error POSIX_INTERRUPT_HAMMER defined but not enough interrupts.
#	endif
#endif

/* ************************************************************************** */

/*
 * Internal state variables etc.
 */
static timer_t timer;
static env_time_t timestamp;
static pthread_key_t thread_mode;
static pthread_key_t thread_context;
static int posix_num_threads;
static ack_t *thread_ready_ack;
static pthread_mutex_t kernel_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t interrupt_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t interrupt_enabled_signal = PTHREAD_COND_INITIALIZER;
static ack_t *interrupt_start_ack;
static ack_t *interrupt_ack;
static sig_atomic_t posix_interrupt;
static volatile sig_atomic_t interrupts_enabled;
static posix_ext_interrupt_handler_t posix_interrupt_vector[POSIX_NUM_INTERRUPTS];

/* ************************************************************************** */

/**
 * \brief POSIX "interrupt" handler.
 */
static void interrupt_handler(int sig)
{
	sig_atomic_t interrupt;
	posix_context_t *context;

	context = pthread_getspecific(thread_context);

	assert(context);
	assert(context == tt_current);
	assert(!posix_isprotected());

	/* 
	 * This requires the kernel_lock so we will not progress beyond this
	 * point before the interrupt generating thread releases the kernel
	 * lock.
	 */
	posix_protect(1);

	/* 
	 * Read the interrupt id variable, acknowledge the interrupt and execute
	 * it.
	 */
	interrupt = posix_interrupt;
	ack_set(interrupt_ack);
	clock_gettime(CLOCK_REALTIME, &timestamp);

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

	/* SIGUSR1 is used to generate interrupts. */
	sigfillset(&block);
	sigdelset(&block, SIGUSR1);
	if (pthread_sigmask(SIG_BLOCK, &block, NULL)) {
		posix_panic(
				"posix_thread_wrapper(): Unable to set sigmask for thread.\n"
				);
	}

	/*
	 * Setup thread local variables.
	 */

	if (pthread_mutex_lock(&context->lock)) {
		posix_panic("posix_thread_wrapper(): Unable to aquire context lock.\n");
	}
	if (pthread_setspecific(thread_context, data)) {
		posix_panic(
				"posix_thread_wrapper(): "
				"Unable to set thread specific context.\n"
				);
	}

	/* 
	 * One more thread is ready to run.
	 */

	ack_set(thread_ready_ack);
	while (!context->run) {
		if (pthread_cond_wait(&context->signal, &context->lock)) {
			posix_panic(
					"posix_thread_wrapper(): "
					"Unable to wait for signal state change.\n"
					);
		}
	}

	/*
	 * Must release context so that we may be interrupted later. For
	 * interrupt synchronization reasons.
	 */

	if (pthread_mutex_unlock(&context->lock)) {
		posix_panic(
				"posix_thread_wrapper(): Unable to release context lock.\n"
				);
	}

	/*
	 * Run the thread, should never return.
	 */

	posix_protect(1);
	context->function();
	assert(NULL);

	return NULL;
}

#ifdef POSIX_INTERRUPT_HAMMER

/* ************************************************************************** */

static void *hammer_thread0(void *data)
{
	ack_wait(interrupt_start_ack, 1);
	fprintf(stderr, "It's hammer time! (0)\n");
	for (;;) {
		/* Generate a large amount of interrupts. */
		posix_ext_interrupt_generate(POSIX_NUM_INTERRUPTS-1);
		usleep(1);
	}
}

/* ************************************************************************** */

static void *hammer_thread1(void *data)
{
	ack_wait(interrupt_start_ack, 1);
	fprintf(stderr, "It's hammer time! (1)\n");
	for (;;) {
		/* Generate a large amount of interrupts. */
		posix_ext_interrupt_generate(POSIX_NUM_INTERRUPTS-2);
		usleep(1);
	}
}

/* ************************************************************************** */

static void *hammer_thread2(void *data)
{
	ack_wait(interrupt_start_ack, 1);
	fprintf(stderr, "It's hammer time! (2)\n");
	for (;;) {
		/* Generate a large amount of interrupts. */
		posix_ext_interrupt_generate(POSIX_NUM_INTERRUPTS-3);
		usleep(1);
	}
}
#endif

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
 * \brief POSIX timer source interrupt handler.
 */
static void timer_interrupt_generate(int id)
{
	/* Interrupt id 0 is always timer interrupt. */
	posix_ext_interrupt_generate(0);
}

/* ************************************************************************** */

/**
 * \brief POSIX timer interrupt generation thread (or receiver of SIGARLM).
 */
static void *timer_thread(void *args)
{
	sigset_t block;
	struct sigaction signal_action;

	memset(&signal_action, 0, sizeof(signal_action));
	signal_action.sa_handler = timer_interrupt_generate;
	if (sigaction(SIGALRM, &signal_action, NULL)) {
		posix_panic(
				"timer_thread(): Unable to set the SIGALRM signal handler\n"
				);
	}

	sigfillset(&block);
	sigdelset(&block, SIGALRM);
	if (pthread_sigmask(SIG_SETMASK, &block, NULL)) {
		posix_panic(
				"timer_thread(): Unable to set sigmask for timer_thread.\n"
				);
	}

	ack_wait(interrupt_start_ack, 1);
	for (;;) {
		pause();
	}
}

/* ************************************************************************** */

/**
 * \brief POSIX timer interrupt handler (real pseudo interrupt).
 */
static void timer_interrupt_handler(int id)
{
	if (tt_expired(posix_timer_get())) {
		tt_schedule();
	}
}

/** \endcond */

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
	struct sigevent timer_event;
	struct sigaction signal_action;
	pthread_t timer_interrupt;
#ifdef POSIX_INTERRUPT_HAMMER
	pthread_t hammer_interrupt0;
	pthread_t hammer_interrupt1;
	pthread_t hammer_interrupt2;
#endif

	if (pthread_key_create(&thread_mode, NULL)) {
		posix_panic(
				"posix_init(): Unable to create protected key for thread.\n"
				);
	}
	posix_protect(1);

	/* Set some special thread local variables for root. */
	if (pthread_key_create(&thread_context, NULL)) {
		posix_panic(
				"posix_init(): Unable to create context key for thread.\n"
				);
	}
	if (pthread_setspecific(thread_mode, (void *)1)) {
		posix_panic(
				"posix_init(): Unable to set the protected mode for root.\n"
				);
	}
	if (pthread_setspecific(thread_context, NULL)) {
		posix_panic(
				"posix_init(): Unable to set the root context.\n"
				);
	}

	/* Install defailt interrupt handlers. */
	posix_interrupt_vector[0] = timer_interrupt_handler;
	for (i=1;i<POSIX_NUM_INTERRUPTS;i++) {
		posix_interrupt_vector[i] = posix_interrupt_noop;
	}

	posix_num_threads = 0;
	thread_ready_ack = ack_new();

	interrupt_ack = ack_new();
	interrupt_start_ack = ack_new();

	/*
	 * Install the singal handlers for the pseudo interrupt and the timer
	 * interrupt generating singal (SIGUSR1 and SIGALRM).
	 */

	memset(&signal_action, 0, sizeof(signal_action));
	signal_action.sa_flags = SA_RESTART;
	sigemptyset(&signal_action.sa_mask);
	signal_action.sa_handler = interrupt_handler;
	if (sigaction(SIGUSR1, &signal_action, NULL)) {
		posix_panic(
				"posix_init(): Unable to set the SIGUSR1 signal handler.\n"
				);
	}

	/* 
	 * Block any signals except SIGINT and SIGUSR1.
	 *
	 * SIGINT is used to kill the program and SIGUSR1 is used for
	 * synchronization betweem the root thread and the worker threads.
	 */

	sigfillset(&block);
	sigdelset(&block, SIGINT);
	sigdelset(&block, SIGUSR1);
	if (pthread_sigmask(SIG_SETMASK, &block, NULL)) {
		posix_panic("posix_init(): Unable to set sigmask for root thread.\n");
	}

	/*
	 * Create the timer used to generate the timer "interrupt".
	 */
	memset(&timer_event, 0, sizeof(timer_event));
	timer_event.sigev_notify = SIGEV_SIGNAL;
	timer_event.sigev_signo = SIGALRM;
	timer_create(CLOCK_REALTIME, &timer_event, &timer);

	/*
	 * Set the timestamp to a somewhat meaningfull value, since we don't
	 * exactly "start" the timer we just assume that the startup is negligable.
	 */

	clock_gettime(CLOCK_REALTIME, &timestamp);

	/* Create the timer thread. */
	if (pthread_create(&timer_interrupt, NULL, timer_thread, NULL)) {
		posix_panic(
				"posix_init(): Unable to create the timer interrupt thread.\n"
				);
	}

#ifdef POSIX_INTERRUPT_HAMMER
	/* Create the hammer0 thread. */
	if (pthread_create(&hammer_interrupt0, NULL, hammer_thread0,NULL)) {
		posix_panic(
				"posix_init(): Unable to create hammer0 interrupt thread.\n"
				);
	}

	/* Create the hammer1 thread. */
	if (pthread_create(&hammer_interrupt1, NULL, hammer_thread1, NULL)) {
		posix_panic(
				"posix_init(): Unable to create hammer1 interrupt thread.\n"
				);
	}

	/* Create the hammer2 thread. */
	if (pthread_create(&hammer_interrupt2, NULL, hammer_thread2, NULL)) {
		posix_panic(
				"posix_init(): Unable to create hammer2 interrupt thread.\n"
				);
	}
#endif
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
	if (pthread_mutex_init(&context->lock, NULL)) {
		posix_panic("posix_context_init(): Unable to initialize lock.\n");
	}

	/* Initialize the signal conditioan. */
	if (pthread_cond_init(&context->signal, NULL)) {
		posix_panic("posix_context_init(): Unable to initialize signal.\n");
	}

	/* Set the context function. */
	context->function = function;

	/* Create the thread. */
	if (pthread_create(&context->thread, NULL, posix_thread_wrapper, context)) {
		posix_panic("posix_context_init(): Unable to create thread.\n");
	}

	/* Increse thread counter, used when waiting for threads to initialize. */
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

	if (protect && !isprotected) {
		/*
		 * We are not protected but we wish to enter protected mode.
		 */

		/* Aquire the kernel lock, stops any interrupts. */
		if (pthread_mutex_lock(&kernel_lock)) {
			posix_panic("posix_protect(): Unable to lock the kernel_lock.\n");
		}

		/* Set the mode to protected and disable interrupts. */
		if (pthread_setspecific(thread_mode, (void *)1)) {
			posix_panic("posix_protect(): Unable to set protected mode.\n");
		}
		interrupts_enabled = 0;
	}
	else if (!protect && isprotected) {
		/*
		 * We are protected but we wish to leave protected mode.
		 */

		/* Enable interrupts and set the mode to unprotected. */
		interrupts_enabled = 1;
		if (pthread_setspecific(thread_mode, (void *)0)) {
			posix_panic("posix_protect(): Unable to set unprotected mode.\n");
		}

		/* Signal any pending interrupts that we left protected mode. */
		if (pthread_cond_broadcast(&interrupt_enabled_signal)) {
			posix_panic(
					"posix_protect(): Unable to signal interrupts enabled.\n"
					);
		}

		/* And release the kernel lock, needed by interrupts. */
		if (pthread_mutex_unlock(&kernel_lock)) {
			posix_panic("posix_protect(): Unable to unlock the kernel_lock.\n");
		}
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
	/* If thread_mode is != 0 then we are protected. */
	return !!pthread_getspecific(thread_mode);
}

/* ************************************************************************** */

/**
 * \brief POSIX idle function.
 *
 * Will place the environment into an idle state.
 */
void posix_idle(void)
{
	/* 
	 * Perform the same task that the thread_wrapper performs but manually
	 * since this context was never initialized. Tedious but that's the
	 * way it's done (for now).
	 */
	if (pthread_mutex_init(&tt_current->lock, NULL)) {
		posix_panic("posix_idle(): Unable to initialize lock.\n");
	}
	if (pthread_cond_init(&tt_current->signal, NULL)) {
		posix_panic("posix_idle(): Unable to initialize signal.\n");
	}
	tt_current->thread = pthread_self();
	if (pthread_setspecific(thread_context, tt_current)) {
		posix_panic("posix_idle(): Unable to set thread specific context.\n");
	}

	/* Leave protected mode and start all interrupt generating threads. */
	posix_protect(0);
	ack_set(interrupt_start_ack);
	for (;;) {
		pause();
	}
}

/* ************************************************************************** */

/**
 * \brief POSIX dispatch function.
 *
 * \param The thread to dispatch.
 */
void posix_context_dispatch(posix_context_t *thread)
{
	posix_context_t *context;

	context = pthread_getspecific(thread_context);
	assert(context == tt_current);

	if (!context) {
		/*
		 * We are root, let's wait for all threads to initialize
		 * properly beforewe go any further.
		 */
		ack_wait(thread_ready_ack, posix_num_threads);
	}

	assert(posix_isprotected());

	tt_current = thread;

	/* Synchronize START. */

	if (pthread_mutex_lock(&tt_current->lock)) {
		posix_panic(
				"posix_context_dispatch(): "
				"Unable to aquire new current context lock.\n"
				);
	}

	tt_current->run = 1;

	if (pthread_mutex_unlock(&tt_current->lock)) {
		posix_panic(
				"posix_context_dispatch(): "
				"Unable to release new current context lock.\n"
				);
	}

	if (pthread_cond_signal(&(tt_current->signal))) {
		posix_panic(
				"posix_context_dispatch(): "
				"Unable to signal new current state change.\n"
				);
	}

	context->run = 0;

	if (pthread_mutex_unlock(&context->lock)) {
		posix_panic(
				"posix_context_dispatch(): "
				"Unable to release context lock.\n"
				);
	}

	/* Synchronization DONE. */

	/* Wait until we are released again. */
	while (!context->run) {
		if (pthread_cond_wait(&context->signal, &kernel_lock)) {
			posix_panic(
					"posix_context_dispatch(): "
					"Unable to wait until we are released again.\n"
					);
		}
	}

	/* If we are running we must own our context lock. */
	if (pthread_mutex_lock(&context->lock)) {
		posix_panic(
				"posix_context_dispatch(): "
				"Unable to aquire context lock.\n"
				);
	}
}

/* ************************************************************************** */

/**
 * \brief POSIX set timer function.
 *
 * \param next The next baseline.
 */
void posix_timer_set(const env_time_t *next)
{
	struct itimerspec tmp = {.it_value = *next};
	timer_settime(timer, TIMER_ABSTIME, &tmp, NULL);
}

/* ************************************************************************** */

/**
 * \brief POSIX get the timestamp.
 *
 * \return The timestamp of the most recent interrupt.
 */
env_time_t posix_timestamp(void)
{
	/* Return the last timestamp. */
	return timestamp;
}

/* ************************************************************************** */

/**
 * \brief POSIX interrupt handler install.
 *
 * \param id The id to install the handle for.
 * \param handler The handler to install.
 */
void posix_ext_interrupt_handler(int id, posix_ext_interrupt_handler_t handler)
{
	assert(id < POSIX_NUM_INTERRUPTS);
	posix_interrupt_vector[id] = handler;
}

/* ************************************************************************** */

/**
 * \brief POSIX interrupt generator.
 */
void posix_ext_interrupt_generate(int id)
{
	if (pthread_mutex_lock(&interrupt_lock)) {
		posix_panic(
				"posix_ext_interrupt_generate(): "
				"Unable to aquire interrupt lock.\n"
				);
	}

	posix_interrupt = id;

	if (pthread_mutex_lock(&kernel_lock)) {
		posix_panic(
				"posix_ext_interrupt_generate(): "
				"Unable to aquire kernel lock.\n"
				);
	}

	/* If interrupts are disabled, wait until they are enabled again. */
	while (!interrupts_enabled) {
		if (pthread_cond_wait(&interrupt_enabled_signal, &kernel_lock)) {
			posix_panic(
					"posix_ext_interrupt_generate(): "
					"Unable to wait until we can run the interrupt.\n"
					);
		}
	}
	assert(pthread_mutex_trylock(&kernel_lock));

	/* Interrupt synchornization START. */

	ack_clear(interrupt_ack);

	/* Pre-empt the current thread. */
	if (pthread_kill(tt_current->thread, SIGUSR1)) {
		posix_panic(
				"posix_ext_interrupt_generate(): "
				"Unable to deliver signal to thread.\n"
				);
	}

	/*
	 * The thread should now enter the signal handler so let's release the
	 * kernel_lock so that it may enter protected mode.
	 */
	if (pthread_mutex_unlock(&kernel_lock)) {
		posix_panic(
				"posix_ext_interrupt_generate(): "
				"Unable to unlock kernel lock.\n"
				);
	}

	/* Wait until the thread acknowledges the interrupt. */
	ack_wait(interrupt_ack, 1);

	/* Interrupt synchornization DONE. */

	if (pthread_mutex_unlock(&interrupt_lock)) {
		posix_panic(
				"posix_ext_interrupt_generate(): "
				"Unable to release the interrupt lock.\n"
				);
	}

	/*
	 * Yield the thread that generated the interrupt, so that other
	 * interrupts are generated aswell.
	 */
	if (sched_yield()) {
		posix_panic(
				"posix_ext_interrupt_generate(): "
				"Unable to yield the interrupt thread.\n"
				);
	}
}
