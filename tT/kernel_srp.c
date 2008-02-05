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
 * \file
 * \brief The TinyTimber Kernel implementation (SRP).
 */

#if !defined(TT_CLIB_DISABLE)
	/*
	 * C-lib headers, not needed but makes life easier.
	 */
#	include <string.h>
#endif

/*
 * Following files should not be included in case the file is mangled.
 */
#if ! defined TT_MANGLED
	/*
	 * Environment specific eaders.
	 */
#	include <env.h>
#	include <types.h>

	/*
	 * TinyTimber specific headers.
	 */
#	include <kernel_srp.h>
#	include <objects_srp.h>
#endif

/*
 * Just so that the user is aware that he gone did something stupid.
 */
#if ! defined ENV_SRP
#	error Compiling SRP TinyTimber against regular environment.
#endif

/**
 * \brief TinyTimber no argument argument.
 *
 * Just export a single char that will be copied when the user request
 * that no arguments should be passed.
 */
char tt_args_none = 42;

/* ************************************************************************** */

/**
 * \brief TinyTimber message structure type.
 */
struct tt_message_t
{
	tt_message_t *next;

	/**
	 * \brief Baseline of message.
	 */
	env_time_t baseline;

	/**
	 * \brief Deadline of message.
	 */
	env_time_t deadline;

	/**
	 * \brief The object to perform the call upon.
	 */
	tt_object_t *to;

	/**
	 * \brief The method that should be called upon the given object.
	 */
	tt_method_t method;

	/**
	 * \brief The receipt for this message if any.
	 */
	tt_receipt_t *receipt;

	/**
	 * \brief The message flags.
	 */
	tt_flags_t flags;

	/**
	 * \brief TinyTimber argument buffer.
	 */
	union
	{
		/**
		 * \brief Variable length argument buffer.
		 */
		char buf[TT_ARGS_SIZE];

		/** \cond */
		void *___ptr;
		long ___long;
#if defined __STDC_VERSION__ && __STDC_VERSION >= 19991L
		long long ___long_long;
		/** \endcond */
#endif
	} arg;
};

/* ************************************************************************** */

/**
 * \brief TinyTimber message pool.
 */
#ifdef ENV_PIC18
#	pragma idata message_pool
#endif
static tt_message_t message_pool[TT_NUM_MESSAGES];
#ifdef ENV_PIC18
#	pragma idata
#endif

/* ************************************************************************** */

/**
 * \brief TinyTimber messages housekeeper structure.
 */
static struct
{
	/** \brief List of running messages. */
	tt_message_t *running;

	/** \brief List of active messages. */
	tt_message_t *active;

	/** \brief List of inactive messages. */
	tt_message_t *inactive;

	/** \brief List of free messages. */
	tt_message_t *free;
} messages;

/* ************************************************************************** */

static env_resource_t tt_resources;

/* ************************************************************************** */

#if TT_CLIB_DISABLE
/**
 * \brief memset implementation.
 * 
 * In case we don't want any CLIB requirements in the kernel.
 *
 * \param ptr pointer to the data.
 * \param c the constant to set each byte to.
 * \param n the number of bytes to set.
 * \return a pointer to ptr.
 */
static void *memset(void *ptr, int n, size_t n)
{
	signed char tmp = ptr;
	while (n--) {
		*((signed char *)tmp++) = n;
	}
	return ptr;
}

/* ************************************************************************** */

/**
 * \brief memcpy implementation.
 *
 * In case we don't want any CLIB dependency in the kernel.
 *
 * \param dest Destionation pointer.
 * \param src Source pointer.
 * \param n Number of byte to copy.
 * \return pointer to dest.
 */
static void memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d = dest, *s = src;
	while (n--) {
		*d++ = *s++;
	}
	return dest;
}
#endif

/* ************************************************************************** */

/**
 * \brief TinyTimber dequeue/pop macro.
 */
#define DEQUEUE(list, item) \
do {\
	item = list;\
	list = list->next;\
} while (0)

/* ************************************************************************** */

/**
 * \brief TinyTimber enqueue/push macro.
 */
#define ENQUEUE(list, item) \
do {\
	item->next = list;\
	list = item;\
} while (0)

/* ************************************************************************** */

/**
 * \brief TinyTimber enqueue by deadline function.
 *
 * \param list List to enqueue into.
 * \param msg Message to qneue.
 */
