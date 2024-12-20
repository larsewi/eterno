#include "config.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "dict.h"
#include "list.h"
#include "logger.h"
#include "utils.h"

typedef struct Entry {
  char *key;
  void *value;
  void (*destroy)(void *);
  bool invalidated;
} Entry;

struct Dict {
  size_t length;
  size_t capacity;
  size_t in_use;
  Entry **buffer;
};

/**
 * @brief Hash a key.
 * @param key The key.
 * @return The hash.
 */
static size_t HashKey(const char *const key) {
  assert(key != NULL);

  size_t hash = 5381;
  const size_t length = strlen(key);
  for (size_t i = 0; i < length; i++) {
    hash = ((hash << 5) + hash) + (size_t)key[i];
  }
  return hash;
}

/**
 * @brief Compute the available index of a new entry in a dictionary.
 * @param dict The dictionary.
 * @param key The key.
 * @return The index.
 */
static size_t ComputeIndex(const Dict *const dict, const char *const key) {
  assert(dict != NULL);
  assert(dict->buffer != NULL);
  assert(key != NULL);

  size_t index = HashKey(key) % dict->capacity;
  while (true) {
    Entry *entry = dict->buffer[index];
    if (entry == NULL) {
      break;
    }
    if (!entry->invalidated && StringEqual(entry->key, key)) {
      break;
    }
    index = (index + 1) % dict->capacity;
  }

  return index;
}

static void EnsureCapacity(Dict *const dict) {
  assert(dict != NULL);
  assert(DEFAULT_DICT_MAX_LOAD_FACTOR > DEFAULT_DICT_MIN_LOAD_FACTOR);

  if ((float)dict->in_use <
      ((float)dict->capacity * DEFAULT_DICT_MAX_LOAD_FACTOR)) {
    return;
  }

  /* If we can free enough of the capacity by removing invalidated items, there
   * is no need to expand the buffer. */
  assert(dict->in_use >= dict->length);
  const bool expand =
      (((float)dict->capacity / 100.f) * (float)(dict->in_use - dict->length)) <
      DEFAULT_DICT_MIN_LOAD_FACTOR;

  const size_t new_capacity = (expand) ? dict->capacity * 2 : dict->capacity;
  Entry **const new_buffer = (Entry **)calloc(new_capacity, sizeof(Entry *));
  if (new_buffer == NULL) {
    LOG_CRITICAL("calloc(3): Failed to allocate memory: %s", strerror(errno));
  }

  Entry **const old_buffer = dict->buffer;
  dict->buffer = new_buffer;
  const size_t old_capacity = dict->capacity;
  dict->capacity = new_capacity;

  for (size_t i = 0; expand && (i < old_capacity); i++) {
    Entry *const entry = old_buffer[i];
    if (entry == NULL) {
      continue;
    }

    if (entry->invalidated) {
      free(entry);
      continue;
    }

    const size_t index = ComputeIndex(dict, entry->key);
    assert(new_buffer[index] == NULL);
    new_buffer[index] = entry;
  }

  dict->in_use = dict->length;
  free(old_buffer);
}

Dict *DictCreate(void) {
  Dict *dict = xmalloc(sizeof(Dict));
  dict->length = dict->in_use = 0;
  dict->capacity = DEFAULT_DICT_CAPACITY;
  dict->buffer = (Entry **)calloc(dict->capacity, sizeof(Entry *));

  if (dict->buffer == NULL) {
    LOG_CRITICAL("calloc(3): Failed to allocate memory: %s", strerror(errno));
  }

  return dict;
}

void DictDestroy(void *const ptr) {
  Dict *const dict = (Dict *)ptr;
  if (dict == NULL) {
    return;
  }

  assert(dict->buffer != NULL);

  for (size_t i = 0; i < dict->capacity; i++) {
    Entry *entry = dict->buffer[i];
    if (entry == NULL) {
      continue;
    }

    if (!entry->invalidated) {
      free(entry->key);
      if (entry->destroy != NULL) {
        entry->destroy(entry->value);
      }
    }

    free(entry);
  }

  free(dict->buffer);
  free(dict);
}

size_t DictLength(const Dict *const dict) {
  assert(dict != NULL);
  return dict->length;
}

void DictSet(Dict *const dict, const char *const key, void *const value,
             void (*destroy)(void *)) {
  assert(dict != NULL);
  assert(dict->buffer != NULL);
  assert(key != NULL);

  EnsureCapacity(dict);

  const size_t index = ComputeIndex(dict, key);
  if (dict->buffer[index] != NULL) {
    Entry *const item = dict->buffer[index];
    assert(item->key != NULL);
    assert(StringEqual(key, item->key));

    if (item->destroy != NULL) {
      item->destroy(item->value);
    }
    item->value = value;
    item->destroy = destroy;
  }

  Entry *entry = xmalloc(sizeof(Entry));
  entry->key = xstrdup(key);
  entry->value = value;
  entry->destroy = destroy;
  entry->invalidated = false;

  dict->buffer[index] = entry;
  dict->in_use += 1;
  dict->length += 1;
}

bool DictHasKey(const Dict *const dict, const char *const key) {
  assert(dict != NULL);
  assert(dict->buffer != NULL);
  assert(key != NULL);

  const size_t index = ComputeIndex(dict, key);
  return dict->buffer[index] != NULL;
}

List *DictGetKeys(const Dict *const dict) {
  assert(dict != NULL);
  assert(dict->buffer != NULL);

  List *const keys = ListCreate();
  for (size_t i = 0; i < dict->capacity; i++) {
    Entry *const entry = dict->buffer[i];
    if (entry == NULL || entry->invalidated) {
      continue;
    }

    assert(entry->key != NULL);
    char *const key = xstrdup(entry->key);
    ListAppend(keys, key, free);
  }

  return keys;
}

const void *DictGet(const Dict *const dict, const char *const key) {
  assert(dict != NULL);
  assert(dict->buffer != NULL);
  assert(key != NULL);

  const size_t index = ComputeIndex(dict, key);
  Entry *entry = dict->buffer[index];
  assert(entry != NULL);
  return entry->value;
}

void *DictRemove(Dict *const dict, const char *const key) {
  assert(dict != NULL);
  assert(dict->buffer != NULL);
  assert(key != NULL);

  const size_t index = ComputeIndex(dict, key);
  Entry *const entry = dict->buffer[index];
  assert(entry != NULL);
  assert(entry->key != NULL);
  assert(StringEqual(entry->key, key));
  assert(!entry->invalidated);

  free(entry->key);
  entry->key = NULL;

  void *value = entry->value;
  entry->invalidated = true;

  assert(dict->length > 0);
  dict->length -= 1;

  return value;
}
