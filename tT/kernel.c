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
 * \brief The TinyTimber Kernel implementation.
 */

/*
 * C-lib headers, not needed but makes life easier.
 */
#include <string.h>

#if defined TT_TIMBER

/*
 * GC header, only required when running against the "real" Timber language.
 */
#include <gc.h>

#endif /* TT_TIMBER */

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
#	include <kernel.h>
#endif

/*
 * Just so that the user is aware that he gone did something stupid.
 */
#if defined ENV_SRP
#	error Compiling regular TinyTimber against SRP environment.
#endif

/* ************************************************************************** */

/**
 * \brief TinyTimber message0.
 *
 * Used to get a baseline/deadline for interrupts.
 */

static tt_message_t msg0;

/* ************************************************************************** */

/**
 * \brief TinyTimber no argument argument.
 *
 * Just export a single char that will be copied when the user request that no
 * arguments should be passed.
 *
 * \note
 *	Might be worth nothing that 42 has no "meaning", we never look at the
 *	value, only the address.
 */
char tt_args_none = 42;

/* ************************************************************************** */

/*
 * Timber code use it's own messages structure.
 */
#if ! defined TT_TIMBER

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
#endif /* __STDC_VERSION && __STDC_VERSION >= 19991L */
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
static void *memset(
	void *ptr,
	int n,
	size_t n
	)
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
static void memcpy(
	void *dest,
	const void *src,
	size_t n
	)
{
	unsigned char *d = dest, *s = src;
	while (n--) {
		*d++ = *s++;
	}
	return dest;
}
#endif /* TT_CLIB_DISABLE */

#endif /* TT_TIMBER */

/* ************************************************************************** */

/**
 * \brief TinyTimber idle context.
 */
static tt_thread_t thread_idle;

/* ************************************************************************** */

/**
 * \brief TinyTimber current thread.
 */
tt_thread_t *tt_current;

/* ************************************************************************** */

/**
 * \brief TinyTimber thread pool.
 */
static tt_thread_t thread_pool[ENV_NUM_THREADS];

/* ************************************************************************** */

/**
 * \brief TinyTimber thread housekeeper structure.
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
	tt_thread_t *idle;
} threads = {0};

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
 * \brief TinyTimber helper macro to access the current thread.
 */
