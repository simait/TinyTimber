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

#include <tT.h>
#include <env.h>

#include <objects_srp.h>

#include <app_objects.h>

static void object_calc_req(tt_object_t *object) {
	int i;
	tt_object_t **tmp = app_objects;
	env_resource_t mask = 1;

	for (i=0;i<APP_OBJECT_ID_MAX;i++,mask <<= 1,tmp++) {
		if (mask > object->resource.req) {
			break;
		}

		if (mask & object->resource.req) {
			if (!(*tmp)->resource.req & (*tmp)->resource.id) {
				/* Requirements for this object not yet calculated. */
				object_calc_req(*tmp);
			}
			
			/* Merge resource requirements. */
			object->resource.req |= (*tmp)->resource.req;
		}
	}

	/* 
	 * Set the object resource id in the requirements field to indicate
	 * that recursive calculation has already been done.
	 */
	object->resource.req |= object->resource.id;
}

/**
 * \brief TinyTimber SRP object requirement calculation.
 *
 * Performs a recursive object requirement calculation. Easier for the
 * programmer 
 */
void tt_objects_init(void)
{
	int i;
	tt_object_t **tmp = app_objects;

	if (APP_OBJECT_ID_MAX >= ENV_RESOURCE_MAX) {
		ENV_PANIC("tt_object_init(): Maximum number of objects exceeded.\n");
	}
	
	/*
	 * For each object we will traverse the list of requirements and
	 * add them to the list recursively.
	 */
	for (i=0;i<APP_OBJECT_ID_MAX;i++,tmp++) {
		if (ENV_RESOURCE(i) != (*tmp)->resource.id) {
			ENV_PANIC("tt_objects_init(): ID missmatch, check app_objects.\n");
		}

		if ((*tmp)->resource.req & (*tmp)->resource.id) {
			/* Requirements already calculated. */
			continue;
		}

		object_calc_req(*tmp);
	}
}
