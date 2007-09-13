#include <tT.h>
#include <env.h>

#include <string.h>

typedef struct hello_t
{
	tt_object_t obj;
} hello_t;

static hello_t hello;

static env_result_t hello_cb(tt_object_t *obj, void *arg)
{
	TT_AFTER(ENV_SEC(1), obj, hello_cb, TT_ARGS_NONE);

	ENV_DEBUG("Hello World!\n");

	return 0;
}

void init(void)
{
	memset(&hello, 0, sizeof(hello));
	TT_AFTER_BEFORE(ENV_USEC(500), ENV_SEC(1),&hello, hello_cb, TT_ARGS_NONE);
}

ENV_STARTUP(init);
