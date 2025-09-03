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
  sentinelNode->next = sentinelNode->prev = list->lists.sentinel_list->head =
      list->lists.sentinel_list->tail = sentinelNode;
  // Sentinel node should not count toward size
  list->lists.sentinel_list->size = 0;

  return list;
}

void sentinel_list_destroy(List *list, FreeFunc free_func) {
  SentinelLinkedList *sentinel_list = list->lists.sentinel_list;

  // User must pass (non-null) FreeFunc to destroy
  if (free_func) {
    // we eventually get rid of all tails, so avoid derferencing NULL pointer
    Node *currNode = sentinel_list->tail;
    Node *nextNode = currNode->prev;

    while (currNode != NULL) {
      free_func(currNode);                       // frees current tail
      sentinel_list->size = list_size(list) - 1; // Reduce size
      // Update currNode if its does not point to itself
      currNode = (currNode == nextNode) ? NULL : nextNode;
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

size_t list_size(const List *list) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    return sentinel_list_size(list->lists.sentinel_list);
  }
};

bool list_append(List *list, void *data) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    return sentinel_list_append(list->lists.sentinel_list, data);
  }
}
