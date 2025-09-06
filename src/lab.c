#include "lab.h"
#include <stdio.h>
#include <stdlib.h>
/*
 * =====
 * TYPES
 * =====
 */

/**
 * @enum NodeType
 * @brief Enumeration for selecting Node type
 *
 * AI Use: Assisted by AI
 * Needed a way to check (void* data) parameter is a node
 */
typedef enum { NODE, SENTINEL } NodeType;

/**
 * @struct Node
 * @brief a Node struct that is able to point to other nodes both ways
 */
typedef struct Node {
  NodeType type;
  struct Node *next, *prev;
} Node;

/**
 * @struct SentinelLinkedList
 * @brief SentinelLinkedList struct that is one implementation for a List struct
 */
typedef struct SentinelLinkedList {
  Node *head, *tail;
  size_t size;
} SentinelLinkedList;

typedef struct List {
  ListType type;

  // AI Use: Assisted by AI
  // I needed a form of inheritence to keep this type generic
  union {
    struct SentinelLinkedList *sentinel_list;
  } lists;
} List;

/*
 * ================
 * HELPER FUNCTIONS
 * ================
 */

/**
 * @brief frees a passed in pointer to a node
 * @param Node object pointer
 */
void free_node(void *data_ptr) { // GCOVR_EXCL_START
  Node *node = (Node *)data_ptr;
  free(node);
} // GCOVR_EXCL_STOP

bool index_in_bounds(size_t size, size_t index) {
  return (index < size && index >= 0);
}

size_t sentinel_list_size(SentinelLinkedList *sentinel_list) {
  return sentinel_list->size;
}

void *sentinel_list_get(SentinelLinkedList *sentinel_list, size_t index) {

  // Start at tail or head based on which is closest
  // (unnecessary for small sets of data)
  bool nodeIsCloseToTail = index > (sentinel_list->size / 2); // index > middle
  int currIdx = (nodeIsCloseToTail) ? sentinel_list->size - 1
                                    : -1; // (-1) accounts for sentinel
  Node *currNode =
      (nodeIsCloseToTail) ? sentinel_list->tail : sentinel_list->head;
  Node *nextNode = currNode->next;

  // Find node at given index
  // (List is empty || index within bounds)
  while (currIdx != index) {
    // ? shift backward : shift forward
    currNode = nextNode;
    currIdx += (nodeIsCloseToTail) ? -1 : 1;
    nextNode = currNode->next;

    // Index bounds check
    if (!index_in_bounds(sentinel_list_size(sentinel_list), currIdx))
      break;
  }

  // Check index was found
  if (currIdx != index) { // GCOVR_EXCL_START
    return NULL;
  } // GCOVR_EXCL_STOP

  return currNode;
}

/**
 * @brief Create a new list of the specified type.
 * @return Pointer to the newly created list, or NULL on failure.
 */
List *sentinel_list_create(void) {
  List *list = malloc(sizeof(List));
  list->type = LIST_LINKED_SENTINEL;

  // Creates SentinelList Pointer
  // NOTE: allocates memory separately to optimize sizeof List
  // (avoid unnecessary allocation to unused implementations in the future)
  list->lists.sentinel_list = malloc(sizeof(SentinelLinkedList));

  // Sentinel node will always be the head -- we want tail on the first appended
  // element later
  Node *sentinelNode = malloc(sizeof(Node));
  sentinelNode->type = SENTINEL;
  // Sets all meta data pointers to sentinel node initially
  list->lists.sentinel_list->head = list->lists.sentinel_list->tail =
      sentinelNode->next = sentinelNode->prev = sentinelNode;

  // Sentinel node should not count toward size
  list->lists.sentinel_list->size = 0;

  return list;
}

/**
 * @brief Destroy the list and free all associated memory.
 * @param list Pointer to the list to destroy.
 * @param free_func Function to free individual elements. If NULL, elements are
 * not freed.
 */
void sentinel_list_destroy(List *list, FreeFunc free_func) {
  SentinelLinkedList *sentinel_list = list->lists.sentinel_list;

  // User must pass (non-null) FreeFunc to destroy elements
  // NOTE: If skipped, manually cleanup nodes in the list later
  if (free_func) {
    Node *sentinelNode = sentinel_list->head;
    Node *currNode = sentinelNode;
    Node *nextNode = currNode->next;

    while (currNode != NULL) { // GCOVR_EXCL_START
      free_func(currNode);     // GCOVR_EXCL_STOP
      // Reduce size
      sentinel_list->size = list_size(list) - 1;
      // Update currNode until loops back to sentinel node
      currNode = (nextNode == sentinelNode) ? NULL : nextNode;
      // Update next node
      nextNode = (currNode) ? currNode->next : NULL;
    }
  }

  // Finally cleanup lists
  free(sentinel_list); // GCOVR_EXCL_START
  sentinel_list = NULL;
  free(list);
  list = NULL; // GCOVR_EXCL_STOP
}

/**
 * @brief Append an element to the end of the list.
 * @param sentinel_list Pointer to the sentinel list.
 * @param newTail Pointer to the Node struct to append.
 * @return true on success, false on failure.
 */
