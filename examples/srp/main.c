#include <stdio.h>
#include <unistd.h>

#include <tT.h>
#include <env.h>

#include "app_objects.h"

typedef struct test_t 
{
	tt_object_t obj;
	int id;
} test_t;
#define test(id, oid, req) {tt_object(oid, req), id}

test_t object_a = test(0, OBJECT_A, 0);
test_t object_b = test(1, OBJECT_B, ENV_RESOURCE(OBJECT_A));
test_t object_c = test(2, OBJECT_C, ENV_RESOURCE(OBJECT_B));
test_t object_d = test(3, OBJECT_D, 0);

tt_object_t *app_objects[APP_OBJECT_ID_MAX] = {
	(tt_object_t*)&object_a,
	(tt_object_t*)&object_b,
	(tt_object_t*)&object_c,
	(tt_object_t*)&object_d
};

static env_result_t test_cb(tt_object_t *self, void *args)
{
	fprintf(stderr, "Starting ID %d\n", ((test_t*)self)->id);
	for (;;) {
		if (!sleep(2))
			break;
	}
	fprintf(stderr, "ID  %d is done\n", ((test_t*)self)->id);
	return 0;
}

static void init(void)
{
	TT_AFTER_BEFORE(ENV_SEC(1), ENV_SEC(10), &object_a, test_cb, TT_ARGS_NONE);
	TT_AFTER_BEFORE(ENV_SEC(2), ENV_SEC(5), &object_b, test_cb, TT_ARGS_NONE);
	TT_AFTER_BEFORE(ENV_SEC(3), ENV_SEC(3), &object_c, test_cb, TT_ARGS_NONE);
	TT_AFTER_BEFORE(ENV_SEC(4), ENV_SEC(1), &object_d, test_cb, TT_ARGS_NONE);
}

ENV_STARTUP(init);
