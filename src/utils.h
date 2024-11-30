#ifndef __ETERNO_UTILS_H__
#define __ETERNO_UTILS_H__

#include <stdlib.h>

/**
 * @brief Allocate memory using malloc(3). On error, print error message and
 *        abort(3).
 */
void *xmalloc(size_t size);

#endif /* __ETERNO_UTILS_H__ */
