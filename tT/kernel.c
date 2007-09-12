/*
 * C-lib headers, not needed but makes life easier.
 */
#include <string.h>

/*
 * Environment specific eaders.
 */
#include <types.h>
#include <env.h>
#include <range.h>

/*
 * tinyTimber specific headers.
 */
#include <tT.h>
#include <kernel.h>

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
		 * \brief Argument buffer.
		 */
		char buf[TT_ARGS_SIZE];

		/**
		 * \brief Pointer for alignment.
		 */
		void *___ptr;

		/**
		 * \brief long for alignment.
		 */
		long ___long;

#if defined __STDC_VERSION__ && __STDC_VERSION >= 19991L
		/**
		 * \brief long long for alignment.
		 */
		long long ___long_long;
#endif
	} arg;
};

/* ************************************************************************** */

/**
 * \brief tinyTimber idle context.
 */
env_context_t thread_idle = {0};

/* ************************************************************************** */

/**
 * \brief tinyTimber current thread.
 */
env_context_t *tt_current;

/* ************************************************************************** */

/**
 * \brief tinyTimber thread pool.
 */
tt_thread_t thread_pool[ENV_NUM_THREADS] = {{{0}}};

/* ************************************************************************** */

/**
 * \brief tinyTimber thread housekeeper structure.
 */
struct
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
tt_message_t message_pool[TT_NUM_MESSAGES] = {{0}};
#ifdef ENV_PIC18
#	pragma idata
#endif

/* ************************************************************************** */

/**
 * \brief tinyTimber messages housekeeper structure.
 */
struct
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
} messages = {0};

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
		 */
		ENV_PROTECT(1);

		/* Sanity checks are good(tm). */
		TT_SANITY(messages.active);
		TT_SANITY(threads.active == CURRENT());

		/* Grab first active message. */
		DEQUEUE(messages.active, this);

		/* Cancel the receipt if there is one. */
		if (this->receipt)
			this->receipt->msg = NULL;

		/* Sanity checks are good(tm). */
		TT_SANITY(this->to);
		TT_SANITY(this->method);

		/* Set the current message pointer. */
		CURRENT()->msg = this;

		/* Leave protected mode again. */
		ENV_PROTECT(0);

		/* Perform the request. */
		tt_request(this->to, this->method, (void *)&this->arg.buf);

		/* Enter protected mode again. */
		ENV_PROTECT(1);

		/* Sanity checks are good(tm). */
		TT_SANITY(threads.active == CURRENT());
		TT_SANITY(this == CURRENT()->msg);

		/* Recycle the message. */
		ENQUEUE(messages.free, this);

		/*
		 * If there are no more messages or the previous message
		 * has earlier or equal deadline to head of the list we will
		 * run previous thread. IF the previous thread is the idle
		 * thread we will run it unconditionally.
		 */

		/* Check for active messages. */
		if (messages.active == NULL)
			goto yield;


		/*
		 * Check for pre-empted threads. If there are none just run the
		 * next message.
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

		/* Place the current thread onto the inactive stack. */
		DEQUEUE(threads.active, tmp);
		ENQUEUE(threads.inactive, tmp);

		/* Check for pre-empted threads. */
		if (threads.active)
		{
			/*
			 * Check for blocked threads. Must be done since we depend on
			 * blocked threads not running until they have access to the
			 * object that they where waiting for.
			 */
			tmp = threads.active;
			while (tmp->waits_for)
				tmp = tmp->waits_for->owned_by;
			ENV_CONTEXT_DISPATCH(tmp);
		}
		else
		{
			/* Ok, let's idle. */
			ENV_CONTEXT_DISPATCH(threads.idle);
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

	/* Initialize the environment. */
	ENV_INIT();

	/* Sanity checks are good(tm). */
	TT_SANITY(ENV_ISPROTECTED());

	/* Initialize message structure and list. */
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
	 * do is set the tt_current pointer to the idle thread.
	 */
	memset(&thread_idle, 0, sizeof(thread_idle));
	threads.idle = &thread_idle;
	tt_current = (env_context_t *)threads.idle;

	/* Initialize the "worker" threads. */
	memset(thread_pool, 0, sizeof(thread_pool));
	threads.inactive = thread_pool;
	for (i=0;i<ENV_NUM_THREADS;i++)
	{
		/* Set the next link. */
		thread_pool[i].next = &thread_pool[i+1];

		/* Initialize the context. */
		ENV_CONTEXT_INIT(
				&thread_pool[i].context,
				ENV_STACKSIZE,
				tt_thread_run
				);
	}
	thread_pool[ENV_NUM_THREADS-1].next = NULL;

	/* We have no active threads. */
	threads.active = NULL;
}

