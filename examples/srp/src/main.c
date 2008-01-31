#include <tT.h>
#include <env.h>

#include "app_objects.h"

/*#define VERBOSE 1*/

typedef struct wait_t 
{
	tt_object_t obj;
	volatile char flag;
} wait_t;
#define wait(oid, req) {tt_object(oid, req), 1}

typedef struct release_t
{
	tt_object_t obj;
} release_t;
#define release(oid, req) {tt_object(oid, req)}

typedef struct announce_t
{
	tt_object_t obj;
} announce_t;
#define announce(oid, req) {tt_object(oid, req)}

wait_t object_a = wait(OBJECT_A, 0);
wait_t object_b = wait(OBJECT_B, ENV_RESOURCE(OBJECT_A));
wait_t object_c = wait(OBJECT_C, ENV_RESOURCE(OBJECT_B));
wait_t object_d = wait(OBJECT_D, 0);

release_t release = release(RELEASE, 0);

announce_t announce = announce(ANNOUNCE, 0);

tt_object_t *app_objects[APP_OBJECT_ID_MAX] = {
	(tt_object_t*)&object_a,
	(tt_object_t*)&object_b,
	(tt_object_t*)&object_c,
	(tt_object_t*)&object_d,
	(tt_object_t*)&release,
	(tt_object_t*)&announce
};

static env_result_t wait_a(wait_t *, void*);
static env_result_t wait_b(wait_t *, void*);
static env_result_t wait_c(wait_t *, void*);
static env_result_t wait_d(wait_t *, void*);
static env_result_t queue_a(announce_t *, void *);
static env_result_t queue_b(announce_t *, void *);
static env_result_t queue_c(announce_t *, void *);
static env_result_t queue_d(announce_t *, void *);
static env_result_t release_a(release_t *, char **);
static env_result_t release_b(release_t *, char **);
static env_result_t release_c(release_t *, char **);
static env_result_t release_d(release_t *, char **);

static env_result_t wait_a(wait_t *self, void *args)
{
	ENV_DEBUG("\twait_a: start\r\n");
	while (self->flag) {}
	ENV_DEBUG("\twait_a: done\r\n");
	return 0;
}

static env_result_t wait_b(wait_t *self, void *args)
{
	ENV_DEBUG("\t\twait_b: start\r\n");
	while (self->flag) {}
	ENV_DEBUG("\t\twait_b: done\r\n");
	return 0;
}

static env_result_t wait_c(wait_t *self, void *args)
{
	ENV_DEBUG("\t\t\twait_c: start\r\n");
	while (self->flag) {}
	ENV_DEBUG("\t\t\twait_c: done\r\n");
	return 0;
}

static env_result_t wait_d(wait_t *self, void *args)
{
	ENV_DEBUG("\t\t\t\twait_d: start\r\n");
	while (self->flag) {}
	ENV_DEBUG("\t\t\t\twait_d: done\r\n");
	return 0;
}

static env_result_t release_a(release_t *self, char **args)
{
	object_a.flag = 0;
	return 0;
}

static env_result_t release_b(release_t *self, char **args)
{
	object_b.flag = 0;
	return 0;
}

static env_result_t release_c(release_t *self, char **args)
{
	object_c.flag = 0;
	return 0;
}

static env_result_t release_d(release_t *self, char **args)
{
	object_d.flag = 0;
	return 0;
}

static env_result_t queue_a(announce_t *self, void *args)
{
	ENV_DEBUG("<wait_a queued>\r\n");
	return 0;
}

static env_result_t queue_b(announce_t *self, void *args)
{
	ENV_DEBUG("<wait_b queued>\r\n");
	return 0;
}

static env_result_t queue_c(announce_t *self, void *args)
{
	ENV_DEBUG("<wait_c queued>\r\n");
	return 0;
}

static env_result_t queue_d(announce_t *self, void *args)
{
	ENV_DEBUG("<wait_d queued>\r\n");
	return 0;
}

static void init(void)
{
	TT_AFTER_BEFORE(ENV_SEC(1), ENV_SEC(50), &object_a, wait_a, TT_ARGS_NONE);
	TT_AFTER_BEFORE(ENV_SEC(1), ENV_SEC(49), &announce, queue_a, TT_ARGS_NONE);
	TT_AFTER_BEFORE(ENV_SEC(5), ENV_SEC(0), &release, release_a, TT_ARGS_NONE);

	TT_AFTER_BEFORE(ENV_SEC(2), ENV_SEC(40), &object_b, wait_b, TT_ARGS_NONE);
	TT_AFTER_BEFORE(ENV_SEC(2), ENV_SEC(39), &announce, queue_b, TT_ARGS_NONE);
	TT_AFTER_BEFORE(ENV_SEC(6), ENV_SEC(0), &release, release_b, TT_ARGS_NONE);
	
	TT_AFTER_BEFORE(ENV_SEC(3), ENV_SEC(30), &object_c, wait_c, TT_ARGS_NONE);
	TT_AFTER_BEFORE(ENV_SEC(3), ENV_SEC(29), &announce, queue_c, TT_ARGS_NONE);
	TT_AFTER_BEFORE(ENV_SEC(6), ENV_SEC(0), &release, release_c, TT_ARGS_NONE);
	
	TT_AFTER_BEFORE(ENV_SEC(4), ENV_SEC(20), &object_d, wait_d, TT_ARGS_NONE);
	TT_AFTER_BEFORE(ENV_SEC(4), ENV_SEC(19), &announce, queue_d, TT_ARGS_NONE);
	TT_AFTER_BEFORE(ENV_SEC(8), ENV_SEC(0), &release, release_d, TT_ARGS_NONE);
}

ENV_STARTUP(init);
