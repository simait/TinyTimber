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

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <posix/env.h>
#include <posix/ack.h>

/**
 * \brief The ack structure.
 */
struct ack_t
{
	/**
	 * \brief ack lock.
	 */
	pthread_mutex_t lock;

	/**
	 * \brief ack signal.
	 */
	pthread_cond_t signal;

	/**
	 * \brief ack variable.
	 */
	sig_atomic_t acked;
};


/**
 * \brief Create a new ack.
 *
 * \return The new ack, will not fail but call posix_panic().
 */
ack_t *ack_new(void)
{
	ack_t *ack;

	/* Allocate a new ack. */
	ack = calloc(1, sizeof(ack_t));
	if (!ack)
		posix_panic("Unable to allocate new ack.\n");

	/* Create the lock. */
	if (pthread_mutex_init(&ack->lock, NULL))
		posix_panic("Unable to initialize ack lock.\n");

	/* Create the signal condition. */
	if (pthread_cond_init(&ack->signal, NULL))
		posix_panic("Unable to initialize ack cond.\n");

	/* Success. */
	return ack;
}

/**
 * \brief Destroy an ack.
 *
 * \param ack The ack to destroy.
 */
void ack_delete(ack_t *ack)
{
	assert(ack);

	/* Free the mallocs. */
	free(ack);
}

/**
 * \brief Ack an ack.
 *
 * \param ack the ack to acknoledge.
 */
void ack_set(ack_t *ack)
{
	/* Aquire the lock. */
	if (pthread_mutex_lock(&ack->lock))
		posix_panic("Unable to aquire ack lock.\n");

	/* Set the acked to non-zero ie. acked. */
	ack->acked++;

	/* Signal the change. */
	if (pthread_cond_broadcast(&ack->signal))
		posix_panic("Unable to signal acked state.\n");

	/* Release the lock. */
	if (pthread_mutex_unlock(&ack->lock))
		posix_panic("Unable to release ack lock.\n");
}

/**
 * \brief Reset an ack.
 *
 * \param ack The acknowledge struct to reset.
 */
void ack_clear(ack_t *ack)
{
	assert(ack);

	/* Aquire the lock. */
	if (pthread_mutex_lock(&ack->lock))
		posix_panic("Unable to aquire ack lock.\n");

	/* Set the acked to 0 ie. not acked. */
	ack->acked = 0;

	/* Release the lock. */
	if (pthread_mutex_unlock(&ack->lock))
		posix_panic("Unable to release ack lock.\n");
}

/**
 * \brief Wait for ack condition.
 *
 * \note
 * 	When we return we will have at least the number of acks
 * 	requested.
 *
 * \param ack The acknowledge struct to wait for.
 * \param last The number of acks to receive before returning.
 */
void ack_wait(ack_t *ack, int num)
{
	assert(ack);

	/* Aquire the lock. */
	if (pthread_mutex_lock(&ack->lock))
		posix_panic("Unable to aquire ack lock.\n");

	/* Wait until we get an acknoledgment. */
	while (ack->acked < num)
	{
		/* Wait. */
		if (pthread_cond_wait(&ack->signal, &ack->lock))
			posix_panic("Unable to wait for ack.\n");
	}

	/* Someone acknoledged us. */
	if (pthread_mutex_unlock(&ack->lock))
		posix_panic("Unable to release ack lock.\n");
}

/**
 *  \brief Check if ack is set.
 *
 *  \param ack The acknowledge state to check.
 *  \return zero if no ack was set, non-zero if ack was set.
 */
int ack_acked(ack_t *ack)
{
	int acked;

	/* Aquire the lock. */
	if (pthread_mutex_lock(&ack->lock))
		posix_panic("Unable to aquire lock.\n");

	/* Grab an ack if there was one. */
	acked = ack->acked;

	/* Release the lock. */
	if (pthread_mutex_unlock(&ack->lock))
		posix_panic("Unable to release lock.\n");

	return acked;
}
