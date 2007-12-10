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
 * C-lib headers, not needed but makes life easier.
 */
#include <string.h>

/*
 * Following files should not be included in case the file is mangled.
 */
#if ! defined TT_MANGLED
/*
 * Environment specific eaders.
 */
#	include <types.h>
#	include <env.h>
#	include <range.h>

/*
 * tinyTimber specific headers.
 */
#	include <kernel.h>
#endif

/**
 * \brief tinyTimber no argument argument.
 *
 * Just export a single char that will be copied when the user request
 * that no arguments should be passed.
 */
char tt_args_none = 42;

/* ************************************************************************** */

/**
 * \brief tinyTimber message structure type.
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
	 * \brief tinyTimber argument buffer.
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
	while (n--)
		*((signed char *)tmp++) = n;
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
	while (n--)
		*d++ = *s++;
	return dest;
}
#endif

/* ************************************************************************** */

/**
 * \brief tinyTimber idle context.
 */
static env_context_t thread_idle;

/* ************************************************************************** */

/**
 * \brief tinyTimber current thread.
 */
env_context_t *tt_current;

/* ************************************************************************** */

/**
 * \brief tinyTimber thread pool.
 */
static tt_thread_t thread_pool[ENV_NUM_THREADS];

/* ************************************************************************** */

/**
 * \brief tinyTimber thread housekeeper structure.
 */
static struct
{
	/**
	 * \brief List of active threads.
	 */
	tt_thread_t *active;

	/**
	 * \brief List of inactive threads.
	 */
	tt_thread_t *inactive;

	/**
	 * \brief Idle context.
	 */
	env_context_t *idle;
} threads = {0};

/* ************************************************************************** */

/**
 * \brief tinyTimber message pool.
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
 * \brief tinyTimber messages housekeeper structure.
 */
static struct
{
	/**
	 * \brief List of active messages.
	 */
	tt_message_t *active;

	/**
	 * \brief List of inactive messages.
	 */
	tt_message_t *inactive;

	/**
	 * \brief List of free messages.
	 */
	tt_message_t *free;
} messages;

/* ************************************************************************** */

/**
 * \brief tinyTimber helper macro to access the current thread.
 */
#define CURRENT() ((tt_thread_t*)tt_current)

/* ************************************************************************** */

/**
 * \brief tinyTimber dequeue/pop macro.
 */
#define DEQUEUE(list, item) \
do\
{\
	item = list;\
	list = list->next;\
} while (0)

/* ************************************************************************** */

/**
 * \brief tinyTimber enqueue/push macro.
 */
#define ENQUEUE(list, item) \
do\
{\
	item->next = list;\
	list = item;\
} while (0)

/* ************************************************************************** */

/**
 * \brief tinyTimber enqueue by deadline function.
 *
 * \param list List to enqueue into.
 * \param msg Message to qneue.
 */
static ENV_CODE_FAST ENV_INLINE void enqueue_by_deadline(tt_message_t **list, tt_message_t *msg)
{
	tt_message_t *prev = NULL;
	tt_message_t *tmp = *list;

	/* Find where to place the message. */
	while (
			tmp &&
			ULONG_LE(tmp->deadline, msg->deadline)
		  )
	{
		/* Next item in the list. */
		prev = tmp;
		tmp = tmp->next;
	}

	/* Insert the message into the list, check for head etc. */
	msg->next = tmp;
	if (prev)
		prev->next = msg;
	else
		*list = msg;
}

/* ************************************************************************** */

/**
 * \brief tinyTimber enqueue by baseline function.
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
			(ULONG_LE(tmp->baseline, msg->baseline))
		  )
	{
		/* Next item in the list. */
		prev = tmp;
		tmp = tmp->next;
	}

	/* Insert the message into the list, check for head etc. */
	msg ->next = tmp;
	if (prev)
		prev->next = msg;
	else
		*list = msg;
}

/* ************************************************************************** */

/**
 * \brief tinyTimber run thread function.
 *
 * This is the entry point for all environment threads.
 */
