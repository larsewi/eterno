#ifndef __ETERNO_BUFFER_H__
#define __ETERNO_BUFFER_H__

#include <stdbool.h>
#include <stdlib.h>

typedef struct Buffer Buffer;

/**
 * @brief Create a buffer.
 * @return Buffer.
 * @note Caller takes ownership of returned value.
 */
Buffer *BufferCreate(void);

/**
 * @brief Get buffer data.
 * @param buf Buffer.
 * @return Pointer to internal buffer data.
 */
const char *BufferData(const Buffer *buf);

/**
 * @brief Get length of buffer.
 * @param buf Buffer.
 * @return Length of buffer excluding terminating null-byte.
 */
size_t BufferLength(const Buffer *buf);

/**
 * @brief Append a byte to the buffer.
 * @param buf Buffer.
 * @param ch Byte.
 */
void BufferAppend(Buffer *buf, char ch);

/**
 * @brief Print string to buffer.
 * @param buf Buffer.
 * @param str String.
 */
void BufferPrint(Buffer *buf, const char *str);

/**
 * @brief Print string to buffer.
 * @param buf Buffer.
 * @param fmt Format string.
 * @param ... Format arguments.
 */
void BufferPrintFormat(Buffer *buf, const char *fmt, ...);

/**
 * @brief Convert buffer to string.
 * @param buf Buffer.
 * @return Pointer to internal buffer data.
 * @note Buffer is destroyed. Caller takes ownership of returned value.
 */
char *BufferToString(Buffer *buf);

/**
 * @brief Create buffer from string.
 * @param str String.
 * @return Buffer.
 * @note Caller takes ownership of returned value.
 */
Buffer *BufferFromString(const char *str);

/**
 * @brief Read file contents into buffer.
 * @param buf Buffer.
 * @param filename Path to file.
 * @return True on success, otherwise false.
 * @note Error is logged to stderr.
 */
bool BufferReadFile(Buffer *buf, const char *filename);

/**
 * @brief Destroy buffer.
 * @param ptr Pointer to Buffer.
 * @note If ptr is NULL, no operation is performed.
 */
void BufferDestroy(void *ptr);

#endif // __ETERNO_BUFFER_H__
