#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "logger.h"

void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    LOG_CRITICAL("Failed to allocate memory");
  }
  assert(ptr != NULL); /* Program execution should have been aborted */
  return ptr;
}

char *xstrdup(const char *str) {
  assert(str != NULL);
  char *dup = strdup(str);
  if (dup == NULL) {
    LOG_CRITICAL("Failed to allocate memory: %s", strerror(errno));
  }
  assert(dup != NULL); /* Program execution should have been aborted */
  return dup;
}