#define CURRENT() (tt_current)

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
static ENV_CODE_FAST ENV_INLINE void enqueue_by_deadline(
	tt_message_t **list,
	tt_message_t *msg
	)
{
	tt_message_t *prev = NULL;
	tt_message_t *tmp = *list;

	/* Find where to place the message. */
	while (tmp && ENV_TIME_LE(tmp->deadline, msg->deadline)) {
		/* Next item in the list. */
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
static ENV_CODE_FAST ENV_INLINE void enqueue_by_baseline(
	tt_message_t **list,
	tt_message_t *msg
	)
{
	tt_message_t *prev = NULL;
	tt_message_t *tmp = *list;

	/* Find where to place the message. */
	while (tmp && ENV_TIME_LE(tmp->baseline, msg->baseline)) {
		/* Next item in the list. */
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
 * \brief TinyTimber run thread function.
 *
 * This is the entry point for all environment threads.
 */
static ENV_CODE_FAST void tt_thread_run(void)
{
	tt_thread_t *tmp;
	tt_message_t *this;

	for (;;) {
		/*
		 * This must always be entered in protected mode. Also we can
		 * not always control when/how interrupts are enabled so we
		 * must always disable them here.
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

#if ! defined TT_TIMBER
		/*
		 * We use a different method of canceling the messages/receipts
		 * when we run against the "real" Timber language.
		 */
		if (this->receipt) {
			this->receipt->msg = NULL;
		}
#endif

		CURRENT()->msg = this;

		TT_SANITY(this->to);
		TT_SANITY(this->method);

		ENV_PROTECT(0);
		/*tt_request(this->to, this->method, &this->arg);*/
		TT_MESSAGE_RUN(this);
		ENV_PROTECT(1);

		TT_SANITY(threads.active == CURRENT());
		TT_SANITY(this == CURRENT()->msg);

#if ! defined TT_TIMBER
		/*
		 * Again, when we run against the "real" Timber language
		 * we will be using GC to collect the messages.
		 */
		ENQUEUE(messages.free, this);
#endif

		/*
		 * If there are no more messages or the previous message has
		 * earlier or equal deadline to head of the list we will run
		 * previous thread. IF the previous thread is the idle thread
		 * we will run it unconditionally.
		 */

		/* 
		 * If there are no more active messages we yeild, in the future
		 * we might sleep/idle instead but that requires some changes
		 * to the code that is non-trivial.
		 */
		if (messages.active == NULL) {
			goto yield;
		}


		/*
		 * Check for pre-empted threads. If there are none we will run
		 * the next message.
		 */
		if (!CURRENT()->next) {
			continue;
		}

		/*
		 * Check if the deadline of the pre-empted thread is earlier
		 * than the deadline of the next message.
		 */
		if (
			ENV_TIME_LE(
				CURRENT()->next->msg->deadline,
				messages.active->deadline
				)
			) {
			goto yield;
		}

		/*
		 * Ok, the next message must exist and have a deadline that is
		 * earlier than the pre-empted thread. Let's run it.
		 */
		continue;

yield:
		/*
		 * Ok to yield we place the head of active threads into the
		 * inactive thread list. If there are pre-empted threads then
		 * we will run the first one in the list, otherwise we will run
		 * the idle thread.
		 */

		/* 
		 * This thread should no longer be active, place it in the
		 * inactive list for re-use.
		 */
		DEQUEUE(threads.active, tmp);
		ENQUEUE(threads.inactive, tmp);

		/*
		 * If there are not pre-empted threads we must dispatch the
		 * idle thread, if there are pre-empted threads then run the
		 * most recently pre-empted one(should have the shortest
		 * baseline).
		 */
		if (threads.active) {
			/*
			 * There are pre-empted threads, however they might be
			 * blocked. There must be at least one thread that is
			 * unblocked or the system is seriously broken.
			 */
			tmp = threads.active;
			while (tmp->waits_for) {
				tmp = tmp->waits_for->owned_by;
			}
			TT_SANITY(tmp);
			ENV_CONTEXT_DISPATCH(tmp);
			TT_SANITY(ENV_ISPROTECTED());
		} else {
			/*
			 * No pre-empted threads, dispatch the idle thread.
			 */
			ENV_CONTEXT_DISPATCH(threads.idle);
			TT_SANITY(ENV_ISPROTECTED());
		}

		/*
		 * Note that will will not get here until the thread is
		 * dispatched again.
		 */
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

#if ! defined TT_TIMBER
	/*
	 * Setup the message housekeeping structure, the memset() is not
	 * neccesary in theory but in practice we will need it. The reason for
	 * this is that not all compilers honor the static initialization
	 * (namely C18).
	 */
	messages.active = NULL;
	messages.inactive = NULL;
	memset(message_pool, 0, sizeof(message_pool));
	messages.free = message_pool;
	for (i=0;i<TT_NUM_MESSAGES;i++) {
		message_pool[i].next = &message_pool[i+1];
	}
	message_pool[TT_NUM_MESSAGES-1].next = NULL;
#endif

	/* 
	 * Setup the idle thread, we do not call ENV_CONTEXT_INIT() on this
	 * since any intialization is deferred to the ENV_IDLE() macro. All we
	 * do is set the tt_current pointer to the idle thread. Again the
	 * memset is not neccesary in theory but we need it in practice.
	 */
	memset(&thread_idle, 0, sizeof(thread_idle));
	threads.idle = &thread_idle;
	tt_current = threads.idle;

	/*
	 * Setup the "worker" threads, again memset() not needed in theory etc.
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
 */
ENV_CODE_FAST void tt_schedule(void)
{
	tt_thread_t *tmp;

	TT_SANITY(ENV_ISPROTECTED());

	/*
	 * If there are no active messages then we return, in theory we
	 * shouldn't call this unless there are messages that need scheduling
	 * but better safe than sorry.
	 */
	if (!messages.active) {
		return;
	}

	/*
	 * If the current thread is the idle thread then we will
	 * unconditionally run a new thread since idle has the lowest priority
	 * of all the threads.
	 */
	if (tt_current == threads.idle) {
		goto schedule_new;
	}

	/*
	 * Check if the deadline of the next message is earlier than the
	 * deadline of the last activated thread(may not be CURRENT()).
	 */
	if (
		ENV_TIME_LE(
			threads.active->msg->deadline,
			messages.active->deadline
			)
		) {
		return;
	}

schedule_new:

	/*
	 * Schedule a new thread, first of all make sure there is a thread to
	 * schedule then go for it.
	 */
	if (!threads.inactive) {
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
	tt_current = tmp;
#endif
}

/* ************************************************************************** */

/**
 * \brief TinyTimber expired function.
 *
 * Function will place any expired messages in the active list. It is up to the
 * callee to run tt_schedule().
 *
 * \param now The time that caused the interrupt.
 */
void ENV_CODE_FAST tt_expired(env_time_t now)
{
	tt_message_t *tmp;

	TT_SANITY(ENV_ISPROTECTED());

	/*
	 * Push all the inactive messages that became active onto the
	 * active list.
	 */
	while (
		messages.inactive &&
		ENV_TIME_LE(messages.inactive->baseline, now)
		) {
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
}

/* ************************************************************************** */

/**
 * \brief TinyTimber lock code.
 *
 * Will leave the object point to by to in a locked state.
 *
 * \param to The object to lock.
 */
#if ! defined TT_TIMBER
static ENV_CODE_FAST void tt_lock(tt_object_t *object)
#else
ENV_CODE_FAST tt_object_t *tt_lock(tt_object_t *object)
#endif
{
	tt_thread_t *tmp;
	tt_thread_t *old_wanted_by;
	int protected = ENV_ISPROTECTED();

#if defined TT_TIMBER
	/*
	 * If we are using the "real" Timber language then the gc-info
	 * pointer might be a forward pointer and thus we need to use
	 * that instead of the to object.
	 */
	object = gc_prologue(object);
#endif /* TT_TIMBER */

	ENV_PROTECT(1);

	/*
	 * If the object is owned (locked) by something else we will let
	 * that something else run until it is done with the object.
	 */
	tmp = object->owned_by;
	if (tmp) {
		/*
		 * Object that owns the requested object might in turn be
		 * blocked.
		 */
		while (tmp->waits_for) {
			tmp = tmp->waits_for->owned_by;
		}

		/*
		 * Deadlocks can only be caused by circular references, thus we
		 * can easily check it in this manner.
		 */
		if (CURRENT() == tmp) {
			ENV_PANIC("tt_request(): Deadlock.\n");
		}

		/*
		 * If someone else wants this then we must save this for later
		 * use, we have (implicitly) shorted deadline.
		 */
		old_wanted_by = object->wanted_by;
		object->wanted_by = CURRENT();
		CURRENT()->waits_for = object;

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
		 * If there was someone waiting for the object then we will
		 * make sure that it is no longer waiting for the object.
		 */
		if (old_wanted_by) {
			old_wanted_by->waits_for = NULL;
		}
	}

	/*
	 * Take ovnership of the object and invoke the method on it. We
	 * perform this action in the same protected state as we entered
	 * this function in (we only enter this function in a protected
	 * state if we are in an interrupt handler).
	 */
	object->owned_by = CURRENT();
	ENV_PROTECT(protected);

#if defined TT_TIMBER
	/*
	 * We only need to return the object if we are compiled for
	 * the "real" Timber language.
	 */
	return object;
#endif
}

/* ************************************************************************** */

/**
 * \brief TinyTimber unlock code.
 *
 * Will leave the object point to by to in an unlocked state.
 *
 * \param to The object to unlock.
 */
#if ! defined TT_TIMBER
static ENV_CODE_FAST void tt_unlock(tt_object_t *object)
#else
ENV_CODE_FAST void tt_unlock(tt_object_t *object)
#endif /* TT_TIMBER */
{
	tt_thread_t *tmp;
	int protected = ENV_ISPROTECTED();

#if defined TT_TIMBER
	/*
	 * If running against the "real" Timber language we must do some
	 * gc-magic before we unlock the object.
	 */
	gc_epilogue(object);
#endif /* TT_TIMBER */

	ENV_PROTECT(1);
	object->owned_by = NULL;

	/*
	 * If we ran on account of someone else we must then they have a
	 * shorter deadline and must thus be allowed to run now.
	 */
	tmp = object->wanted_by;
	if (tmp) {
		object->wanted_by = NULL;
		tmp->waits_for = NULL;
		ENV_CONTEXT_DISPATCH(tmp);
		TT_SANITY(ENV_ISPROTECTED());
	}

	ENV_PROTECT(protected);
}

/* ************************************************************************** */

/*
 * We only expose the tt_request function if we are running as TinyTinber.
 * Otherwise we will be exposing the tt_lock()/tt_unlock() functions as
 * non-static functions.
 */
#if ! defined TT_TIMBER

/**
 * \brief TinyTimber request function.
 *
 * Perform a synchronus call upon an object, this ensures the state integrity
 * of the object. Usually called using the TT_SYNC() macro, or directly from
 * the tt_thread_run() function.
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
ENV_CODE_FAST env_result_t tt_request(
	tt_object_t *to,
	tt_method_t method,
	void *arg
	)
{
	env_result_t result;

	TT_SANITY(to);
	TT_SANITY(method);

	tt_lock(to);

	result = method(to, arg);

	tt_unlock(to);

	return result;
}

#endif /* TT_TIMBER */

/* ************************************************************************** */

#if ! defined TT_TIMBER
static ENV_CODE_FAST void tt_async(
	tt_message_t *msg,
	env_time_t bl,
	env_time_t dl
	)
#else
ENV_CODE_FAST void tt_async(
	tt_message_t *msg,
	env_time_t bl,
	env_time_t dl
	)
#endif
{
	int protected = ENV_ISPROTECTED();
	env_time_t now = ENV_TIMER_GET();
	env_time_t base = CURRENT()->msg->baseline;

	TT_SANITY(msg);

	/*
	 * For now all deadlines/baselines are relative.
	 *
	 * TODO:
	 * 	Fully implement the new time semantics, with inherrited
	 * 	deadlines/baselines.
	 */
	msg->baseline = ENV_TIME_ADD(base, bl);
	if (ENV_TIME_LT(msg->baseline, now)) {
		msg->baseline = now;
	}

	msg->deadline = ENV_TIME_ADD(msg->baseline, dl);

	ENV_PROTECT(1);

	/*
	 * If baseline expired already then we should place the message in
	 * the active list, otherwise the inactive list.
	 */
	if (ENV_TIME_LE(msg->baseline, now)) {
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

/*
 * If we are using the "real" Timber language then we will not be exposing
 * the tt_action but rather the primitive tt_async as a non-static function.
 */
#if ! defined TT_TIMBER

/**
 * \page tt_action_example Example usage.
 * \include tt_action.c
 */

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
 * \subpage tt_action_example
 *
 * \param bl Baseline of the message.
 * \param dl Deadline of the message.
 * \param to Object that should be called.
 * \param method Method that should be called upon the object.
 * \param arg The argument(s) for the call.
 * \param size The size of the argument(s).
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
	tt_message_t *msg;
	tt_message_t *old_msg = NULL;

	TT_SANITY(to);
	TT_SANITY(method);
	TT_SANITY(arg);
	TT_SANITY(size);
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

	/* Only copy the arguments if there are any none. */
	if (arg != &tt_args_none) {
		memcpy(&msg->arg, arg, size);
	}

	/* To and method should always be present of course. */
	msg->to = to;
	msg->method = method;

	/*
	 * The base (used to calculate the baseline of the message) is
	 * depending on the state, if we are protected then we where called
	 * from an interrupt handler and the baseline should be set relative
	 * to the time when the interrupt was triggerted. The deadline is
	 * implicitly depending on this since it's realtive to the baseline.
	 */
	if (protected) {
		old_msg = CURRENT()->msg;
		CURRENT()->msg = &msg0;
		msg0.deadline = msg0.baseline = ENV_TIMESTAMP();
	}

	/* Place the message in the correct queue. */
	tt_async(msg, bl, dl);

	/* Restore any old message (if we where called from and interrupt. */
	if (old_msg) {
		CURRENT()->msg = old_msg;
	}

	ENV_PROTECT(protected);
}

#endif /* TT_TIMBER */

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
		 * We must check if we removed the head of any list and update
		 * the list accordingly.
		 */

		if (prev) {
			prev->next = tmp->next;
		} else {
			/*
			 * Message was the head of some list, update
			 * accordingly.
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
		 * Message is now free and the receipt is no longer valid. We
		 * should also return 0 to indicate success.
		 */
		ENQUEUE(messages.free, tmp);
		receipt->msg = NULL;
		result = 0;
	}

	ENV_PROTECT(protected);
	return result;
}
