#ifndef ENV_AVR_TYPES_H_
#define ENV_AVR_TYPES_H_

/**
 * \brief The AVR context typedef.
 */
typedef struct avr5_context_t
{
	/**
	 * \brief The stackpointer.
	 */
	char *sp;

	/**
	 * \brief The magic cookie pointer.
	 */
	unsigned short *cookie;
} avr5_context_t;

/**
 * \brief Export of avr context as env_context_t.
 */
typedef avr5_context_t env_context_t;

/**
 * \brief The AVR time typedef.
 */
typedef unsigned long env_time_t;

/**
 * \brief The AVR result typedef.
 */
typedef unsigned long env_result_t;

#endif
