#include "config.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "list.h"
#include "logger.h"
#include "utils.h"

typedef struct Element {
  void *value;
  void (*destroy)(void *);
} Element;

struct List {
  size_t length;
  size_t capacity;
  Element **buffer;
};

static void EnsureCapacity(List *const list, const size_t n_elements) {
  assert(list != NULL);

  size_t new_capacity = list->capacity;
  while (new_capacity < list->length + n_elements) {
    new_capacity *= 2;
  }

  if (new_capacity == list->capacity) {
    return;
  }

  Element **new_buffer =
      realloc(list->buffer, sizeof(Element *) * new_capacity);
  if (new_buffer == NULL) {
    LOG_CRITICAL("realloc(3): Failed to allocate memory: %s", strerror(errno));
  }

  list->capacity = new_capacity;
  list->buffer = new_buffer;
}

List *ListCreate(void) {
  List *list = xmalloc(sizeof(List));
  list->length = 0;
  list->capacity = DEFAULT_LIST_CAPACITY;
  list->buffer = xcalloc(list->capacity, sizeof(Element *));
  return list;
}

void ListDestroy(void *const ptr) {
  List *const list = (List *)ptr;
  if (list == NULL) {
    return;
  }

  assert(list->buffer != NULL);
  for (size_t i = 0; i < list->length; i++) {
    Element *element = list->buffer[i];
    assert(element != NULL);
    if (element->destroy != NULL) {
      element->destroy(element->value);
    }
    free(element);
  }

  free(list->buffer);
  free(list);
}

size_t ListLength(const List *const list) {
  assert(list != NULL);
  return list->length;
}

void ListAppend(List *const list, void *const value, void (*destroy)(void *)) {
  assert(list != NULL);
  assert(list->buffer != NULL);

  EnsureCapacity(list, 1);

  // Create element
  Element *element = xmalloc(sizeof(Element));
  element->value = value;
  element->destroy = destroy;

  // Insert element into buffer
  list->buffer[list->length++] = element;
}

const void *ListGet(const List *const list, const size_t index) {
  assert(list != NULL);
  assert(list->buffer != NULL);
  assert(index < list->length);

  Element *element = list->buffer[index];
  assert(element != NULL);
  return element->value;
}

void ListSet(List *const list, const size_t index, void *const value,
             void (*destroy)(void *)) {
  assert(list != NULL);
  assert(list->buffer != NULL);
  assert(index < list->length);
  assert(list->buffer[index] != NULL);

  if (list->buffer[index]->destroy != NULL) {
    list->buffer[index]->destroy(list->buffer[index]->value);
  }
  list->buffer[index]->value = value;
  list->buffer[index]->destroy = destroy;
}

void *ListRemove(List *const list, const size_t index) {
  assert(list != NULL);
  assert(list->buffer != NULL);
  assert(list->length > index);
  assert(list->buffer[index] != NULL);

  // Remove element
  void *const value = list->buffer[index]->value;
  free(list->buffer[index]);

  // Shift elements to the left
  list->length -= 1;
  memmove(list->buffer + index, list->buffer + (index + 1),
          (list->length - index) * sizeof(Element *));

  return value;
}

void ListInsert(List *const list, const size_t index, void *const value,
                void (*destroy)(void *)) {
  assert(list->buffer != NULL);
  assert(list->length >= index);

  EnsureCapacity(list, 1);

  Element *const element = xmalloc(sizeof(Element));
  element->value = value;
  element->destroy = destroy;

  memmove(list->buffer + index + 1, list->buffer + index,
          (list->length - index) * sizeof(Element *));
  list->buffer[index] = element;
  list->length += 1;
}
