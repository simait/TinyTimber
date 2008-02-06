#include <tT.h>
#include <env.h>

#include "app_objects.h"

typedef struct hello_t 
{
	tt_object_t obj;
} hello_t;
#define hello(oid, req) {tt_object(oid, req)}

hello_t hello = hello(HELLO, 0);

tt_object_t *app_objects[APP_OBJECT_ID_MAX] = {
	(tt_object_t*)&hello
};

static env_result_t hello_world(hello_t *, void *);

static env_result_t hello_world(hello_t *self, void *args)
{
	ENV_DEBUG("Hello World!\r\n");
	TT_WITHIN(ENV_SEC(1), ENV_MSEC(500), self, hello_world, TT_ARGS_NONE);
	return 0;
}
static void init(void)
{
	TT_WITHIN(ENV_SEC(0), ENV_MSEC(500), &hello, hello_world, TT_ARGS_NONE);
}

ENV_STARTUP(init);