static ENV_CODE_FAST void tt_thread_run(void)
{
	tt_thread_t *tmp;
	tt_message_t *this;

	for (;;)
	{
		/*
		 * This must always be entered in protected mode. Also we can not
		 * always control when/how interrupts are enabled so we must always
		 * disable them here.
		 *
		 * Not entierly true anymore but let's keep it for now.
		 */
		ENV_PROTECT(1);

		TT_SANITY(messages.active);
		TT_SANITY(threads.active == CURRENT());

		/*
		 * messages.active should always be the correct message to run,
		 * also cancel any receipt.
		 */
		DEQUEUE(messages.active, this);
		if (this->receipt)
			this->receipt->msg = NULL;
		CURRENT()->msg = this;

		TT_SANITY(this->to);
		TT_SANITY(this->method);

		ENV_PROTECT(0);
		tt_request(this->to, this->method, &this->arg);
		ENV_PROTECT(1);

		TT_SANITY(threads.active == CURRENT());
		TT_SANITY(this == CURRENT()->msg);

		ENQUEUE(messages.free, this);

		/*
		 * If there are no more messages or the previous message
		 * has earlier or equal deadline to head of the list we will
		 * run previous thread. IF the previous thread is the idle
		 * thread we will run it unconditionally.
		 */

		/* 
		 * If there are no more active messages we yeild, in the future
		 * we might sleep/idle instead but that requires some changes
		 * to the code that is non-trivial.
		 */
		if (messages.active == NULL)
			goto yield;


		/*
		 * Check for pre-empted threads. If there are none we will run
		 * the next message.
		 */
		if (!CURRENT()->next)
			continue;

		/*
		 * Check if the deadline of the pre-empted thread is earlier than
		 * the deadline of the next message.
		 */
		if (ULONG_LE(CURRENT()->next->msg->deadline, messages.active->deadline))
			goto yield;

		/*
		 * Ok, the next message must exist and have a deadline that is earlier
		 * than the pre-empted thread. Let's run it.
		 */
		continue;

yield:
		/*
		 * Ok to yield we place the head of active threads into the inactive
		 * thread list. If there are pre-empted threads then we will run
		 * the first one in the list, otherwise we will run the idle thread.
		 */

		/* 
		 * This thread should no longer be active, place it in the
		 * inactive list for re-use.
		 */
		DEQUEUE(threads.active, tmp);
		ENQUEUE(threads.inactive, tmp);

		/*
		 * If there are not pre-empted threads we must dispatch the idle
		 * thread, if there are pre-empted threads then run the most
		 * recently pre-empted one(should have the shortest baseline).
		 */
		if (threads.active)
		{
			/*
			 * There are pre-empted threads, however they might be
			 * blocked. There must be at least one thread that is
			 * unblocked or the system is seriously broken.
			 */
			tmp = threads.active;
			while (tmp->waits_for)
				tmp = tmp->waits_for->owned_by;
			TT_SANITY(tmp);
			ENV_CONTEXT_DISPATCH(tmp);
			TT_SANITY(ENV_ISPROTECTED());
		}
		else
		{
			/*
			 * No pre-empted threads, dispatch the idle thread.
			 */
			ENV_CONTEXT_DISPATCH(threads.idle);
			TT_SANITY(ENV_ISPROTECTED());
		}

		/*
		 * Note that will will not get here until the thread is dispatched
		 * again.
		 */
	}
}

/* ************************************************************************** */

/**
 * \brief The tinyTimber init function.
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
	messages.active = NULL;
	messages.inactive = NULL;
	memset(message_pool, 0, sizeof(message_pool));
	messages.free = message_pool;
	for (i=0;i<TT_NUM_MESSAGES;i++)
		message_pool[i].next = &message_pool[i+1];
	message_pool[TT_NUM_MESSAGES-1].next = NULL;

	/* 
	 * Setup the idle thread, we do not call ENV_CONTEXT_INIT() on this
	 * since any intialization is deferred to the ENV_IDLE() macro. All we
	 * do is set the tt_current pointer to the idle thread. Again the
	 * memset is not neccesary in theory but we need it in practice.
	 */
	memset(&thread_idle, 0, sizeof(thread_idle));
	threads.idle = &thread_idle;
	tt_current = (env_context_t *)threads.idle;

	/*
	 * Setup the "worker" threads, again memset() not needed in theory
	 * etc.
	 */
	memset(thread_pool, 0, sizeof(thread_pool));
	threads.active = NULL;
	threads.inactive = thread_pool;
	for (i=0;i<ENV_NUM_THREADS;i++) {
		thread_pool[i].next = &thread_pool[i+1];
		ENV_CONTEXT_INIT(
				&thread_pool[i].context,
				ENV_STACKSIZE,
				tt_thread_run
				);
	}
	thread_pool[ENV_NUM_THREADS-1].next = NULL;
}

/* ************************************************************************** */

