#ifndef ENV_PIC18_ENV_H_
#define ENV_PIC18_ENV_H_

/* TODO: is the name of the pic18 generic file p18fxxx? */
#include <p18f4620.h>
#include <usart.h>

/*
 * Following files should not be included in case the file is mangled.
 */
#if ! defined TT_MANGLED
#	include <tT/kernel.h>
#endif

/* ************************************************************************** */

void pic18_init(void);

void pic18_context_init(pic18_context_t *, size_t, tt_thread_function_t); 
void pic18_context_dispatch(static pic18_context_t *);
void pic18_idle(void);

void pic18_timer_start(void);
env_time_t pic18_timer_get(void);
void pic18_timer_set(env_time_t);

void pic18_print(const rom char *);
void pic18_print_var(unsigned long);

/* ************************************************************************** */

extern env_time_t pic18_timestamp;

extern env_ext_interrupt_t pic18_interrupt_handler_low;
extern env_ext_interrupt_t pic18_interrupt_handler_high;

/* ************************************************************************** */

/**
 * \brief The offset of the tinyTimber stack.
 */
#define PIC18_STACK_OFFSET		0xa00

/* ************************************************************************** */

/**
 * \cond
 *
 * We need theese to save/restore the context.
 */
#define PIC18_TEMP_START		0x0
#define PIC18_TEMP_SIZE			0xb
#define PIC18_TEMP_END			PIC18_TEMP_START+PIC18_TEMP_SIZE-1
#define PIC18_MATH_START		0x13
#define PIC18_MATH_SIZE			0x8
#define PIC18_MATH_END			PIC18_MATH_START+PIC18_MATH_SIZE-1

/**
 * \endcond
 */

/* ************************************************************************** */

/**
 * \brief PIC18 init macro.
 */
#define ENV_INIT() \
	pic18_init()

/* ************************************************************************** */

/**
 * \brief PIC18 protect macro.
 */
#define ENV_PROTECT(state) \
	INTCONbits.GIEL = !state

/* ************************************************************************** */

/**
 * \brief PIC18 isprotected macro.
 */
#define ENV_ISPROTECTED() \
	(!INTCONbits.GIEL)

/* ************************************************************************** */

/**
 * \brief PIC18 debug macro.
 */
#define ENV_DEBUG(msg) \
	pic18_print((const rom char *)msg)

/* ************************************************************************** */

/**
 * \brief PIC18 panic macro.
 */
#define ENV_PANIC(msg) \
do\
{\
	ENV_PROTECT(1);\
	ENV_DEBUG(msg);\
	for (;;);\
} while (0)

/* ************************************************************************** */

#if 0
/**
 * \brief PIC18 debug variable macro.
 */
