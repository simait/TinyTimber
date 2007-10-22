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

#ifndef TT_H_
#define TT_H_

#include <stddef.h>

/*
 * Following files should not be included in case the file is mangled.
 */
#if ! defined TT_MANGLED
#	include <types.h>
#endif

/* ************************************************************************** */

/**
 * \brief Forward declaration of tt_thread_t.
 */
typedef struct tt_thread_t tt_thread_t;

/* ************************************************************************** */

/**
 * \brief tinyTimber object typedef.
 */
typedef struct tt_object_t
{
	/**
	 * \brief The thread that owns this object.
	 */
	tt_thread_t *owned_by;

	/**
	 * \brief The thread that wants this object.
	 */
	tt_thread_t *wanted_by;
} tt_object_t;

/* ************************************************************************** */

/**
 * \brief tinyTimber object "constructor".
 */
#define tt_object() {NULL, NULL}

/* ************************************************************************** */

/**
 * \brief tinyTimber method signature.
 */
typedef env_result_t (*tt_method_t)(tt_object_t *, void *);

/* ************************************************************************** */

/**
 * \brief tinyTimber recepit typedef.
 */
typedef struct tt_receipt_t
{
	/**
	 * \brief
	 * 	Pointer to the data describing the message that this
	 * 	receipt point to.
	 */
	void *msg;
} tt_receipt_t;

/* ************************************************************************** */

/**
 * \brief tinyTimber flags typedef.
 *
 * The size of any flag must not exceed the maximum size of the flag type.
 */
typedef unsigned char tt_flags_t;

/* ************************************************************************** */

enum
{
	/**
	 * \brief Flag to indicate that the message has no deadline.
	 */
	TT_MSG_NO_DL = 1
};

/* ************************************************************************** */

extern char tt_args_none;

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_ARGS_NONE macro.
 *
 * Should indicate that no argument should be sent to the function.
 */
#define TT_ARGS_NONE \
	(&tt_args_none)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_ACTION() macro.
 *
 * Baseline is always relative to current time, deadline is always
 * relative to the baseline.
 */
#define TT_ACTION(bl, dl, to, meth, arg) \
	TT_ACTION_R(bl, dl, to, meth, arg, NULL)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_ACTION_R() macro.
 *
 * Same as TT_ACTION() but with receipt.
 */
#define TT_ACTION_R(bl, dl, to, meth, arg, rec) \
	tt_action(bl, dl, (tt_object_t *)to, (tt_method_t)meth, arg, sizeof(*arg), rec)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_ASYNC() macro.
 *
 * Will evaluate to TT_ACTION() with baseline 0 and deadline 0.
 */
#define TT_ASYNC(to, meth, arg) \
	TT_ASYNC_R(to, meth, arg, NULL)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_ASYNC_R() macro.
 *
 * Same as TT_ASYNC() but with receipt.
 */
#define TT_ASYNC_R(to, meth, arg, rec) \
	TT_ACTION_R(0, 0, to, meth, arg, rec)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_AFTER() macro.
 *
 * Will evaluate to TT_ACTION() with baseline bl and deadline 0.
 */
#define TT_AFTER(bl, to, meth, arg) \
	TT_AFTER_R(bl, to, meth, arg, NULL)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_AFTER_R() macro.
 *
 * Same as TT_AFTER() but with receipt.
 */
#define TT_AFTER_R(bl, to, meth, arg, rec) \
	TT_ACTION_R(bl, 0, to, meth, arg, rec)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_BEFORE() macro.
 *
 * Will evaluate to TT_ACTION() with baseline 0, and deadline dl.
 */
#define TT_BEFORE(dl, to, meth, arg) \
	TT_BEFORE_R(dl, to, meth, arg, NULL)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_BEFORE_R() macro.
 *
 * Same as TT_BEFORE() but with receipt.
 */
#define TT_BEFORE_R(dl, to, meth, arg, rec) \
	TT_ACTION_R(0, dl, to, meth, arg, rec)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_AFTER_BEFORE() macro.
 *
 * Will evaluate to TT_ACTION() with baseline bl and deadline dl.
 */
#define TT_AFTER_BEFORE(bl, dl, to, meth, arg) \
	TT_AFTER_BEFORE_R(bl, dl, to, meth, arg, NULL)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_AFTER_BEFORE_R() macro.
 *
 * Same as TT_ARFTER_BEFORE() but with receipt.
 */
#define TT_AFTER_BEFORE_R(bl, dl, to, meth, arg, rec) \
	TT_ACTION_R(bl, dl, to, meth, arg, rec)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_CANCEL() macro.
 *
 * Will cancel the message specified by the receipt, if possible.
 */
#define TT_CANCEL(rec) \
	tt_cancel(rec)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_SYNC() macro.
 *
 * Performs a synchronus call.
 */
#define TT_SYNC(to, meth, arg) \
	tt_request((tt_object_t *)to, (tt_method_t)meth, arg)

/* ************************************************************************** */

/**
 * \brief tinyTimber TT_SCHEDULE() macro.
 *
 * Just supplied this for visibility.
 */
#define TT_SCHEDULE() \
	tt_schedule()

/* ************************************************************************** */

void tt_init(void);
void tt_run(void);
env_result_t tt_request(tt_object_t *, tt_method_t, void *);
void tt_action(
		env_time_t,
		env_time_t,
		tt_object_t *,
		tt_method_t,
		void *,
		size_t,
		tt_receipt_t *
		);
int tt_cancel(tt_receipt_t *);
void tt_schedule(void);

#endif
