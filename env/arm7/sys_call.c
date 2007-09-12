#include <arm7/env.h>
#include <arm7/sys_call.h>

/* ************************************************************************** */

typedef void (*arm7_sys_call_t)(void);

/* ************************************************************************** */

/**
 * \cond
 *
 * Just forward all system call implementations so that we can use them in
 * the table, no need for parameter correctness as we just want the address.
 */

/**
 * \endcond
 */

/**
 * \brief
 * 	System call table, used to determine which system call should
 * 	be issued.
 */
arm7_sys_call_t arm7_sys_call_table[] =
{
	(arm7_sys_call_t)_arm7_context_dispatch
};

/* ************************************************************************** */

/**
 * \cond
 *
 * System call binding, used to get the mapping of syscall id to function.
 */

/**
 * \endcond
 */

/* ************************************************************************** */

/**
 * \brief ARM7 system call entry point.
 *
 * A system call should be treated as a simple C function call. We just
 * need some magic to make the syscall id translate into a C function.
 *
 * \todo Perform a sanity check that the system call id is valid.
 */
ENV_EXT_FORCE_RAM __attribute__((naked)) void arm7_sys_call(void)
{
	/* 
	 * Adjudt the link register so that we can treat this as a regular
	 * interrupt ie. use save/restore context macros.
	 */
	asm volatile("add	lr, lr, #4\n");

	/* 
	 * Save context, we must do this since we don't know if the system
	 * call will possibly switch the context...
	 */
	ARM7_CONTEXT_SAVE();

	asm volatile
	(
		/*
		 * r0-r3 may or may not hold parameters, let's assume they do
		 * and use r4 and r5 as temporary. Please note that _all_ user
		 * mode registers are saved but registers r0-r3 are used for
		 * parameter passing thus we wish to keep them intact so that
		 * we do not have to take anything special into conisderation
		 * when dealing with the system calls.
		 */
		"ldr	r4, [lr, #-8]\n"
		"and	r4, r4, #0xffffff\n"
		"ldr	r5, =arm7_sys_call_table\n"
		"ldr	r4, [r5, r4, lsl #2]\n"
		"mov	lr, pc\n"
		"mov	pc, r4\n"
	);

	/* Restore the context, might be same context or new context. */
	ARM7_CONTEXT_RESTORE();
}
