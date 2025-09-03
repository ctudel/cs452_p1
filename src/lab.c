#include "lab.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * ==========================
 * HELPER FUNCTIONS AND TYPES
 * ==========================
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
 * TODO: Document
 */
void free_list_or_element(void *data_ptr) {
  Node *node = (Node *)data_ptr;

  // Only clean tail if specified
  if (node->next != node) { // handles double free
    free(node->next);
  }

  free(node);
  node = node->next = node->prev = NULL;
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
  sentinelNode->next = sentinelNode;
  sentinelNode->prev = sentinelNode;

  list->lists.sentinel_list->head = sentinelNode;
  list->lists.sentinel_list->tail = sentinelNode;
  list->lists.sentinel_list->size = 0;

  return list;
}

void sentinel_list_destroy(List *list, FreeFunc free_func) {
  SentinelLinkedList *sentinel_list = list->lists.sentinel_list;

  // User must pass (non-null) FreeFunc to destroy
  if (free_func) {
    Node *currNode = sentinel_list->tail;

    // NOTE: the current tail should free first, along with each current node,
    // so freeing prev is not necessary (different from setting to NULL)
    while (currNode != NULL) {
      // Shift tail
      sentinel_list->tail = currNode->prev;
      // Cleanup previous tail (prev will free itself next loop)
      free_func(currNode);
      // Reduce size
      sentinel_list->size = list_size(list) - 1;
      // Update current node to a different node (not itself)
      currNode = (currNode == list->lists.sentinel_list->tail)
                     ? NULL
                     : list->lists.sentinel_list->tail;
    }

    // Finally cleanup lists
    free(sentinel_list);
    sentinel_list = NULL;
    free(list);
    list = NULL;
  }
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
  List *list;

  // List can have multiple implementations -- find by type
  switch (type) {
  case LIST_LINKED_SENTINEL:
    list = sentinel_list_create();
    break;

  default:
    list = NULL;
    break;
  }

  return list;
}

void list_destroy(List *list, FreeFunc free_func) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    sentinel_list_destroy(list, free_func);
    break;
  default:

    break;
  }
}

size_t list_size(const List *list) {
  switch (list->type) {
  case LIST_LINKED_SENTINEL:
    return sentinel_list_size(list->lists.sentinel_list);
    break;

  default:
    return -1;
    break;
  }
};