/* ************************************************************************** */

/**
 * \brief The tinyTimber run function.
 */
void tt_run(void)
{
	if (messages.active)
		ENV_TIMER_SET(messages.active->baseline);

	/* Start timer service. */
	ENV_TIMER_START();

	/*
	 * Make the environment enter the "idle" state, also send in the
	 * idle context in case the environment wishes to modify it.
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

	/* Sanity checks are good(tm). */
	TT_SANITY(ENV_ISPROTECTED());

	/* Check for active message that might need to be scheduled. */
	if (!messages.active)
		return;

	/*
	 * Check if we are idle, if we run the next message in the active list
	 * unconditionally.
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
		/* Sanity checks are good(tm). */
		TT_SANITY(threads.idle);
		ENV_PANIC("tt_schedule(): Out of threads.\n");
	}

	/* Dispatch a new thread. */
	DEQUEUE(threads.inactive, tmp);
	ENQUEUE(threads.active, tmp);

	/*
	 * We must assume that this function is called with the context saved.
	 */
	tt_current = &tmp->context;
}

/* ************************************************************************** */

/**
 * \brief tinyTimber expired function.
 *
 * Function will place any expired messages in the active list.
 *
 * \param now The time that caused the interrupt.
 */
void ENV_CODE_FAST tt_expired(env_time_t now)
{
	tt_message_t *tmp;

	/* Sanity checks are good(tm). */
	TT_SANITY(ENV_ISPROTECTED());

	/*
	 * Push all the inactive messages that became active onto the
	 * active list.
	 */
	while (	messages.inactive && ULONG_LE(messages.inactive->baseline, now))
	{
		/* Grab the active message. */
		DEQUEUE(messages.inactive, tmp);
		enqueue_by_deadline(&messages.active, tmp);
	}

	/* Schedule a new timer only if we have anything in the inactive queue. */
	if (messages.inactive)
		ENV_TIMER_SET(messages.inactive->baseline);
}

/* ************************************************************************** */

/**
 * \brief tinyTimber request function.
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

	/* Sanity check. */
	TT_SANITY(to);
	TT_SANITY(method);

	/* Enter protected mode. */
	ENV_PROTECT(1);

	/* Check if the object is locked. */
	tmp = to->owned_by;
	if (tmp)
	{
		/* Find first non-blocked thread. */
		while (tmp->waits_for)
			tmp = tmp->waits_for->owned_by;

		/* Check for deadlock. */
		if (CURRENT() == tmp)
			ENV_PANIC("tt_request(): Deadlock.\n");

		/* Save the previous thread on the wishlist. */
		old_wanted_by = to->wanted_by;

		/* Place ourself on the wishlist. */
		to->wanted_by = CURRENT();
		CURRENT()->waits_for = to;

		/* Run the thread that has locked our object. */
		ENV_CONTEXT_DISPATCH(tmp);

		/* Sanity checks are good(tm). */
		TT_SANITY(ENV_ISPROTECTED());

		/*
		 * Make sure the previous thread on the wish list doesn't
		 * wait for the object anymore, if there was one.
		 */
		if (old_wanted_by)
			old_wanted_by->waits_for = NULL;
	}

	/* Take ownership of the object. */
	to->owned_by = CURRENT();

	/* Make the call in unprotected mode if we can. */
	ENV_PROTECT(protected);
	result = method(to, arg);
	ENV_PROTECT(1);

	/* Release ownership of the object. */
	to->owned_by = NULL;

	/*
	 * Let's see if we ran on the account of someone else.
	 */
	tmp = to->wanted_by;
	if (tmp)
	{
		/*
		 * Cleanup the links and dispatch the thread that wanted us
		 * to run.
		 */
		to->wanted_by = NULL;
		tmp->waits_for = NULL;
		ENV_CONTEXT_DISPATCH(tmp);

		/* Sanity checks are good(tm). */
		TT_SANITY(ENV_ISPROTECTED());
	}

	/* Leave the same way that we entered. */
	ENV_PROTECT(protected);

	/* Return the result of the call. */
	return result;
}

