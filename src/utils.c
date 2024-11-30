#include "utils.h"

#include <stdio.h>

#include "logger.h"

void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    LOG_CRITICAL("Failed to allocate memory");
    abort();
  }
  return ptr;
}