static ENV_CODE_FAST ENV_INLINE void enqueue_by_deadline(tt_message_t **list, tt_message_t *msg)
{
	tt_message_t *prev = NULL;
	tt_message_t *tmp = *list;

	/* Find where to insert the message. */
	while (
			tmp &&
			ENV_TIME_LE(tmp->deadline, msg->deadline)
		  ) {
		prev = tmp;
		tmp = tmp->next;
	}

	/* Insert the message into the list, check for head etc. */
	msg->next = tmp;
	if (prev) {
		prev->next = msg;
	} else {
		*list = msg;
	}
}

/* ************************************************************************** */

/**
 * \brief TinyTimber enqueue by baseline function.
 *
 * \param list List to enqueue into.
 * \param msg Message to enqueue.
 */
static ENV_CODE_FAST ENV_INLINE void enqueue_by_baseline(tt_message_t **list, tt_message_t *msg)
{
	tt_message_t *prev = NULL;
	tt_message_t *tmp = *list;

	/* Find where to place the message. */
	while (
			tmp &&
			ENV_TIME_LE(tmp->baseline, msg->baseline)
		  ) {
		prev = tmp;
		tmp = tmp->next;
	}

	/* Insert the message into the list, check for head etc. */
	msg ->next = tmp;
	if (prev) {
		prev->next = msg;
	} else {
		*list = msg;
	}
}

/* ************************************************************************** */

/**
 * \brief The TinyTimber init function.
 *
 * Should initialize the kernel to a state where async messages can be sent.
 * No timer should be running.
 *
 * \note
 *	Will call ENV_PANIC() upon failure.
 */
void tt_init(void)
{
	int i;

	/*
	 * We must always initialize the environment before anything else.
	 * Since ENV_CONTEXT_INIT() etc. might need it.
	 */
	ENV_INIT();

	TT_SANITY(ENV_ISPROTECTED());

	/*
	 * Setup the message housekeeping structure, the memset() is not
	 * neccesary in theory but in practice we will need it. The reason
	 * for this is that not all compilers honor the static
	 * initialization (namely C18).
	 */
	messages.running = NULL;
	messages.active = NULL;
	messages.inactive = NULL;
	memset(message_pool, 0, sizeof(message_pool));
	messages.free = message_pool;
	for (i=0;i<TT_NUM_MESSAGES;i++)
		message_pool[i].next = &message_pool[i+1];
	message_pool[TT_NUM_MESSAGES-1].next = NULL;

	/* Initialize all object requirements. */
	tt_objects_init();
}

/* ************************************************************************** */

/**
 * \brief The TinyTimber run function.
 *
 * Will, if needed, set the timer so that the first timer interrupt will
 * be generated correctly (if messages end up directly in active list
 * during ENV_STARTUP() no timer will be set). Once the first timer
 * interrupt is staged the ENV_IDLE() macro will be called so that the
 * environment can place the first (implicitly the idle thread) into the
 * idle state.
 *
 * Please note that this should _ONLY_ be invoked after the context is
 * saved. Invoking it without saving the context results in undefined
 * behavior.
 */
void tt_run(void)
{
	/*
	 * Make sure first timer interrupt is scheduled before we start the
	 * timer.
	 */
	if (messages.active) {
		ENV_TIMER_SET(messages.active->baseline);
	}
	ENV_TIMER_START();

	/*
	 * Enter the environment defined idle state, this should set up the
	 * tt_current context and leave protected mode.
	 */
	ENV_IDLE();
}

/* ************************************************************************** */

/**
 * \brief TinyTimber schedule function.
 *
 * This is the SRP version of schedule.
 */
ENV_CODE_FAST void tt_schedule(void)
{
	tt_message_t *tmp;

	TT_SANITY(ENV_ISPROTECTED());

	/* If there are not messages waiting to run then no-op. */
	if (!messages.active) {
		return;
	}

	/*
	 * Ok, we now have 2 constraints that we need to check, first is resource
	 * and secondly is the deadline. Of the two the resource has precedence
	 * in accordance with SRP.
	 */

	/* If nothing else is running then get cracking. */
	if (!messages.running) {
		goto dispatch;
	}

	/* Resource. */
	if (!ENV_RESOURCE_AVAILABLE(tt_resources, messages.active->to->resource.req)) {
		return;
	}

	/* Deadline. */
	if (!ENV_TIME_LT(messages.active->deadline, messages.running->deadline)) {
		return;
	}

dispatch:
	/* Dispatch the message at the top of the active stack. */
	DEQUEUE(messages.active, tmp);
	ENQUEUE(messages.running, tmp);

	/* Clear the receipt. */
	if (tmp->receipt) {
		tmp->receipt->msg = NULL;
	}

	/* Perform the request, this will be the "root" of the request chain. */
	ENV_INTERRUPT_PRIORITY_RESET();
	ENV_PROTECT(0);
	tt_request(tmp->to, tmp->method, tmp->arg.buf);
	ENV_PROTECT(1);
}

