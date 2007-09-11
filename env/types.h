#ifndef ENV_TYPES_H_
#define ENV_TYPES_H_

#if defined ENV_POSIX
#	include "env/posix/types.h"
#elif defined ENV_MSP430
#	include "env/msp430/types.h"
#elif defined ENV_AVR
#	include "env/avr/types.h"
#elif defined ENV_PIC18
#	include "env/pic18/types.h"
#elif defined ENV_ARM7
#	include "env/arm7/types.h"
#elif defined ENV_SKEL
#	include "env/skel/types.h"
#else
#	error Unknown environment.
#endif

#endif
