
#include "config.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "buffer.h"
#include "logger.h"
#include "utils.h"

struct Buffer {
  size_t length;
  size_t capacity;
  char *buffer;
};

static void EnsureCapacity(Buffer *const buf, const size_t needed) {
  assert(buf != NULL);

  while ((buf->capacity - buf->length) <= needed) {
    size_t new_capacity =
        (buf->capacity > 0) ? buf->capacity * 2 : DEFAULT_BUFFER_CAPACITY;
    char *new_buffer = realloc(buf->buffer, new_capacity);
    if (new_buffer == NULL) {
      LOG_CRITICAL("realloc(3): Failed to allocate memory: %s",
                   strerror(errno));
    }
    buf->capacity = new_capacity;
    buf->buffer = new_buffer;
  }
}

Buffer *BufferCreate(void) {
  Buffer *buf = xmalloc(sizeof(Buffer));

  buf->capacity = DEFAULT_BUFFER_CAPACITY;
  buf->length = 0;
  buf->buffer = xmalloc(buf->capacity);
  buf->buffer[0] = '\0';

  return buf;
}

const char *BufferData(const Buffer *const buf) {
  assert(buf != NULL);
  return buf->buffer;
}

size_t BufferLength(const Buffer *const buf) {
  assert(buf != NULL);
  return buf->length;
}

void BufferAppend(Buffer *const buf, const char ch) {
  assert(buf != NULL);
  EnsureCapacity(buf, 1);
  buf->buffer[buf->length++] = ch;
  buf->buffer[buf->length] = '\0';
  assert(buf->length <= buf->capacity);
}

void BufferPrint(Buffer *const buf, const char *const str) {
  assert(buf != NULL);
  assert(str != NULL);

  const size_t length = strlen(str);
  EnsureCapacity(buf, length);
  memcpy(buf->buffer + buf->length, str, length);
  buf->length += length;
  buf->buffer[buf->length] = '\0';
  assert(buf->length <= buf->capacity);
}

void BufferPrintFormat(Buffer *const buf, const char *const fmt, ...) {
  assert(buf != NULL);
  assert(fmt != NULL);

  va_list ap;

  // Figure out how many bytes we need
  va_start(ap, fmt);
  const int length = vsnprintf(NULL, 0, fmt, ap);
  va_end(ap);

  if (length < 0) {
    LOG_CRITICAL("vsnprintf(3): Unexpected return value (%d < 0): %s", length,
                 strerror(errno));
  }
  EnsureCapacity(buf, (size_t)length);

  va_start(ap, fmt);
  const int ret = vsnprintf(buf->buffer + buf->length,
                            buf->capacity - buf->length, fmt, ap);
  va_end(ap);

  if (ret != length) {
    LOG_CRITICAL("vsnprintf(3): Unexpected return value (%d != %d): %s", ret,
                 length, strerror(errno));
  }

  buf->length += (size_t)length;
  assert(buf->length <= buf->capacity);
}

char *BufferToString(Buffer *const buf) {
  assert(buf != NULL);
  char *const str = buf->buffer;
  free(buf);
  return str;
}

Buffer *BufferFromString(const char *const str) {
  Buffer *const buf = BufferCreate();
  BufferPrint(buf, str);
  return buf;
}

bool BufferReadFile(Buffer *const buf, const char *const filename) {
  assert(buf != NULL);
  assert(filename != NULL);

  const int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    LOG_ERROR("Failed to open file '%s' for reading: %s", filename,
              strerror(errno));
    return false;
  }

  ssize_t n_read = 0;
  do {
    EnsureCapacity(buf, DEFAULT_BUFFER_CAPACITY);
    n_read = read(fd, buf->buffer + buf->length, DEFAULT_BUFFER_CAPACITY);
    if (n_read < 0) {
      LOG_ERROR("Failed to read file '%s': %s", filename, strerror(errno));
      close(fd);
      return false;
    }

    buf->length += (size_t)n_read;
    assert(buf->length <= buf->capacity);
  } while (n_read > 0);

  close(fd);
  buf->buffer[buf->length] = '\0';
  LOG_DEBUG("Read %zu byte(s) from file '%s'", buf->length, filename);

  return true;
}

void BufferDestroy(void *const ptr) {
  Buffer *const buf = (Buffer *)ptr;
  if (buf != NULL) {
    free(buf->buffer);
    free(buf);
  }
}
