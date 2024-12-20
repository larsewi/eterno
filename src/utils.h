#ifndef __ETERNO_UTILS_H__
#define __ETERNO_UTILS_H__

#include <limits.h>
#include <stdlib.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif /* PATH_MAX */

#ifndef MIN
#define MIN(a, b) (a <= b ? a : b)
#endif /* MIN */

#ifndef MAX
#define MAX(a, b) (a >= b ? a : b)
#endif /* MAX */

#define ARG_UNUSED __attribute__((unused))

#ifdef NDEBUG
#define NDEBUG_UNUSED __attribute__((unused))
#else
#define NDEBUG_UNUSED
#endif

/**
 * @brief Allocate memory using malloc(3). On error, print error message and
 *        abort(3).
 */
void *xmalloc(size_t size);

/**
 * @brief Duplicate string using strdup(3). On error, print error message and
 *        abort(3).
 */
char *xstrdup(const char *str);

#endif /* __ETERNO_UTILS_H__ */
