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

#include <kernel_srp.h>
#include <posix_srp/env.h>

static timer_t timer;

static sigset_t sigmask_protect;
static sigset_t sigmask_unprotect;

static struct timespec timestamp;

static int protected;

static void interrupt_handler(int sig)
{
	switch (sig) {
		case SIGALRM:
			tt_expired(posix_srp_timer_get());
			tt_schedule();
			break;
	
		default:
			fprintf(stderr, "Unknown signal caught (%d).\n", sig);
			break;
	}
}

void posix_srp_init(void)
{
	struct sigevent timer_event;
	memset(&timer_event, 0, sizeof(timer_event));

	sigfillset(&sigmask_protect);
	sigdelset(&sigmask_protect, SIGINT);

	sigfillset(&sigmask_unprotect);
	sigdelset(&sigmask_unprotect, SIGINT);
	sigdelset(&sigmask_unprotect, SIGALRM);
	sigdelset(&sigmask_unprotect, SIGUSR1);
	
	protected = 1;
	sigprocmask(SIG_SETMASK, &sigmask_protect, NULL);

	signal(SIGALRM, interrupt_handler);
	signal(SIGUSR1, interrupt_handler);

	timer_event.sigev_notify = SIGEV_SIGNAL;
	timer_event.sigev_signo = SIGALRM;

	timer_create(CLOCK_REALTIME, &timer_event, &timer);

	clock_gettime(CLOCK_REALTIME, &timestamp);
}

void posix_srp_panic(const char * const msg)
{
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

void posix_srp_timer_set(const env_time_t *next)
{
	struct itimerspec tmp = {.it_value = *next};
	timer_settime(timer, TIMER_ABSTIME, &tmp, NULL);
}

env_time_t posix_srp_timer_get()
{
	struct timespec tmp;
	clock_gettime(CLOCK_REALTIME, &tmp);
	return tmp;
}

env_time_t posix_srp_timestamp(void)
{
	return timestamp;
}

void posix_srp_protect(int state)
{
	if (state) {
		sigprocmask(SIG_SETMASK, &sigmask_protect, NULL);
		protected = 1;
	} else {
		protected = 0;
		sigprocmask(SIG_SETMASK, &sigmask_unprotect, NULL);
	}
}

int posix_srp_isprotected(void)
{
	return protected;
}

void posix_srp_idle(void)
{
	posix_srp_protect(0);
	for (;;) {
		pause();
	}
}