/* ************************************************************************** */

/**
 * \brief tinyTimber action function.
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
	unsigned char *t0, *t1;
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

	/* Grab a message from the free list. */
	DEQUEUE(messages.free, msg);

	/*
	 * If user wants a receipt, user gets a receipt.
	 * NOTE: The reason we do this here is that this needs to be atomic.
	 */
	msg->receipt = receipt;
	if (receipt)
		receipt->msg = msg;

	/*
	 * Calculate what our current time perspective is, timestamp
	 * or current baseline.
	 */
	if (protected)
		base = ENV_TIMESTAMP();
	else
		base = CURRENT()->msg->baseline;

#if defined ENV_PREEMPTIVE_INTERRUPTS
	/*
	 * Need to draw up some guidelines for what the environment must
	 * guarantee for us to be preemtive here.
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
	 */
	msg->baseline = base + bl;
	msg->deadline = base + bl + dl;

	/* Set the message call variables. */
	msg->to = to;
	msg->method = method;

	/* Sanity checks are good(tm). */
	TT_SANITY(size <= TT_ARGS_SIZE);

	/* Copy the argument. */
	t0 = (void *)msg->arg.buf;
	t1 = (void *)arg;
	while (size--)
		*t0++ = *t1++;
	/*memcpy(msg->arg.buf, arg, size);*/

	/* Enter protected mode. */
	ENV_PROTECT(1);

	/*
	 * Check where to place the message, either in active or inactive
	 * queue.
	 */
	if (ULONG_LE(msg->baseline, ENV_TIMER_GET()))
		enqueue_by_deadline(&messages.active, msg);
	else
	{
		enqueue_by_baseline(&messages.inactive, msg);

		/*
		 * If we placed this message as the head then we must set a new
		 * timer value as well.
		 */
		if (messages.inactive == msg)
			ENV_TIMER_SET(msg->baseline);
	}

	/* Leave the same way we entered. */
	ENV_PROTECT(protected);
}

/* ************************************************************************** */

/**
 * \brief tinyTimber tt_cancel function.
 *
 * \param receipt The receipt that should be canceled.
 * \return zero upon success, non-zero upon failure.
 */
ENV_CODE_FAST int tt_cancel(tt_receipt_t *receipt)
{
	int result = 1;
	int protected = ENV_ISPROTECTED();
	tt_message_t *prev, *tmp;

	/* Enter protected mode. */
	ENV_PROTECT(1);

	/* Check if the receipt is valid. */
	if (receipt->msg)
	{
		/* Search inactive queue. */
		prev = NULL;
		tmp = messages.inactive;
		while (tmp && tmp != receipt->msg)
		{
			prev = tmp;
			tmp = tmp->next;
		}

		/*
		 * If it's not in the inactive queue then we might be lucky enough
		 * to find it in the active queue.
		 */
		if (!tmp)
		{
			/* Search active queue. */
			prev = NULL;
			tmp = messages.active;
			while (tmp && tmp != receipt->msg)
			{
				prev = tmp;
				tmp = tmp->next;
			}

			/*
			 * If it's not in active or inactive queue we have stumbled upon
			 * a BUG.
			 */
			TT_SANITY(tmp);
		}

		/*
		 * We must check if we removed the head of any list and update the
		 * list accordingly.
		 */

		if (prev)
		{
			/* Not the head of any list, somewhere in the middle. */
			prev->next = tmp->next;
		}
		else
		{
			if (tmp == messages.inactive)
			{
				messages.inactive = messages.inactive->next;

				/*
				 * Set new timeout if there is a head, otherwise set it
				 * to the current time(should be an already expired timeout).
				 */
				if (messages.inactive)
					ENV_TIMER_SET(messages.inactive->baseline);
				else
					ENV_TIMER_SET(ENV_TIMER_GET());
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

	/* Sanity checks are good(tm). */
	TT_SANITY(ENV_ISPROTECTED());

	/* Leave the same way we entered. */
	ENV_PROTECT(protected);

	/* Return result. */
	return result;
}
