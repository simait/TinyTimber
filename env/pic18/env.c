/*
 * Following files should not be included in case the file is mangled.
 */
#if ! defined TT_MANGLED
#	include <env.h>
#	include <types.h>
#	include <kernel.h>
#endif

#include <usart.h>
#include <timers.h>

#define TIME_START 0xfff00000

/* ************************************************************************** */

/**
 * \brief The PIC18 timestamp variable.
 */
unsigned long pic18_timestamp = TIME_START;

/* ************************************************************************** */

/**
 * \brief The PIC18 timer base variable.
 */
unsigned long pic18_timer_base = TIME_START;

/* ************************************************************************** */

/**
 * \brief The PIC18 timer next variable.
 */
unsigned long pic18_timer_next = 0;

/* ************************************************************************** */

/**
 * \brief The PIC18 timer active variable.
 */
unsigned char pic18_timer_active = 0;

/* ************************************************************************** */

/**
 * \brief The PIC18 low priority interrupt handler.
 */
env_ext_interrupt_t pic18_interrupt_handler_low = NULL;

/* ************************************************************************** */

/**
 * \brief The PIC18 high priority interrupt handler.
 */
env_ext_interrupt_t pic18_interrupt_handler_high = NULL;

/* ************************************************************************** */

/**
 * \brief PIC18 stack.
 */
#pragma idata stack_pool
unsigned char pic18_stack[PIC18_STACKSIZE] = {0};
#pragma idata

/** \cond */

/* ************************************************************************** */

/*
 * Ok these two functions are what makes the magic happen when we restore a
 * context. We will return from the context we came from with or without
 * interrupts enabled.
 */
static void pic18_interrupt_return(void)
{
	_asm retfie 0 _endasm
}
static void pic18_normal_return(void)
{
}

/* ************************************************************************** */

/**
 * \brief Make life easier in context dispatch.
 */
static void pic18_context_cookie_panic(void)
{
	(void) pic18_context_cookie_panic;
	ENV_PANIC("pic18_context_dispatch(): Magic cookie corrupted.\n");
}

/* ************************************************************************** */

/**
 * \brief Ugly hack for the TMR3 macro below.
 */
union
{
	struct
	{
		unsigned char low;
		unsigned char high;
	};
	unsigned short value;
} tmr3 = {0};

/* ************************************************************************** */

/**
 * \brief This is just to access the TMR3 value easier.
 * 
 * Also this is to force the compiler to access the L and H values
 * in the right order to utilize the buffered 16-bit read of the timer
 * module. The TMR3L value _should_ be read _first_.
 *
 * Verified the correctness of the compiler code with version 3.10 of C18
 * in the optimizations mode Disable, Debug and All. This might still get
 * broken in future versions so keep an eye out for that.
 */
#define TMR3 \
	(tmr3.low = TMR3L, tmr3.high = TMR3H, tmr3.value)

/* ************************************************************************** */

/**
 * \brief Helper macro to turn off the CCP2 module.
 */
#define CCP2ON() \
do\
{\
	CCP2CON         = 0x0a;\
	PIR2bits.CCP2IF = 0;\
	PIE2bits.CCP2IE = 1;\
} while (0)

/* ************************************************************************** */

/**
 * \brief Helper macro to turn off the CCP2 module.
 */
#define CCP2OFF() \
do\
{\
	CCP2CON         = 0x00;\
	PIE2bits.CCP2IE = 0;\
	PIR2bits.CCP2IF = 0;\
} while (0)


/* ************************************************************************** */

/** \endcond */

/**
 * \brief The PIC18 init function.
 *
 * \todo Initialize the USART and timer/ccp.
 */
void pic18_init(void)
{
	/* 
	 * Setup the interrupts, all interrupts are by default low prio and
	 * global low priority interrupts should be disabled.
	 */
	IPR1 = IPR2 = INTCON = 0;
	RCONbits.IPEN = 1;
	INTCONbits.GIEH = 1;

	/* 
	 * Open the usart for 9600 baud and eight bit transmission,
	 * no interrupts. For more info about this call see the C18
	 * library manual.
	 */
	OpenUSART(
			USART_TX_INT_OFF & USART_RX_INT_OFF &
			USART_ASYNCH_MODE & USART_EIGHT_BIT &
			USART_CONT_RX & USART_BRGH_HIGH,
			127
			);
	
	/* Set the pins for the USART. */
	TRISCbits.TRISC6 = 0;
	TRISCbits.TRISC7 = 1;

	/*
	 * Let's use CCP2 and Timer3, this is mainly due to CCP1
	 * being an enhanced CCP that we don't really need.
	 */
	OpenTimer3(
			TIMER_INT_ON & T3_16BIT_RW &
			T3_SOURCE_INT & T1_CCP1_T3_CCP2 &
			T3_PS_1_8
			);

	/* Make sure the timer is OFF. */
	T3CONbits.TMR3ON = 0;
	TMR3L = TMR3H = 0;

	/* Reset the CCP2 module. */
	CCP2OFF();
}