#define ENV_DEBUG_VAR(var) \
do\
{\
	ENV_DEBUG(#var ": 0x");\
	pic18_print_var((unsigned long)var);\
	ENV_DEBUG("\n");\
} while (0)
#endif

/* ************************************************************************** */

/**
 * \brief PIC18 context init macro.
 */
#define ENV_CONTEXT_INIT(context) \
	pic18_context_init((pic18_context_t *)context)

/* ************************************************************************** */

/**
 * \brief PIC18 dispatch macro.
 */
#define ENV_CONTEXT_DISPATCH(thread) \
	pic18_context_dispatch((pic18_context_t *)thread)

/* ************************************************************************** */

/**
 * \brief The tinyTimber context cookie.
 */
#define PIC18_CONTEXT_COOKIE	0x55aa

/* ************************************************************************** */

/**
 * \brief PIC18 context save macro.
 */
#define PIC18_CONTEXT_SAVE(return_addr) \
do\
{\
	_asm\
		/* Save the STATUS and WREG first. */\
		movff	WREG, PREINC1\
		movff	STATUS, PREINC1\
		/* Save less critical registers but registers that must be saved. */\
		movff	BSR, PREINC1\
		movff	FSR2L, PREINC1\
		movff	FSR2H, PREINC1\
		movff	FSR0L, PREINC1\
		movff	FSR0H, PREINC1\
		movff	TABLAT, PREINC1\
		movff	TBLPTRU, PREINC1\
		movff	TBLPTRH, PREINC1\
		movff	TBLPTRL, PREINC1\
		movff	PRODH, PREINC1\
		movff	PRODL, PREINC1\
		movff	PCLATU, PREINC1\
		movff	PCLATH, PREINC1\
		/*\
		 * Save the .tempdata and MATH_DATA, we must have a correct size for\
		 * these or we _WILL_ fuck things up.\
		 */\
		lfsr	0, PIC18_TEMP_START\
		movlw	PIC18_TEMP_SIZE\
		temp_save_next:\
		decf	WREG, 0, 0\
		bnc		temp_save_done\
		movff	POSTINC0, PREINC1\
		bra		temp_save_next\
		temp_save_done:\
		lfsr	0, PIC18_MATH_START\
		movlw	PIC18_MATH_SIZE\
		math_save_next:\
		decf	WREG, 0, 0\
		bnc		math_save_done\
		movff	POSTINC0, PREINC1\
		bra		math_save_next\
		math_save_done:\
		/* Set the return address/method. */\
		push\
	_endasm\
		TOS = (unsigned long short)return_addr;\
	_asm\
		/* Save the call stack. */\
		movff	STKPTR, FSR0L\
		movf	STKPTR, 0, 0\
		stack_save_next:\
		decf	WREG, 0, 0\
		bnc		stack_save_done\
		movff	TOSL, PREINC1\
		movff	TOSH, PREINC1\
		movff	TOSU, PREINC1\
		pop\
		bra		stack_save_next\
		stack_save_done:\
		movff	FSR0L, PREINC1\
		/* Write the SP to tt_current. */\
		movff	tt_current, FSR0L\
		movff	tt_current+1, FSR0H\
		movff	FSR1L, POSTINC0\
		movff	FSR1H, POSTINC0\
		/* Check the context cookie. */\
		movff	POSTINC0, PREINC1\
		movff	POSTINC0, PREINC1\
		movff	POSTDEC1, FSR0H\
		movff	POSTDEC1, FSR0L\
		movlw	PIC18_CONTEXT_COOKIE&0xff\
		cpfseq	POSTINC0, 0\
		call	pic18_context_cookie_panic, 0\
		movlw	(PIC18_CONTEXT_COOKIE>>8)&0xff\
		cpfseq	POSTINC0, 0\
		call	pic18_context_cookie_panic, 0\
		/* Compiler assumes stack pointer points to first free. */\
		movf	PREINC1, 0, 0\
	_endasm\
} while (0)

/* ************************************************************************** */

/**
 * \brief PIC18 context restore macro.
 */
#define PIC18_CONTEXT_RESTORE() \
do\
{\
	_asm\
		/* Check the context cookie. */\
		movff	tt_current, FSR0L\
		movff	tt_current+1, FSR0H\
		movf	POSTINC0, 0, 0\
		movf	POSTINC0, 0, 0\
		movff	POSTINC0, PREINC1\
		movff	POSTINC0, PREINC1\
		movff	POSTDEC1, FSR0H\
		movff	POSTDEC1, FSR0L\
		movlw	PIC18_CONTEXT_COOKIE&0xff\
		cpfseq	POSTINC0, 0\
		call	pic18_context_cookie_panic, 0\
		movlw	(PIC18_CONTEXT_COOKIE>>8)&0xff\
		cpfseq	POSTINC0, 0\
		call 	pic18_context_cookie_panic, 0\
		/* Grab the sp for the context. */\
		movff	tt_current, FSR0L\
		movff	tt_current+1, FSR0H\
		movff	POSTINC0, FSR1L\
		movff	POSTINC0, FSR1H\
		/* Check how many items we should restore from stack. */\
		movff	POSTDEC1, FSR0L\
		stack_restore_next:\
		decf	FSR0L, 1, 0\
		bnc		stack_restore_done\
		push\
		movf	POSTDEC1, 0, 0\
		movwf	TOSU, 0\
		movf	POSTDEC1, 0, 0\
		movwf	TOSH, 0\
		movf	POSTDEC1, 0, 0\
		movwf	TOSL, 0\
		bra		stack_restore_next\
		stack_restore_done:\
		/* Restore the MATH_DATA and .tempdata sections. */\
		lfsr	0, PIC18_MATH_END\
		movlw	PIC18_MATH_SIZE\
		math_restore_next:\
		decf	WREG, 0, 0\
		bnc		math_restore_done\
		movff	POSTDEC1, POSTDEC0\
		bra		math_restore_next\
		math_restore_done:\
		lfsr	0, PIC18_TEMP_END\
		movlw	PIC18_TEMP_SIZE\
		temp_restore_next:\
		decf	WREG, 0, 0\
		bnc		temp_restore_done\
		movff	POSTDEC1, POSTDEC0\
		bra 	temp_restore_next\
		temp_restore_done:\
		/* Restore less critical registers. */\
		movff	POSTDEC1, PCLATH\
		movff	POSTDEC1, PCLATU\
		movff	POSTDEC1, PRODL\
		movff	POSTDEC1, PRODH\
		movff	POSTDEC1, TBLPTRL\
		movff	POSTDEC1, TBLPTRH\
		movff	POSTDEC1, TBLPTRU\
		movff	POSTDEC1, TABLAT\
		movff	POSTDEC1, FSR0H\
		movff	POSTDEC1, FSR0L\
		movff	POSTDEC1, FSR2H\
		movff	POSTDEC1, FSR2L\
		movff	POSTDEC1, BSR\
		/* Restore the STATUS and WREG last. */\
		movff	POSTDEC1, WREG\
		movff	POSTDEC1, STATUS\
	_endasm\
} while (0)

/* ************************************************************************** */

/**
 * \brief PIC18 dispatch idle macro.
 */
#define ENV_IDLE(thread) \
	pic18_idle()

/* ************************************************************************** */

/**
 * \brief PIC18 timer start macro.
 */
#define ENV_TIMER_START() \
	do {T3CONbits.TMR3ON = 1;} while (0)

/* ************************************************************************** */

/**
 * \brief PIC18 timer get macro.
 */
#define ENV_TIMER_GET() \
	pic18_timer_get()

/* ************************************************************************** */

/**
 * \brief PIC18 timer set macro.
 */
#define ENV_TIMER_SET(time) \
	pic18_timer_set(time)

/* ************************************************************************** */

/**
 * \brief PIC18 timestamp macro.
 */
#define ENV_TIMESTAMP() \
	pic18_timestamp

/* ************************************************************************** */

/**
 * \brief PIC18 timer hz.
 *
 * Fosc/4/8
 */
#define ENV_TIMER_HZ() 614400UL

/* ************************************************************************** */

/**
 * \brief PIC18 timer count.
 */
#define PIC18_TIMER_COUNT 0x10000 /* 65536 */

/* ************************************************************************** */

/**
 * \brief PIC18 usec macro.
 */
#define ENV_USEC(val) \
	(((env_time_t)val*ENV_TIMER_HZ())/1000000UL)

/* ************************************************************************** */

/**
 * \brief PIC18 msec macro.
 */
#define ENV_MSEC(val) \
	(((env_time_t)val*ENV_TIMER_HZ())/1000UL)

/* ************************************************************************** */

/**
 * \brief PIC18 sec macro.
 */
#define ENV_SEC(val) \
	((env_time_t)val*ENV_TIMER_HZ())

/* ************************************************************************** */

/**
 * \brief PIC18 stack size.
 */
#define ENV_STACKSIZE 128

/* ************************************************************************** */

/**
 * \brief PIC18 idle stack size
 */
#define ENV_STACKSIZE_IDLE 128

/* ************************************************************************** */

/**
 * \brief PIC18 Number of threads.
 */
#define ENV_NUM_THREADS 5

/* ************************************************************************** */

/**
 * \brief PIC18 stack size.
 */
#define PIC18_STACKSIZE ((ENV_STACKSIZE*ENV_NUM_THREADS)+ENV_STACKSIZE_IDLE)

/* ************************************************************************** */

/**
 * \brief PIC18 startup macro.
 */
#define ENV_STARTUP(function) \
void main(void)\
{\
	/* NOTE: Do NOT!!! use stack allocated variables here. */\
	extern unsigned char pic18_stack[];\
	FSR1 = (unsigned short)pic18_stack;\
	FSR2 = (unsigned short)pic18_stack;\
	tt_init();\
	function();\
	tt_run();\
} extern char dummy


/* ************************************************************************** */

/**
 * \brief PIC18 interrupt extention for high priority interrupts.
 */
#define ENV_EXT_INTERRUPT(which, handler) \
	which = handler

#endif
