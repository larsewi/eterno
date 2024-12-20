
#ifndef __ETERNO_DICT_H__
#define __ETERNO_DICT_H__

#include <stdbool.h>
#include <stdlib.h>

#include "list.h"

typedef struct Dict Dict;

/**
 * @brief Create a dictionary.
 * @return The dictionary.
 * @note Caller takes ownership of returned value.
 */
Dict *DictCreate(void);

/**
 * @brief Destroy the dictionary.
 * @param dict Pointer to dictionary.
 * @note If ptr is NULL, no operation is performed. Otherwise, values are
 *       destroyed using passed destroy function unless it's NULL.
 */
void DictDestroy(void *dict);

/**
 * @brief Get number of entries in dictionary.
 * @param dict The dictionary.
 * @return Number of entries in dictionary.
 */
size_t DictLength(const Dict *dict);

/**
 * @brief Create/update entry in dictionary.
 * @param dict The dictionary.
 * @param key Key of entry.
 * @param value Value of entry.
 * @param destroy Function to destroy the value of the entry or NULL.
 */
void DictSet(Dict *dict, const char *key, void *value, void (*destroy)(void *));

/**
 * @brief Check for existance of entry in dictionary.
 * @param dict The dictionary.
 * @param key Key of entry.
 * @return True if entry with key exists.
 */
bool LCH_DictHasKey(const Dict *dict, const char *key);

/**
 * @brief Get list of keys in dictionary.
 * @param dict The dictionary.
 * @return List of existing keys.
 * @note Caller takes ownership of returned value.
 */
List *DictGetKeys(const Dict *dict);

/**
 * @brief Get value of entry with key in dictionary.
 * @param dict The dictionary.
 * @param key Key of entry.
 * @return Value of entry.
 */
const void *DictGet(const Dict *dict, const char *key);

/**
 * @brief Remove entry from dictionary.
 * @param dict The dictionary.
 * @param key Key of entry.
 * @return Value of entry.
 * @note Caller takes ownership of returned value.
 */
void *DictRemove(Dict *dict, const char *key);

#endif // __ETERNO_DICT_H__
