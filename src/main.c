#include <tT/tT.h>
#include <tT/kernel.h>
#include <env/env.h>

#include <string.h>

struct hello_obj
{
	tt_object_t obj;
} hello = {{0}}/*, hello2*/;

env_result_t hello_cb(tt_object_t *obj, void *arg)
{
	TT_AFTER(ENV_SEC(1), obj, hello_cb, TT_ARGS_NONE);

	ENV_DEBUG("Hello World!\n");

	return 0;
}

#if 0
env_result_t hello_cb2(tt_object_t *obj, void *arg)
{
	TT_AFTER(ENV_SEC(1), obj, hello_cb2, TT_ARGS_NONE);

	ENV_DEBUG("asdf!\n");

	return 0;
}
#endif

#if defined ENV_ARM7
void irq_test(void)
{
	ENV_MARK();
	TT_ASYNC(&hello, hello_cb, TT_ARGS_NONE);
	tt_schedule();
}
#elif defined ENV_AVR
__attribute__ ((signal, used, naked)) void TIMER1_COMPB_vect(void)
{
	AVR_CONTEXT_SAVE(avr_interrupt_return);
	TT_ASYNC(&hello, hello_cb, TT_ARGS_NONE);
	tt_schedule();
	AVR_CONTEXT_RESTORE();
}
#elif defined ENV_MSP430
__attribute__((naked)) interrupt(TIMERB1_VECTOR) msp430_timerb_compare1(void)
{
	MSP430_CONTEXT_SAVE();
	TT_ASYNC(&hello, hello_cb, TT_ARGS_NONE);
	tt_schedule();
	MSP430_CONTEXT_RESTORE();
	asm volatile ("reti");
}
#elif defined ENV_PIC18
void pic18_irq_test(void)
{
	TT_ASYNC(&hello, hello_cb, TT_ARGS_NONE);
	tt_schedule();
}
#endif

void init(void)
{
	memset(&hello, 0, sizeof(hello));
	/*memset(&hello2, 0, sizeof(hello2));*/
	TT_AFTER_BEFORE(ENV_USEC(500), ENV_SEC(1),&hello, hello_cb, TT_ARGS_NONE);
	TT_SANITY(ENV_ISPROTECTED());
	/*TT_AFTER_BEFORE(ENV_USEC(550), ENV_MSEC(900), &hello2, hello_cb2, TT_ARGS_NONE);
	TT_SANITY(ENV_ISPROTECTED());*/
}

ENV_STARTUP(init);
