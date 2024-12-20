
#ifndef __ETERNO_LIST_H__
#define __ETERNO_LIST_H__

#include <stdlib.h>

typedef struct List List;

/**
 * @brief Create a list.
 * @return The list.
 * @note Caller takes ownership of returned value.
 */
List *ListCreate(void);

/**
 * @brief Destroy the list.
 * @param ptr Pointer to the list.
 * @note If ptr is NULL, no operation is performed. Otherwise, elements are
 *       destroyed using passed destroy function unless it's NULL.
 */
void ListDestroy(void *list);

/**
 * @brief Get length of list.
 * @param list The list.
 * @return Length of the list.
 */
size_t ListLength(const List *list);

/**
 * @brief Append an element to list.
 * @param list The list.
 * @param value The value of the element.
 * @param destroy Function to destroy the value of the element or NULL.
 */
void ListAppend(List *list, void *value, void (*destroy)(void *));

/**
 * @brief Get the value of an element in the list.
 * @param list The list.
 * @param index Index of the element.
 * @return Value of the element.
 */
const void *ListGet(const List *list, size_t index);

/**
 * @brief Set the value of an element.
 * @param list The list.
 * @param index The index of the element.
 * @param value The value.
 * @param destroy Function to destroy the value of the element or NULL.
 */
void ListSet(List *list, size_t index, void *value, void (*destroy)(void *));

/**
 * @brief Remove an element from the list.
 * @param list The list.
 * @param index The index of the element.
 * @return The value of the element.
 * @note The caller takes ownership of returned value.
 */
void *ListRemove(List *list, size_t index);

/**
 * @brief Insert an element to the list.
 * @param list The list.
 * @param index The index of the element.
 * @param value The value.
 * @param destroy Function to destroy the value of the element or NULL.
 * @note All elements from the index to the end of the list are shifted once to
 *       the right in order to make space for the new element. This can be an
 *       expensive operation for large lists.
 */
void ListInsert(List *list, size_t index, void *value, void (*destroy)(void *));

#endif // __ETERNO_LIST_H__
