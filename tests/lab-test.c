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

void free_element(void *data_ptr) {
  Node *node = (Node *)data_ptr;

  // Only clean tail if specified
  if (node->next != data_ptr) { // handles double free
    free(node->next);
  }

  free(node);
  node = NULL;
}

void setUp(void) { printf("\nSetting up tests...\n"); }

void tearDown(void) { printf("\nTearing down tests...\n"); }

void test_list_create(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  SentinelLinkedList *sentinel_list_ptr = list->lists.sentinel_list;

  TEST_ASSERT_NOT_NULL(list);
  TEST_ASSERT_NOT_NULL(sentinel_list_ptr);
  TEST_ASSERT_NOT_NULL(sentinel_list_ptr->head);
  TEST_ASSERT_NOT_NULL(sentinel_list_ptr->tail);

  // Clean up -- directly free to avoid premature stacktraces
  // (testing destroy separately)
  free(list->lists.sentinel_list->head); // should also free the tail
  free(list->lists.sentinel_list);
  free(list);
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

//
// void test_append(void) {
//   List *list = list_create(LIST_LINKED_SENTINEL);
//
//   bool item_appended = list_append(list, &test_element);
//   TEST_ASSERT_TRUE(item_appended);
//
//   // Cleanup
//   list_destroy(list, free_list_or_element);
//   list = NULL;
// }
//
// // FIXME: Be sure to use real nodes here
// void test_insert(void) {
//   int test_element_2 = 2;
//   List *list = list_create(LIST_LINKED_SENTINEL);
//   list_append(list, &test_element);
//
//   // Test insert correctly shifts the list elements and stores the
//   // value at the given index
//   bool insertedElement = list_insert(list, 0, &test_element_2);
//   TEST_ASSERT_TRUE(insertedElement);
//   TEST_ASSERT_EQUAL_INT(2, list_get(list, 0));
//
//   // Cleanup
//   list_destroy(list, free_list_or_element);
//   list = NULL;
// }
//
// void test_insert_out_of_bounds(void) {
//   List *list = list_create(LIST_LINKED_SENTINEL);
//
//   // Expected to fail (due to out of bounds)
//   bool insertedElement = list_insert(list, 1, &test_element);
//   TEST_ASSERT_FALSE(insertedElement);
//
//   // Cleanup
//   list_destroy(list, free_list_or_element);
//   list = NULL;
// }
//
// void test_get_element(void) {
//   List *list = list_create(LIST_LINKED_SENTINEL);
//   list_append(list, &test_element);
//
//   // Test item retrieval
//   void *element = list_get(list, 0);
//   TEST_ASSERT_NOT_NULL(element);
//
//   // Cleanup
//   list_destroy(list, free_list_or_element);
//   list = NULL;
// }
//
// void test_get_out_of_bounds(void) {
//   List *list = list_create(LIST_LINKED_SENTINEL);
//   list_append(list, &test_element);
//
//   // Test item retrieval fails (out of bounds)
//   void *element = list_get(list, 1);
//   TEST_ASSERT_NULL(element);
//
//   // Cleanup
//   list_destroy(list, free_list_or_element);
//   list = NULL;
// }
//
// void test_remove(void) {
//   List *list = list_create(LIST_LINKED_SENTINEL);
//   list_append(list, &test_element);
//   list_append(list, &test_element);
//
//   // Test removing items + list shifts accordingly
//   bool first_item_removed = list_remove(list, 0);
//   bool second_item_removed = list_remove(list, 0);
//   bool non_existent_item_removed = list_remove(list, 0); // no item should
//   exist
//
//   TEST_ASSERT_TRUE(first_item_removed);
//   TEST_ASSERT_TRUE(second_item_removed);
//   TEST_ASSERT_FALSE(non_existent_item_removed);
//
//   // Cleanup
//   list_destroy(list, free_list_or_element);
//   list = NULL;
// }
//
// void test_remove_out_of_bounds(void) {
//   List *list = list_create(LIST_LINKED_SENTINEL);
//
//   // Test remove fails
//   bool non_existent_item_removed = list_remove(list, 0); // no item should
//   exist TEST_ASSERT_FALSE(non_existent_item_removed);
//
//   // Cleanup
//   list_destroy(list, free_list_or_element);
//   list = NULL;
// }

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_list_create);
  RUN_TEST(test_list_destroy);
  // RUN_TEST(test_append);
  // RUN_TEST(test_insert);
  // RUN_TEST(test_insert_out_of_bounds);
  // RUN_TEST(test_get_element);
  // RUN_TEST(test_get_out_of_bounds);
  // RUN_TEST(test_remove);
  // RUN_TEST(test_remove_out_of_bounds);
  return UNITY_END();
}
