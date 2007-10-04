#ifndef ENV_TYPES_H_
#define ENV_TYPES_H_

#if ! defined TT_MANGLED
#	if defined ENV_POSIX
#		include "posix/types.h"
#	elif defined ENV_MSP430
#		include "msp430/types.h"
#	elif defined ENV_AVR
#		include "avr5/types.h"
#	elif defined ENV_PIC18
#		include "pic18/types.h"
#	elif defined ENV_ARM7
#		include "arm7/types.h"
#	elif defined ENV_MIPS
#		include "mips/types.h"
#	elif defined ENV_SKEL
#		include "skel/types.h"
#	else
#		error Unknown environment.
#	endif
#endif

#endif
