#include "lab.h"
#include <stdio.h>
#include <stdlib.h>
/*
 * =====
 * TYPES
 * =====
 */
typedef struct Node {
  struct Node *next, *prev;
} Node;

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

/*
 * TODO: Document
 */
void free_element(void *data_ptr) {
  Node *node = (Node *)data_ptr;
  free(node);
}

/*
 * TODO: Document
 */
List *sentinel_list_create(void) {
  List *list = malloc(sizeof(List));
  list->type = LIST_LINKED_SENTINEL;

  // Creates SentinelList Pointer
  // NOTE: allocates memory separately to optimize sizeof List
  // (avoid unnecessary allocation to unused implementations)
  list->lists.sentinel_list = malloc(sizeof(SentinelLinkedList));

  // Sentinel node will always be the head -- we want tail on the first appended
  // element later
  Node *sentinelNode = malloc(sizeof(Node));
  // Sets all meta data pointers to sentinel node initially
  list->lists.sentinel_list->head = list->lists.sentinel_list->tail =
      sentinelNode->next = sentinelNode->prev = sentinelNode;

  // Sentinel node should not count toward size
  list->lists.sentinel_list->size = 0;

  return list;
}

void sentinel_list_destroy(List *list, FreeFunc free_func) {
  SentinelLinkedList *sentinel_list = list->lists.sentinel_list;

  // User must pass (non-null) FreeFunc to destroy list
  if (free_func) {
    Node *sentinelNode = sentinel_list->head;
    Node *currNode = sentinelNode;
    Node *nextNode = currNode->next;

    while (currNode != NULL) {
      free_func(currNode);
      // Reduce size
      sentinel_list->size = list_size(list) - 1;
      // Update currNode until loops back to sentinel node
      currNode = (nextNode == sentinelNode) ? NULL : nextNode;
      // Update next node
      nextNode = (currNode) ? currNode->next : NULL;
    }

    // Finally cleanup lists
    free(sentinel_list);
    sentinel_list = NULL;
    free(list);
    list = NULL;
  }
}

bool sentinel_list_append(SentinelLinkedList *sentinel_list, void *data) {
  Node *currTail = sentinel_list->tail;
  Node *newTail = (Node *)data;

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

bool sentinel_list_insert(SentinelLinkedList *sentinel_list, size_t index,
                          void *data) {
  // Index is out of bounds
  if (index > sentinel_list->size || index < 0)
    return false;

  // Start at tail or head based on which is closest
  // (unnecessary for small sets of data)
  bool nodeIsCloseToTail = index > (sentinel_list->size / 2); // index > middle
  int currIdx = (nodeIsCloseToTail) ? sentinel_list->size - 1 : -1;
  Node *currNode =
      (nodeIsCloseToTail) ? sentinel_list->tail : sentinel_list->head;
  Node *nextNode = currNode->next;

  // Find node at given index
  // (List is empty || index within bounds)
  while (currIdx != index) {
    // ? shift backward : shift forward
    currNode = nextNode;
    currIdx += (nodeIsCloseToTail) ? -1 : 1;
  }

  // Check index was found
  if (currIdx != index)
    return false;

  Node *newNode = (Node *)data;
  Node *oldPrev = currNode->prev;
  // Curr Node Prev <-> New Node
  newNode->prev = oldPrev;
  oldPrev->next = newNode;
  // New Node <-> Curr Node
  newNode->next = currNode;
  currNode->prev = newNode;

  // Update list data as needed
  sentinel_list->tail =
      (sentinel_list->size == 0) ? newNode : sentinel_list->tail;
  sentinel_list->size += 1;

  return true;
}

size_t sentinel_list_size(SentinelLinkedList *sentinel_list) {
  return sentinel_list->size;
}

/*
 * =================
 * PRIMARY FUNCTIONS
 * =================
 */

List *list_create(ListType type) {
  List *list = NULL; // could remain null

  // List can have multiple implementations -- find by type
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
  }
}

bool list_append(List *list, void *data) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    return sentinel_list_append(list->lists.sentinel_list, data);
  }
}

bool list_insert(List *list, size_t index, void *data) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    return sentinel_list_insert(list->lists.sentinel_list, index, data);
  }
}

size_t list_size(const List *list) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    return sentinel_list_size(list->lists.sentinel_list);
  }
};

bool list_is_empty(const List *list) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    return list->lists.sentinel_list->size == 0;
  }
};
