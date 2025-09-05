#include "../src/lab.h"
#include "harness/unity.h"
#include "harness/unity_internals.h"
#include <stdio.h>
#include <stdlib.h>

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

int test_element = 4; // int simply for testing

extern void free_element(void *data_ptr);

void setUp(void) { printf("\nSetting up tests...\n"); }

void tearDown(void) { printf("\nTearing down tests...\n"); }

void test_list_create(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  SentinelLinkedList *sentinel_list_ptr = list->lists.sentinel_list;

  TEST_ASSERT_NOT_NULL(list);
  TEST_ASSERT_NOT_NULL(sentinel_list_ptr);
  TEST_ASSERT_NOT_NULL(sentinel_list_ptr->head);
  TEST_ASSERT_NOT_NULL(sentinel_list_ptr->tail);
  TEST_ASSERT_EQUAL(sentinel_list_ptr->head, sentinel_list_ptr->tail);

  // Free manually to avoid premature stacktraces (tests destroy separately)
  free(list->lists.sentinel_list->head); // frees sentinel
  free(list->lists.sentinel_list);
  free(list);
  sentinel_list_ptr = NULL;
  list = NULL;
}

void test_list_destroy(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  list_destroy(list, free_element);
  list = NULL;

  // TEST_ASSERT_NULL(sentinel_list_ptr->head);
  // TEST_ASSERT_NULL(sentinel_list_ptr->tail);
  TEST_ASSERT_NULL(list);
}

void test_append(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  SentinelLinkedList *sentinel_list = list->lists.sentinel_list;
  Node *newNode = malloc(sizeof(Node));
  Node *newNode2 = malloc(sizeof(Node));

  // Append an item
  bool item_appended = list_append(list, newNode);
  TEST_ASSERT_TRUE(item_appended);
  // Check node addresses are different
  TEST_ASSERT_NOT_EQUAL(sentinel_list->head, sentinel_list->tail);
  // Sentinel -> New node
  TEST_ASSERT_EQUAL(sentinel_list->head->next, sentinel_list->tail);
  TEST_ASSERT_EQUAL(sentinel_list->head->prev, sentinel_list->tail);
  // New node -> Sentinel
  TEST_ASSERT_EQUAL(sentinel_list->tail->prev, sentinel_list->head);
  TEST_ASSERT_EQUAL(sentinel_list->tail->next, sentinel_list->head);

  // Append second item (testing shifting and pointers)
  bool item_appended2 = list_append(list, newNode2);
  // Sentinel -> New node
  TEST_ASSERT_EQUAL(sentinel_list->head->prev, sentinel_list->tail);
  // New node -> Sentinel
  TEST_ASSERT_EQUAL(sentinel_list->tail->next, sentinel_list->head);
  // Sentinel !(->) New Node
  TEST_ASSERT_NOT_EQUAL(sentinel_list->head->next, sentinel_list->tail);
  // New Node !(->) Sentinel
  TEST_ASSERT_NOT_EQUAL(sentinel_list->tail->prev, sentinel_list->head);

  // Cleanup
  list_destroy(list, free_element);
  list = NULL;
  sentinel_list = NULL;
  newNode = NULL;
}

void test_insert(void) {
  int test_element_2 = 2;
  List *list = list_create(LIST_LINKED_SENTINEL);
  SentinelLinkedList *sentinel_list = list->lists.sentinel_list;
  Node *newNode = malloc(sizeof(Node));

  // Test insert stores the node at the given index
  bool insertedElement = list_insert(list, 0, newNode);
  TEST_ASSERT_TRUE(insertedElement);
  // Sentinel -> New node
  TEST_ASSERT_EQUAL(sentinel_list->head->next, sentinel_list->tail);
  TEST_ASSERT_EQUAL(sentinel_list->head->prev, sentinel_list->tail);
  // New node -> Sentinel
  TEST_ASSERT_EQUAL(sentinel_list->head->next, sentinel_list->tail);
  TEST_ASSERT_EQUAL(sentinel_list->head->prev, sentinel_list->tail);

  // Test insert shifts the list correctly and stores the node
  Node *newNode2 = malloc(sizeof(Node));
  bool insertedElement2 = list_insert(list, 0, newNode2);
  TEST_ASSERT_TRUE(insertedElement2);
  TEST_ASSERT_NOT_EQUAL(sentinel_list->head->next, sentinel_list->tail);
  TEST_ASSERT_EQUAL(sentinel_list->head->next, sentinel_list->tail->prev);

  // Cleanup
  list_destroy(list, free_element);
  sentinel_list = NULL;
  list = NULL;
}

