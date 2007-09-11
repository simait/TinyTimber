#include <tT/tT.h>
#include <env/env.h>

#include <string.h>

struct hello_obj
{
	tt_object_t obj;
};
#define hello_obj() {tt_object()}

static struct hello_obj hello = hello_obj();

static env_result_t hello_cb(tt_object_t *obj, void *arg)
{
	TT_AFTER(ENV_SEC(1), obj, hello_cb, TT_ARGS_NONE);

	ENV_DEBUG("Hello World!\n");

	return 0;
}

void init(void)
{
	TT_AFTER_BEFORE(ENV_USEC(500), ENV_SEC(1),&hello, hello_cb, TT_ARGS_NONE);
}

ENV_STARTUP(init);