/**
 * \brief The tinyTimber run function.
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
	if (messages.active)
		ENV_TIMER_SET(messages.active->baseline);
	ENV_TIMER_START();

	/*
	 * Enter the environment defined idle state, this should set up the
	 * tt_current context and leave protected mode.
	 */
	ENV_IDLE();
}

/* ************************************************************************** */

/**
 * \brief tinyTimber schedule function.
 */
ENV_CODE_FAST void tt_schedule(void)
{
	tt_thread_t *tmp;

	TT_SANITY(ENV_ISPROTECTED());

	/*
	 * If there are no active messages then we return, in theory we
	 * shouldn't call this unless there are messages that need
	 * scheduling but better safe than sorry.
	 */
	if (!messages.active)
		return;

	/*
	 * If the current thread is the idle thread then we will
	 * unconditionally run a new thread since idle has the lowest
	 * priority of all the threads.
	 */
	if (tt_current == threads.idle)
		goto schedule_new;

	/*
	 * Check if the deadline of the next message is earlier than the deadline
	 * of the last activated thread(may not be CURRENT()).
	 */
	if (ULONG_LE(threads.active->msg->deadline, messages.active->deadline))
		return;

schedule_new:

	/*
	 * Schedule a new thread, first of all make sure there is a thread to
	 * schedule then go for it.
	 */
	if (!threads.inactive)
	{
		TT_SANITY(threads.idle);
		ENV_PANIC("tt_schedule(): Out of threads.\n");
	}

	/*
	 * Activate a new thread and implicitly dispatch it.
	 */
	DEQUEUE(threads.inactive, tmp);
	ENQUEUE(threads.active, tmp);

#if defined ENV_CONTEXT_NOT_SAVED
	ENV_CONTEXT_DISPATCH(&tmp->context);
	TT_SANITY(ENV_ISPROTECTED());
#else
	tt_current = &tmp->context;
#endif
}

/* ************************************************************************** */

/**
 * \brief tinyTimber expired function.
 *
 * Function will place any expired messages in the active list. It is up
 * to the callee to run tt_schedule().
 *
 * \param now The time that caused the interrupt.
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
	while (	messages.inactive && ULONG_LE(messages.inactive->baseline, now))
	{
		DEQUEUE(messages.inactive, tmp);
		enqueue_by_deadline(&messages.active, tmp);
	}

	/*
	 * If there are still inactive messages update the timer to the next
	 * absolute baseline.
	 */
	if (messages.inactive)
		ENV_TIMER_SET(messages.inactive->baseline);

	return old_head != messages.active;
}

/* ************************************************************************** */

/**
 * \brief tinyTimber request function.
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
	tt_thread_t *tmp;
	tt_thread_t *old_wanted_by;
	env_result_t result;
	int protected = ENV_ISPROTECTED();

	TT_SANITY(to);
	TT_SANITY(method);

	ENV_PROTECT(1);

	/*
	 * If the object is owned (locked) by something else we will let
	 * that something else run until it is done with the object.
	 */
	tmp = to->owned_by;
	if (tmp)
	{
		/*
		 * Object that owns the requested object might in turn be
		 * blocked.
		 */
		while (tmp->waits_for)
			tmp = tmp->waits_for->owned_by;

		/*
		 * Deadlocks can only be caused by circular references, thus we
		 * can easily check it in this manner.
		 */
		if (CURRENT() == tmp)
			ENV_PANIC("tt_request(): Deadlock.\n");

		/*
		 * If someone else wants this then we must save this for later
		 * use, we have (implicitly) shorted deadline.
		 */
		old_wanted_by = to->wanted_by;
		to->wanted_by = CURRENT();
		CURRENT()->waits_for = to;

		/*
		 * Allow the first unblocked thread to run until our object is
		 * released. Note that this does not mean that the first
		 * unblocked thread is actually in possession of the object,
		 * only that if we run it our object will eventually be
		 * released.
		 */
		ENV_CONTEXT_DISPATCH(tmp);
		TT_SANITY(ENV_ISPROTECTED());

		/*
		 * If there was someone waiting for the object then we will make
		 * sure that it is no longer waiting for the object.
		 */
		if (old_wanted_by)
			old_wanted_by->waits_for = NULL;
	}

	/*
	 * Take ovnership of the object and invoke the method on it. We
	 * perform this action in the same protected state as we entered
	 * this function in (we only enter this function in a protected
	 * state if we are in an interrupt handler).
	 */
	to->owned_by = CURRENT();
	ENV_PROTECT(protected);
	result = method(to, arg);
	ENV_PROTECT(1);
	to->owned_by = NULL;

	/*
	 * If we ran on account of someone else we must then they have a
	 * shorter deadline and must thus be allowed to run now.
	 */
	tmp = to->wanted_by;
	if (tmp)
	{
		to->wanted_by = NULL;
		tmp->waits_for = NULL;
		ENV_CONTEXT_DISPATCH(tmp);
		TT_SANITY(ENV_ISPROTECTED());
	}

	ENV_PROTECT(protected);
	return result;
}

