/* Example structure for multiple arguments. */
typedef struct test_args_t
{
	int a0;
	unsigned short a1;
	struct
	{
		char b0;
		char b1;
	} bytes;
} test_args_t;

/* Example function. */
env_result_t test(tt_object_t *self, test_args_t *args)
{
	/* Do something with the arguments, might be more usefull than this. */
	return args->a0<<args->bytes.b0 + args->a1*args->bytes.b1;
}

/* Example invocation of test function after 1 second. */
tt_object_t object = tt_object();
test_args_t args = {0, 1, {2, 3}};
TT_AFTER(ENV_TIME_SEC(1), &object, test, &args);