/* ************************************************************************** */

/**
 * \brief TinyTimber expired function.
 *
 * Function will place any expired messages in the active list. It is up
 * to the callee to run tt_schedule() (if this function returns non-zero).
 *
 * \param now The time that caused the interrupt.
 * \return non-zero if tt_schedule() should be called, otherwise zero.
 */
int ENV_CODE_FAST tt_expired(env_time_t now)
{
	tt_message_t *tmp;
	tt_message_t *old_head = messages.active;

	TT_SANITY(ENV_ISPROTECTED());

	/*
	 * Push all the inactive messages that became active onto the
	 * active list.
	 */
	while (messages.inactive && ENV_TIME_LE(messages.inactive->baseline, now)) {
		DEQUEUE(messages.inactive, tmp);
		enqueue_by_deadline(&messages.active, tmp);
	}

	/*
	 * If there are still inactive messages update the timer to the next
	 * absolute baseline.
	 */
	if (messages.inactive) {
		ENV_TIMER_SET(messages.inactive->baseline);
	}

	return old_head != messages.active;
}

/* ************************************************************************** */

/**
 * \brief TinyTimber request function.
 *
 * Perform a synchronus call upon an object, this ensures the state
 * integrity of the object. Usually called using the TT_SYNC() macro, or
 * directly from the tt_thread_run() function.
 *
 * \note
 * 	While it is possible to call this in a protected context please
 * 	don't do this as it is very error-prone and should it fail you might
 * 	have a non-circular deadlock on your hands (not detected by the
 * 	kernel).
 *
 * \param to Object that should be called.
 * \param method Method that should be called upon the given object.
 * \param arg The argument for the call.
 * \return The result of the call.
 */
ENV_CODE_FAST env_result_t tt_request(tt_object_t *to, tt_method_t method, void *arg)
{
	tt_message_t *tmp;
	env_result_t result;
	int protected = ENV_ISPROTECTED(); /* Save state for exit. */

	TT_SANITY(to);
	TT_SANITY(method);
	TT_SANITY(to->resource.id & to->resource.req);

	ENV_PROTECT(1);

	if (!ENV_RESOURCE_AVAILABLE(tt_resources, to->resource.id)) {
		/* 
		 * This should not be allowed by the tt_schedule() you probably have
		 * an invalid resource mask.
		 */
		ENV_PANIC("tt_request(): Deadlock.\n");
	}
	ENV_RESOURCE_AQUIRE(tt_resources, to->resource.id);

	ENV_PROTECT(protected);
	result = method(to, arg);
	ENV_PROTECT(1);

	ENV_RESOURCE_RELEASE(tt_resources, to->resource.id);

	/* This message was the root of a request chain. */
	if (messages.running->to == to) {
		DEQUEUE(messages.running, tmp);
		ENQUEUE(messages.free, tmp);
	}

	/* Always check if there is something more important to run. */
	tt_schedule();

	ENV_PROTECT(protected);
	return result;
}

/* ************************************************************************** */

/**
 * \brief TinyTimber action function.
 *
 * Will schedule a message with a given baseline and deadline, if the
 * baseline has expired the message is placed directly into the active
 * list instead of the inactive list. Usually called via the macros
 * TT_ASYNC(), TT_BEFORE(), TT_AFTER(), TT_AFTER_BEFORE(), and their
 * *_R() counterparts.
 *
 * \param bl Baseline of the message.
 * \param dl Deadline of the message.
 * \param to Object that should be called.
 * \param method Method that should be called upon the object.
 * \param arg The argument for the call.
 * \param receipt The receipt pointer for the message.
 */
