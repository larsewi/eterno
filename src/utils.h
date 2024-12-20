#ifndef __ETERNO_UTILS_H__
#define __ETERNO_UTILS_H__

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"

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

#define StringEqual(a, b) (strcmp(a, b) == 0)

/**
 * @brief Allocate memory using malloc(3). On error, print error message and
 *        abort(3).
 */
static inline void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    LOG_CRITICAL("Failed to allocate memory");
  }
  assert(ptr != NULL); /* Program execution should have been aborted */
  return ptr;
}

/**
 * @brief Allocate memory using calloc(3). On error, print error message and
 *        abort(3).
 */
static inline void *xcalloc(size_t nmemb, size_t size) {
  void *ptr = calloc(nmemb, size);
  if (ptr == NULL) {
    LOG_CRITICAL("Failed to allocate memory");
  }
  assert(ptr != NULL); /* Program execution should have been aborted */
  return ptr;
}

/**
 * @brief Duplicate string using strdup(3). On error, print error message and
 *        abort(3).
 */
static inline char *xstrdup(const char *str) {
  assert(str != NULL);
  char *dup = strdup(str);
  if (dup == NULL) {
    LOG_CRITICAL("Failed to allocate memory: %s", strerror(errno));
  }
  assert(dup != NULL); /* Program execution should have been aborted */
  return dup;
}

#endif /* __ETERNO_UTILS_H__ */
