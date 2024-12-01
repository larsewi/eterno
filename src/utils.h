#ifndef __ETERNO_UTILS_H__
#define __ETERNO_UTILS_H__

#include <limits.h>
#include <stdlib.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif /* PATH_MAX */

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