/* ************************************************************************** */

/**
 * \brief The PIC18 context init function.
 */
void pic18_context_init(
		pic18_context_t *context,
		size_t stacksize,
		tt_thread_function_t function
		)
{
	int i;
	static unsigned char *pic18_stack_offset = &pic18_stack[ENV_STACKSIZE_IDLE];

	/*
	 * NOTE: stack grows UP on PIC18.
	 */
	context->sp = pic18_stack_offset;
	context->cookie = (unsigned short *)(pic18_stack_offset + stacksize - 2);
	*context->cookie = PIC18_CONTEXT_COOKIE;

	/* We need one blank, this is due to how the context save/restore works. */
	*context->sp++ = 0;

	/* Save exactly 15 special function registers. */
	for (i=0;i<15;i++)
		*context->sp++ = 0;

	/* Save some fake data for .tmpdata. */
	for (i=0;i<PIC18_TEMP_SIZE;i++)
		*context->sp++ = 0;

	/* Save some fake data for MATH_DATA. */
	for (i=0;i<PIC18_MATH_SIZE;i++)
		*context->sp++ = 0;

	/* Create the fake return path on the stack. */
	*context->sp++ = ((unsigned short long)function) & 0xff;
	*context->sp++ = ((unsigned short long)function >> 8) & 0xff;
	*context->sp++ = ((unsigned short long)function >> 16) & 0xff;
	*context->sp = 1; /* We use POSTDEC1 when we retore. */

	/* And this is where the next stack should end up. */
	pic18_stack_offset += stacksize;
}

/* ************************************************************************** */

/**
 * \brief The PIC18 context dispatch function.
 */
void pic18_context_dispatch(static pic18_context_t *context)
{
	/* 
	 * NOTE: must NOT!!! have _ANY_ variables on the stack or we are
	 * fried. Hence why parameter is static(check the C18 compiler reference
	 * manual).
	 */
	PIC18_CONTEXT_SAVE(pic18_normal_return);
	tt_current = context;
	PIC18_CONTEXT_RESTORE();
}

/* ************************************************************************** */

/**
 * \brief The pic18 idle function.
 *
 * Clear the hardware stack and setup the context cookie, sp and fp.
 */
void pic18_idle(void)
{
	/* Clear hardware stack. */
	STKPTR = 0x00;

	/* Setup the idle context. */
	tt_current->sp	= pic18_stack;
	tt_current->cookie =
		(unsigned short *)(tt_current->sp + ENV_STACKSIZE_IDLE - 2);
	*tt_current->cookie =  PIC18_CONTEXT_COOKIE;

	_asm
		/* Setup the stack pointer. */
		movlw	PIC18_STACK_OFFSET&0xff
		movwf	FSR1L, 0
		movlw	(PIC18_STACK_OFFSET>>8)&0xff
		movwf	FSR1H, 0

		/* Setup the frame pointer. */
		movff	FSR1L, FSR2L
		movff	FSR1H, FSR2H

		/* 
		 * Save the framepointer onto the stack in case we have
		 * to return from somewhere. Shouldn't happen but we can spend
		 * 4 cycles on it when we init the device.
		 */
		movff	FSR2L, POSTINC1
		movff	FSR2H, POSTINC1
	_endasm

	/* Leave protected mode so that we might receive interrupts. */
	ENV_PROTECT(0);

	/* Do nothing, forever. */
	for (;;);
}

/* ************************************************************************** */

/**
 * \brief The PIC18 timer get function.
 *
 * Will take into account the overflow crap.
 */
env_time_t pic18_timer_get(void)
{
	return pic18_timer_base + TMR3;
}

/* ************************************************************************** */

/**
 * \brief The PIC18 timer epoch schedule routine.
 */
void pic18_timer_epoch_schedule(void)
{
	signed long diff;
	
	/* If we're not active then don't bother. */
	if (!pic18_timer_active)
		return;

	/* Reset CCP2 module. */
	CCP2OFF();

	/* 
	 * Figure out when this is going to happen with respect to the
	 * time that is NOW.
	 */
	diff = (signed long)pic18_timer_next - (signed long)pic18_timer_base;
	if (diff < PIC18_TIMER_COUNT)
	{
		/* 
		 * The baseline will expire this epoch, might already have
		 * expired though. Always check for race condition.
		 */
		if (TMR3 >= diff)
			CCPR2 = TMR3 + 1;
		else
			CCPR2 = diff;

		/* Enable the CCP2 interrupt. */
		CCP2ON();

		/* Make sure we didn't race the timer. */
		while (CCPR2 <= TMR3 && !(PIR2bits.CCP2IF))
		{
			/*
			 * Do NOT use CCPR2++ since it might *#&"#-up when we wrap around.
			 * This might still *#&"#-up but the counter does not run amok.
			 */
			CCPR2 = TMR3 + 1;
		}
	}

}

/* ************************************************************************** */

