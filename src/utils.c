#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "logger.h"

void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    LOG_CRITICAL("Failed to allocate memory");
  }
  return ptr;
}

char *xstrdup(const char *str) {
  char *dup = strdup(str);
  if (dup == NULL) {
    LOG_CRITICAL("Failed to allocate memory: %s", strerror(errno));
  }
  return dup;
}
