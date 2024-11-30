#include "logger.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static bool LOGGER_LOG_DEBUG = false;

void SetDebugLogging(const bool enable) { LOGGER_LOG_DEBUG = enable; }

void LogMessage(enum LogLevel level, const char *file, const int line,
                const char *format, ...) {
  assert(file != NULL);
  assert(format != NULL);

  if (level == LOG_LEVEL_DEBUG && !LOGGER_LOG_DEBUG) {
    return;
  }

  va_list ap;
  va_start(ap, format);

  char message[1024];
  int size = vsnprintf(message, sizeof(message), format, ap);
  if (size < 0 || (size_t)size >= sizeof(message)) {
    LOG_WARNING("Truncation error: Log message too long (%d >= %d)", size,
                sizeof(message));
    message[sizeof(message) - 2] = '.';
    message[sizeof(message) - 3] = '.';
    message[sizeof(message) - 4] = '.';
  }
  va_end(ap);

  switch (level) {
  case LOG_LEVEL_DEBUG:
    fprintf(stdout, "[DBG][%s:%d]: %s\n", file, line, message);
    break;
  case LOG_LEVEL_WARNING:
    fprintf(stdout, "[WRN]: %s\n", message);
    break;
  case LOG_LEVEL_ERROR:
    fprintf(stderr, "[ERR]: %s\n", message);
    break;
  case LOG_LEVEL_CRITICAL:
    fprintf(stderr, "[CRT][%s:%d]: %s: ", file, line, message);
    abort(); /* It's not safe to proceed */
  }
}
