/**
 * \brief Skeleton environment types.
 *
 * Just used for compile testing and perhaps as a template for a new
 * environment. Should compile but does not run.
 */
#ifndef ENV_ARM7_TYPES_H_
#define ENV_ARM7_TYPES_H_

/**
 * \brief ARM7 context typedef.
 */
typedef struct arm7_context_t
{
	/**
	 * \brief The stack pointer.
	 */
	unsigned int *sp;

	/**
	 * \brief The context cookie pointer.
	 */
	unsigned int *cookie;
} arm7_context_t;

/**
 * \brief Environment context typedef.
 *
 * Should evalutate to the type that is used to store the context of a
 * thread.
 */
typedef arm7_context_t env_context_t;

/**
 * \brief Environment time typedef.
 *
 * Should evaluate to the type that is used to represent time within the
 * environment.
 */
typedef int env_time_t;

/**
 * \brief Environment result typdef.
 *
 * Should evaluate to the result of a synchronus function call.
 */
typedef int env_result_t;

#endif
