#ifndef ENV_ARM7_ARM7_SYS_CALL_H_
#define ENV_ARM7_ARM7_SYS_CALL_H_

/* ************************************************************************** */

#define ARM7_SYS_CALL_BIND(function, id) \
__attribute__((naked)) function\
{\
	asm volatile(\
		"swi %0\n"\
		"mov pc, lr\n"\
		:: "I" (id)\
	);\
}

/* ************************************************************************** */

/**
 * \brief ARM7 system call enum
 */
enum
{
	/**
	 * \brief Dispatch system call id.
	 */
	ARM7_SYS_CALL_DISPATCH = 0,

	/**
	 * \brief The last system call id + 1, needed for sanity.
	 */
	ARM7_SYS_CALL_MAX
};

#endif
