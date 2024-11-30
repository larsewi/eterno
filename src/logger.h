#ifndef __ETERNO_LOGGER_H__
#define __ETERNO_LOGGER_H__

#include <stdbool.h>

enum LogLevel {
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_CRITICAL,
};

#define LOG_DEBUG(...)                                                         \
  LogMessage(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

#define LOG_WARNING(...)                                                       \
  LogMessage(LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)

#define LOG_ERROR(...)                                                         \
  LogMessage(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#define LOG_CRITICAL(...)                                                      \
  LogMessage(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, __VA_ARGS__)

void SetDebugLogging(bool enable);

void LogMessage(enum LogLevel level, const char *file, const int line,
                const char *format, ...);

#endif // __ETERNO_LOGGER_H__