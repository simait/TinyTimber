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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <env.h>
#include <kernel_srp.h>

/* ************************************************************************** */

/** \cond */

static sigset_t sigmask_protect;
static sigset_t sigmask_unprotect;
static pthread_t root;

/*
 * Sort of private internal but used in the header file.
 */
int posix_srp_protected;
env_time_t posix_srp_timer_timestamp;
timer_t posix_srp_timer;
env_time_t posix_time_inherit = {0};

/* ************************************************************************** */

static void interrupt_handler(int sig)
{
	switch (sig) {
		case SIGALRM:
			tt_expired(posix_srp_timer_get());
			tt_schedule();
			break;
	
		default:
			posix_srp_panic("Unknown singal caught.\n");
			break;
	}
}

/** \endcond */

/* ************************************************************************** */

void posix_srp_init(void)
{
	struct sigevent timer_event;
	struct sigaction signal_action;

	sigfillset(&sigmask_protect);
	sigdelset(&sigmask_protect, SIGINT);

	sigfillset(&sigmask_unprotect);
	sigdelset(&sigmask_unprotect, SIGINT);
	sigdelset(&sigmask_unprotect, SIGALRM);
	sigdelset(&sigmask_unprotect, SIGUSR1);
	
	posix_srp_protected = 1;
	pthread_sigmask(SIG_SETMASK, &sigmask_protect, NULL);

	memset(&signal_action, 0, sizeof(signal_action));
	signal_action.sa_handler = interrupt_handler;
	sigemptyset(&signal_action.sa_mask);
	sigaction(SIGALRM, &signal_action, NULL);
	sigaction(SIGUSR1, &signal_action, NULL);

	memset(&timer_event, 0, sizeof(timer_event));
	timer_event.sigev_notify = SIGEV_SIGNAL;
	timer_event.sigev_signo = SIGALRM;

	timer_create(CLOCK_REALTIME, &timer_event, &posix_srp_timer);

	clock_gettime(CLOCK_REALTIME, &posix_srp_timer_timestamp);

	root = pthread_self();
}

/* ************************************************************************** */

void posix_srp_print(const char * const msg)
{
	fprintf(stderr, "%s", msg);
}

/* ************************************************************************** */

void posix_srp_panic(const char * const msg)
{
	fprintf(stderr, "%s", msg);
	abort();
}

/* ************************************************************************** */

void posix_srp_protect(int state)
{
	if (state) {
		pthread_sigmask(SIG_SETMASK, &sigmask_protect, NULL);
		posix_srp_protected = 1;
	} else {
		posix_srp_protected = 0;
		pthread_sigmask(SIG_SETMASK, &sigmask_unprotect, NULL);
	}
}

/* ************************************************************************** */

void posix_srp_idle(void)
{
	posix_srp_protect(0);
	for (;;) {
		pause();
	}
}