ENV_CODE_FAST void tt_action(
		env_time_t bl,
		env_time_t dl,
		tt_object_t *to,
		tt_method_t method,
		void *arg,
		size_t size,
		tt_receipt_t *receipt
		)
{
	int protected = ENV_ISPROTECTED();
	env_time_t base;
	tt_message_t *msg;

	TT_SANITY(to);
	TT_SANITY(method);
	TT_SANITY(arg);
	TT_SANITY(size > 0);
	TT_SANITY(size <= TT_ARGS_SIZE);

	ENV_PROTECT(1);

	/* This is _VERY_ important, this can and will f*ck up. */
	if (!messages.free) {
		ENV_PANIC("tt_action(): Out of messages.\n");
	}

	DEQUEUE(messages.free, msg);
	msg->receipt = receipt;
	if (receipt) {
		receipt->msg = msg;
	}

	/*
	 * The base (used to calculate the baseline of the message) is
	 * depending on the state, if we are protected then we where called
	 * from an interrupt handler and the baseline should be set relative
	 * to the time when the interrupt was triggerted. The deadline is
	 * implicitly depending on this since it's realtive to the baseline.
	 */
	if (protected) {
		base = ENV_TIMESTAMP();
	} else {
		base = messages.running->baseline;
	}

	/*
	 * If we were called from a non-protected context we can always
	 * leave protected mode, if however we were called from a protected
	 * context we cannot leave it unless the environment can handle
	 * pre-emptive interrupts. We have as of today no guidelines for how
	 * this might be handled.
	 */
	ENV_PROTECT(protected);

	/*
	 * For now all deadlines/baselines are relative.
	 *
	 * TODO:
	 * 	Fully implement the new time semantics, with inherrited
	 * 	deadlines/baselines.
	 *
	 * TODO(2):
	 *	We need to figure out in what way we want to be ablo to post messages
	 *	in, soft_irq, no_deadline etc. are proposed message types. We should
	 *	sit down and talk about it some day...
	 */
	msg->baseline = ENV_TIME_ADD(base, bl);
	if (ENV_TIME_LT(msg->baseline, ENV_TIMER_GET())) {
		msg->baseline = ENV_TIMER_GET();
	}

	msg->deadline = ENV_TIME_ADD(msg->baseline, dl);
	msg->to = to;
	msg->method = method;

	/* Only copy the arguments if there are none. */
	if (arg != &tt_args_none) {
		memcpy(&msg->arg, arg, size);
	}

	/*
	 * We could probably squeeze a few iunstructions by only making the
	 * actual queue handling code protected but for now that's not really
	 * a good idea.
	 */
	ENV_PROTECT(1);

	/*
	 * If baseline expired already then we should place the message in
	 * the active list, otherwise the inactive list.
	 */
	if (ENV_TIME_LE(msg->baseline, ENV_TIMER_GET())) {
		enqueue_by_deadline(&messages.active, msg);
	} else {
		enqueue_by_baseline(&messages.inactive, msg);
		if (messages.inactive == msg) {
			ENV_TIMER_SET(msg->baseline);
		}
	}

	ENV_PROTECT(protected);
}

/* ************************************************************************** */

/**
 * \brief TinyTimber tt_cancel function.
 *
 * Will cancel a message depending on the given receipt (if it's still
 * valid).
 *
 * \param receipt The receipt that should be canceled.
 * \return zero upon success, non-zero upon failure.
 */
ENV_CODE_FAST int tt_cancel(tt_receipt_t *receipt)
{
	int result = 1;
	int protected = ENV_ISPROTECTED();
	tt_message_t *prev = NULL, *tmp;

	ENV_PROTECT(1);

	/*
	 * If the receipt is still valid we will, in this order;
	 *
	 *  - Search the inactive list for the message, and
	 *  - Search the active list for the mesage.
	 *
	 * If we cannot find the message then we have a BUG! (given that the
	 * recipet is valid).
	 */
	if (receipt->msg) {
		tmp = messages.inactive;
		while (tmp && tmp != receipt->msg) {
			prev = tmp;
			tmp = tmp->next;
		}

		if (!tmp) {
			tmp = messages.active;
			while (tmp && tmp != receipt->msg) {
				prev = tmp;
				tmp = tmp->next;
			}

			if (!tmp) {
				ENV_PANIC("tt_cancel(): Unable to find message.\n");
			}
		}

		/*
		 * We must check if we removed the head of any list and update the
		 * list accordingly.
		 */

		if (prev) {
			prev->next = tmp->next;
		} else {
			/*
			 * Message was the head of some list, update accordingly.
			 */
			if (tmp == messages.inactive) {
				messages.inactive = messages.inactive->next;
				if (messages.inactive) {
					ENV_TIMER_SET(messages.inactive->baseline);
				}
			} else {
				messages.active = messages.active->next;
			}
		}

		/*
		 * Message is now free and the receipt is no longer valid. We should
		 * also return 0 to indicate success.
		 */
		ENQUEUE(messages.free, tmp);
		receipt->msg = NULL;
		result = 0;
	}

	ENV_PROTECT(protected);
	return result;
}
