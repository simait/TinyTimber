/**
 * \brief Skeleton environment types.
 *
 * Just used for compile testing and perhaps as a template for a new
 * environment. Should compile but does not run.
 */
#ifndef ENV_SKEL_TYPES_H_
#define ENV_SKEL_TYPES_H_

/**
 * \brief Environment context typedef.
 *
 * Should evalutate to the type that is used to store the context of a
 * thread.
 */
typedef int env_context_t;

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
