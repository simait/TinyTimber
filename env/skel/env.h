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

/**
 * \brief Skeleton environment.
 *
 * Just used for compile testing and perhaps as a template for a new
 * environment. Should compile but does not run.
 */
#ifndef ENV_SKEL_ENV_H_
#define ENV_SKEL_ENV_H_

/* ************************************************************************** */

/**
 * \brief Environment initialization.
 *
 * Should take care of initializing the environment setting up any hardware
 * required by the environment such as serial ports, timers etc.
 */
#define ENV_INIT()

/* ************************************************************************** */

/**
 * \brief Environemnt debug macro.
 *
 * May print the string pointer to by msg but it is not required.
 */
#define ENV_DEBUG(msg)

/* ************************************************************************** */

/**
 * \brief Environment panic macro.
 *
 * Must indicate that a critical error has occured and halt or reset the
 * execution.
 */
#define ENV_PANIC(msg)

/* ************************************************************************** */

/**
 * \brief Environment protect macro.
 *
 * Should place the environment in a protected mode when state is non-zero
 * otherwise it should leave protected mode.
 */
#define ENV_PROTECT(state) (void)(state)

/* ************************************************************************** */

/**
 * \brief Environment isprotected macro.
 *
 * Should evaluate to non-zero when the environment is in a protected mode
 * otherwise it should evaluate to zero.
 */
#define ENV_ISPROTECTED() 0

/* ************************************************************************** */

/**
 * \brief Environment number of threads macro.
 *
 * The number of threads that this environment can manage. Defined here to
 * one so that we can make a "fake" compile of the environment without
 * warning.
 */
#define ENV_NUM_THREADS 1

/* ************************************************************************** */

/**
 * \brief Environment context init macro.
 *
 * Should initialize a context with a certain amount of stack and running the
 * function. The definition here is just because we wish to compile the
 * skeleton environment without warnings.
 */
#define ENV_CONTEXT_INIT(context, stacksize, function) (function)()

/* ************************************************************************** */

/**
 * \brief Environment context dispatch macro.
 *
 * Should suspend the current context and start executing the specified
 * context. Must change the tt_current variable.
 */
#define ENV_CONTEXT_DISPATCH(context)

/* ************************************************************************** */

/**
 * \brief Environment idle macro.
 *
 * Should place the environment in an "idle" state. Can be busy waiting or
 * some power-saving mode. The environment must initialize the context pointerd
 * to by tt_current so that the kernel can dispatch another thread.
 */
#define ENV_IDLE()

/* ************************************************************************** */

/**
 * \brief Environment timer start macro.
 *
 * Should start the environment timer service.
 */
#define ENV_TIMER_START()

/* ************************************************************************** */

/**
 * \brief Environment timer set macro.
 *
 * Should set the time when tt_expired(time) should be called.
 */
#define ENV_TIMER_SET(time)

/* ************************************************************************** */

/**
 * \brief Environment timer get macro.
 *
 * Should evaluate to the current time.
 */
#define ENV_TIMER_GET() 0

/* ************************************************************************** */

/**
 * \brief Environment timestamp macro.
 *
 * Should evaluate to the time when the most recent interrupt was triggered.
 */
#define ENV_TIMESTAMP() 0

/* ************************************************************************** */

/**
 * \brief Environment usec macro.
 *
 * The given number of usecs should be converted to time-base of
 * the environment.
 */
#define ENV_USEC(n) 0

/* ************************************************************************** */

/**
 * \brief Environment msec macro.
 *
 * The given number of msecs should be converted to time-base of
 * the environment.
 */
#define ENV_MSEC(n) 0

/* ************************************************************************** */

/**
 * \brief Environment sec macro.
 *
 * The given number of secs should be converted to time-base of
 * the environment.
 */
#define ENV_SEC(n) 0

/**
 * \brief Environment startup macro.
 *
 * Should make the environment run the given function upon startup and/or
 * reset.
 */
#define ENV_STARTUP(function) \
int main(void)\
{\
	for (;;);\
	return 0;\
} char dummy /* Force semi-colon. */

#endif