void test_insert_out_of_bounds(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  Node *newNode = malloc(sizeof(Node));

  // Expected to fail (due to out of bounds)
  bool insertedElement = list_insert(list, 1, newNode);
  TEST_ASSERT_FALSE(insertedElement);

  // Cleanup
  list_destroy(list, free_element);
  free(newNode);
  list = NULL;
  newNode = NULL;
}

void test_get_element(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  Node *node1 = malloc(sizeof(Node));
  Node *node2 = malloc(sizeof(Node));
  list_append(list, node1);
  list_append(list, node2);

  // Test item retrieval
  void *foundNode1 = list_get(list, 0);
  void *foundNode2 = list_get(list, 1);
  TEST_ASSERT_NOT_NULL(foundNode1);
  TEST_ASSERT_NOT_NULL(foundNode2);
  TEST_ASSERT_EQUAL(node1, foundNode1);
  TEST_ASSERT_EQUAL(node2, foundNode2);

  // Cleanup
  list_destroy(list, free_element);
  node1 = node2 = NULL;
  list = NULL;
}

void test_get_out_of_bounds(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  Node *node1 = malloc(sizeof(Node));
  list_append(list, node1);

  // Test item retrieval
  void *outOfBoundsNode = list_get(list, 1);
  TEST_ASSERT_NULL(outOfBoundsNode);

  // Cleanup
  list_destroy(list, free_element);
  node1 = NULL;
  list = NULL;
}

void test_remove(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  Node *node1 = malloc(sizeof(Node));
  Node *node2 = malloc(sizeof(Node));
  Node *node3 = malloc(sizeof(Node));
  list_append(list, node1);
  list_append(list, node2);
  list_append(list, node3);

  // Test removing items + list shifts accordingly
  void *first_item_removed = list_remove(list, 0);  // removes node 1
  void *second_item_removed = list_remove(list, 1); // removes node 3
  void *third_item_removed = list_remove(list, 0);  // removes node 2

  TEST_ASSERT_NOT_NULL(first_item_removed);
  TEST_ASSERT_NOT_NULL(second_item_removed);
  TEST_ASSERT_NOT_NULL(third_item_removed);
  TEST_ASSERT_EQUAL(node1, first_item_removed);
  TEST_ASSERT_EQUAL(node3, second_item_removed);
  TEST_ASSERT_EQUAL(node2, third_item_removed);

  // Cleanup
  free(node1);
  free(node2);
  free(node3);
  list_destroy(list, free_element);
  list = NULL;
  node1 = node2 = node3 = first_item_removed = second_item_removed =
      third_item_removed = NULL;
}

void test_remove_out_of_bounds(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  SentinelLinkedList *sentinel_list = list->lists.sentinel_list;
  Node *newNode = malloc(sizeof(Node));
  list_append(list, newNode);

// Test remove fails
  void *non_existent_item_removed = list_remove(list, 1);
  void *item_removed = list_remove(list, 0);
  void *non_existent_item_removed_2 = list_remove(list, 0);
  printf("\nRemoved: %p\n", non_existent_item_removed_2);

  TEST_ASSERT_NULL(non_existent_item_removed);
  TEST_ASSERT_NOT_NULL(item_removed);
  TEST_ASSERT_NULL(non_existent_item_removed_2);

  // Cleanup
  list_destroy(list, free_element);
  free(item_removed);
  list = NULL;
  item_removed = non_existent_item_removed_2 = non_existent_item_removed =
      newNode = NULL;
}

void test_list_is_empty(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  Node *newNode = malloc(sizeof(Node));

  // Test remove fails
  TEST_ASSERT_TRUE(list_is_empty(list));
  list_append(list, newNode);
  TEST_ASSERT_FALSE(list_is_empty(list));

  // Cleanup
  list_destroy(list, free_element);
  list = NULL;
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_list_create);
  RUN_TEST(test_list_destroy);
  RUN_TEST(test_append);
  RUN_TEST(test_insert);
  RUN_TEST(test_insert_out_of_bounds);
  RUN_TEST(test_get_element);
  RUN_TEST(test_get_out_of_bounds);
  RUN_TEST(test_remove);
  RUN_TEST(test_remove_out_of_bounds);
  RUN_TEST(test_list_is_empty);
  return UNITY_END();
}