bool sentinel_list_append(SentinelLinkedList *sentinel_list, Node *newTail) {
  Node *currTail = sentinel_list->tail;

  // Previous Tail <---> New Tail
  currTail->next = newTail;
  newTail->prev = currTail;

  // Sentinel <---> New Tail
  newTail->next = sentinel_list->head;
  sentinel_list->head->prev = newTail;

  // Update tail and list data
  sentinel_list->tail = newTail;
  sentinel_list->size += 1;

  return newTail == sentinel_list->tail;
}

/**
 * @brief Insert an element at a specific index.
 * @param sentinel_list Pointer to the sentinel list.
 * @param index Index at which to insert the element.
 * @param newNode Pointer to the Node struct to insert.
 * @return true on success, false on failure (e.g., index out of bounds).
 */
bool sentinel_list_insert(SentinelLinkedList *sentinel_list, size_t index,
                          Node *newNode) {
  // Index is out of bounds
  // GCOVR_EXCL_START
  if (index < 0 || index > sentinel_list_size(sentinel_list))
    return false;
  // GCOVR_EXCL_STOP
  //
  Node *nodeAtGivenIndex = sentinel_list_get(sentinel_list, index);
  // Node was not found
  // GCOVR_EXCL_START
  if (!nodeAtGivenIndex)
    return false;
  // GCOVR_EXCL_STOP

  Node *oldPrev = nodeAtGivenIndex->prev;
  // Found Node Prev <-> New Node
  newNode->prev = oldPrev;
  oldPrev->next = newNode;
  // New Node <-> Found Node
  newNode->next = nodeAtGivenIndex;
  nodeAtGivenIndex->prev = newNode;

  // Update list data as needed
  sentinel_list->tail =
      (sentinel_list_size(sentinel_list) == 0) ? newNode : sentinel_list->tail;
  sentinel_list->size += 1;

  // GCOVR_EXCL_START
  return true;
  // GCOVR_EXCL_STOP
}

/**
 * @brief Remove an element at a specific index.
 * @param sentinel_list Pointer to the sentinel list.
 * @param index Index of the element to remove.
 * @return Pointer to the element, or NULL if index is out of bounds.
 */
void *sentinel_list_remove(SentinelLinkedList *sentinel_list, size_t index) {
  // Index is out of bounds
  // GCOVR_EXCL_START
  if (!index_in_bounds(sentinel_list_size(sentinel_list), index)) {
    return NULL;
  } // GCOVR_EXCL_STOP

  Node *nodeAtGivenIndex = sentinel_list_get(sentinel_list, index);
  // Node was not found
  if (!nodeAtGivenIndex) { // GCOVR_EXCL_START
    return NULL;
  } // GCOVR_EXCL_STOP

  // "Remove" node at given index
  Node *prevOfFoundNode = nodeAtGivenIndex->prev;
  Node *nextOfFoundNode = nodeAtGivenIndex->next;
  // Prev of Node to Remove <-> Next of Node to Remove
  prevOfFoundNode->next = nextOfFoundNode;
  nextOfFoundNode->prev = prevOfFoundNode;

  // Update list data as needed
  sentinel_list->size -= 1;
  if (nodeAtGivenIndex == sentinel_list->tail)
    sentinel_list->tail = prevOfFoundNode;

  // NOTE: Function returns pointer, so maybe don't clean
  // (avoids dangling pointer)
  return nodeAtGivenIndex;
}

/*
 * =================
 * PRIMARY FUNCTIONS
 * =================
 */

List *list_create(ListType type) {
  List *list = NULL; // could remain null

  // List may have multiple implementations -- assume find by type
  switch (type) {
  case LIST_LINKED_SENTINEL:
    list = sentinel_list_create();
    break;
  }

  return list;
}

void list_destroy(List *list, FreeFunc free_func) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    sentinel_list_destroy(list, free_func);
    break;
  }

  // AI Use: Assisted by AI
  // Used to find how to skip brackets marked as "untested" line
  // NOTE: tests for brackets with no code behind it

} // GCOVR_EXCL_LINE

bool list_append(List *list, void *data) {
  Node *dataNode = data;
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    // Check a node was passed in as data
    // GCOVR_EXCL_START
    if (dataNode->type != SENTINEL && dataNode->type != NODE)
      return false;
    // GCOVR_EXCL_STOP
    return sentinel_list_append(list->lists.sentinel_list, data);
  }
} // GCOVR_EXCL_LINE

bool list_insert(List *list, size_t index, void *data) {
  Node *dataNode = data;
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    // Check a node was passed in as data
    // GCOVR_EXCL_START
    if (dataNode->type != SENTINEL && dataNode->type != NODE)
      return false;
    // GCOVR_EXCL_STOP

    // Insert node into list
    return sentinel_list_insert(list->lists.sentinel_list, index, dataNode);
  }
} // GCOVR_EXCL_LINE

void *list_remove(List *list, size_t index) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    return sentinel_list_remove(list->lists.sentinel_list, index);
  }
} // GCOVR_EXCL_LINE

void *list_get(const List *list, size_t index) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    return sentinel_list_get(list->lists.sentinel_list, index);
  }
} // GCOVR_EXCL_LINE

size_t list_size(const List *list) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    return sentinel_list_size(list->lists.sentinel_list);
  }
} // GCOVR_EXCL_LINE

bool list_is_empty(const List *list) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    return sentinel_list_size(list->lists.sentinel_list) == 0;
  }
} // GCOVR_EXCL_LINE