/* ************************************************************************** */

/**
 * \brief tinyTimber action function.
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
	TT_SANITY(size);

	ENV_PROTECT(1);

	/* This is _VERY_ important, this can and will f*ck up. */
	if (!messages.free)
		ENV_PANIC("tt_action(): Out of messages.\n");

	DEQUEUE(messages.free, msg);
	msg->receipt = receipt;
	if (receipt)
		receipt->msg = msg;

	/*
	 * The base (used to calculate the baseline of the message) is
	 * depending on the state, if we are protected then we where called
	 * from an interrupt handler and the baseline should be set relative
	 * to the time when the interrupt was triggerted. The deadline is
	 * implicitly depending on this since it's realtive to the baseline.
	 */
	if (protected)
		base = ENV_TIMESTAMP();
	else
		base = CURRENT()->msg->baseline;

#if defined ENV_PREEMPTIVE_INTERRUPTS
	/*
	 * Ok, this is new code, so new it's not in use _EVER_. So please
	 * don't go defining ENV_PREEMPTIVE_INTERRUPTS, and if you do well
	 * tough luck.
	 */
	ENV_PROTECT(0);
#else
	/*
	 * If we were called from a non-protected context we can always
	 * leave protected mode, if however we were called from a protected
	 * context we cannot leave it unless the environment can handle
	 * pre-emptive interrupts. We have as of today no guidelines for how
	 * this might be handled.
	 */
	ENV_PROTECT(protected);
#endif

	/*
	 * For now all deadlines/baselines are relative.
	 *
	 * TODO:
	 *	We need to figure out in what way we want to be ablo to post messages
	 *	in, soft_irq, no_deadline etc. are proposed message types. We should
	 *	sit down and talk about it some day...
	 *
	 * TODO2:
	 * 	Fully implement the new time semantics, with inherrited
	 * 	deadlines/baselines.
	 */
	msg->baseline = base + bl;
	if (ULONG_LT(msg->baseline, ENV_TIMER_GET()))
		msg->baseline = ENV_TIMER_GET();

	msg->deadline = msg->baseline + dl;
	msg->to = to;
	msg->method = method;

	TT_SANITY(size <= TT_ARGS_SIZE);

	memcpy(&msg->arg, arg, size);

	ENV_PROTECT(1);

	/*
	 * If baseline expired already then we should place the message in
	 * the active list, otherwise the inactive list.
	 */
	if (ULONG_LE(msg->baseline, ENV_TIMER_GET()))
		enqueue_by_deadline(&messages.active, msg);
	else
	{
		enqueue_by_baseline(&messages.inactive, msg);
		if (messages.inactive == msg)
			ENV_TIMER_SET(msg->baseline);
	}

	ENV_PROTECT(protected);
}

/* ************************************************************************** */

/**
 * \brief tinyTimber tt_cancel function.
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
	if (receipt->msg)
	{
		tmp = messages.inactive;
		while (tmp && tmp != receipt->msg)
		{
			prev = tmp;
			tmp = tmp->next;
		}

		if (!tmp)
		{
			tmp = messages.active;
			while (tmp && tmp != receipt->msg)
			{
				prev = tmp;
				tmp = tmp->next;
			}

			if (!tmp)
				ENV_PANIC("tt_cancel(): Unable to find message.\n");
		}

		/*
		 * We must check if we removed the head of any list and update the
		 * list accordingly.
		 */

		if (prev)
			prev->next = tmp->next;
		else
		{
			/*
			 * Message was the head of some list, update accordingly.
			 */
			if (tmp == messages.inactive)
			{
				messages.inactive = messages.inactive->next;
				if (messages.inactive)
					ENV_TIMER_SET(messages.inactive->baseline);
			}
			else
				messages.active = messages.active->next;
		}

		/*
		 * Message is now free and the receipt is no longer valid. We should
		 * also return 0 to indicate success.
		 */
		ENQUEUE(messages.free, tmp);
		receipt->msg = NULL;
		result = 0;
	}

	TT_SANITY(ENV_ISPROTECTED());

	ENV_PROTECT(protected);
	return result;
}