/**
 * \brief The PIC18 timer set function.
 */
void pic18_timer_set(env_time_t next)
{
	/* Set new next and schedule the timer interrupt. */
	pic18_timer_next = next;
	pic18_timer_active = 1;
	pic18_timer_epoch_schedule();
}

/* ************************************************************************** */

/**
 * \brief The PIC18 print function.
 *
 * C18 has some serious issue when it handles strings, especially constant
 * strings. They tend to end up in the rom and thus makes it impossible to
 * treat constants strings and other strings in the same way.
 */
void pic18_print(const rom char *msg)
{
	while (*msg)
	{
		while (BusyUSART());
		putcUSART(*msg++);
	}
	while (BusyUSART());
}

/* ************************************************************************** */

#if 0
/**
 * \brief The PIC18 print variable function.
 *
 * Used for debugging purposes only.
 */
void pic18_print_var(unsigned long var)
{
	static unsigned char digits[] =
	{
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'a', 'b',
		'c', 'd', 'e', 'f'
	};
	unsigned char i, *tmp = (unsigned char*)&var;

	tmp += 3;
	for (i=0;i<4;i++, tmp--)
	{
		while (BusyUSART());
		putcUSART(digits[*tmp >> 4]);
		while (BusyUSART());
		putcUSART(digits[*tmp & 0xf]);
	}
	while (BusyUSART());
}
#endif

/* ************************************************************************** */

/**
 * \brief The PIC18 low priority interrupt handler.
 *
 * The tinyTimber kernel has 2 interrupts, Timer3 overflow and
 * CCP2 compare interrupt.
 */
/*#pragma interruptlow pic18_interrupt_low*/
void pic18_interrupt_low(void)
{
	/* 
	 * NOTE: must NOT!!! have _ANY_ variables on the stack or we are
	 * fried.
	 */
	static unsigned char tmp = 0;

	/* Save context. */
	PIC18_CONTEXT_SAVE(pic18_interrupt_return);

	if (PIR2bits.TMR3IF || PIR2bits.CCP2IF)
	{
		if (PIR2bits.CCP2IF)
		{
			/*
			 * Ok, some baseline expired, let's tell tinyTimber that. But
			 * first disable the ccp2 interrupt.
			 */
			CCP2OFF();
			if (pic18_timer_active)
			{
				pic18_timer_active = 0;
				tt_expired(pic18_timer_base + CCPR2);
				tt_schedule();
			}
		}

		if (PIR2bits.TMR3IF)
		{
			/* 
			 * Overflow, update the time and rescedule anyinterrupt that
			 * should occur during this epoch.
			 */
			PIR2bits.TMR3IF = 0;
			pic18_timer_base += PIC18_TIMER_COUNT;
			pic18_timer_epoch_schedule();
		}
	}
	else
	{
		/* 
		 * Unknown or user interrupt, pass to the user interrupt handler
		 * if there is one, otherwise we panic().
		 */
		if (pic18_interrupt_handler_low)
		{
			/* 
			 * Update the timestamp of course, take any overflow into account
			 * as well. Guard againt the race condition as well, *sigh* this is
			 * costly :/
			 */
			tmp = PIR2bits.TMR3IF;
			if (tmp)
				pic18_timestamp = pic18_timer_base + PIC18_TIMER_COUNT + TMR3;
			else
			{
				/*
				 * No overflow but check after assignment so that it has not
				 * occured.
				 */
				pic18_timestamp = pic18_timer_base + TMR3;
				if (!tmp && PIR2bits.TMR3IF)
				{
					pic18_timestamp =
						pic18_timer_base + PIC18_TIMER_COUNT + TMR3;
				}
			}
			pic18_interrupt_handler_low();
		}
		else
			ENV_PANIC("Low priority interrupt but no handler installed.\n");
	}

	/* Restore new or old context. */
	PIC18_CONTEXT_RESTORE();
}

/* ************************************************************************** */

/**
 * \brief The PIC18 high priority interrupt handler.
 *
 * The tinyTimber kernel no high priority interrupt and high
 * priority interrupt must _never_ interract with the tinyTimber
 * kernel in _any_ way.
 */
#pragma interrupt pic18_interrupt_high
void pic18_interrupt_high(void)
{
	/* Make sure there is a high priority interrupt installed. */
	if (pic18_interrupt_handler_high)
		pic18_interrupt_handler_high();
	else
		ENV_PANIC("High priority interrut but no interrupt handler.\n");
}

/* ************************************************************************** */

/**
 * \brief The PIC18 low priority interrupt vector.
 */
#pragma code low_vector=0x18
void pic18_interrupt_low_vector(void)
{
	_asm
		goto pic18_interrupt_low
	_endasm
}
#pragma code

/* ************************************************************************** */

/**
 * \brief The PIC18 high priority interrupt vector.
 */
#pragma code high_vector=0x08
void pic18_interrupt_high_vector(void)
{
	_asm
		goto pic18_interrupt_high
	_endasm
}
#pragma code
