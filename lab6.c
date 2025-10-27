#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr)                                                           \
  do {                                                                         \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Assertion failed: %s\n", #expr);                        \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)
#define TEST(expr)                                                             \
  do {                                                                         \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Test failed: %s\n", #expr);                             \
      exit(1);                                                                 \
    } else {                                                                   \
      printf("Test passed: %s\n", #expr);                                      \
    }                                                                          \
  } while (0)

typedef struct node {
  uint64_t data;
  struct node *next;
} node_t;

node_t *head = NULL;

void insert_sorted(uint64_t data) {
  node_t *n = malloc(sizeof(node_t));
  ASSERT(n);
  n->data = data;
  n->next = NULL;
  if (head == NULL || data < head->data) {
    n->next = head;
    head = n;
    return;
  }
  node_t *c = head;
  while (c->next && c->next->data <= data) {
    c = c->next;
  }
  n->next = c->next;
  c->next = n;
}

int index_of(uint64_t d) {
  node_t *c = head;
  int i = 0;
  while (c) {
    if (c->data == d) {
      return i;
    }
    c = c->next;
    i++;
  }
  return -1;
}

int main() {
  insert_sorted(1);
  insert_sorted(2);
  insert_sorted(5);
  insert_sorted(3);
  TEST(index_of(3) == 2);
  insert_sorted(0);
  insert_sorted(4);
  TEST(index_of(4) == 4);
  return 0;
}

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT2(expr)                                                          \
  do {                                                                         \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Assertion failed at %s:%d: %s\n", __FILE__, __LINE__,   \
              #expr);                                                          \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)
#define TEST2(expr)                                                            \
  do {                                                                         \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Test failed at %s:%d: %s\n", __FILE__, __LINE__,        \
              #expr);                                                          \
      exit(1);                                                                 \
    } else {                                                                   \
      printf("Test passed: %s\n", #expr);                                      \
    }                                                                          \
  } while (0)

typedef struct node2 {
  uint64_t data;
  struct node2 *next;
} node2_t;

typedef struct info {
  uint64_t sum;
} info_t;

node2_t *head2 = NULL;
info_t info = {0};

uint64_t list_sum(node2_t *h) {
  uint64_t t = 0;
  node2_t *c = h;
  while (c != NULL) {
    t += c->data;
    c = c->next;
  }
  return t;
}

void insert_sorted2(uint64_t d) {
  node2_t *n = malloc(sizeof(node2_t));
  ASSERT2(n);
  n->data = d;
  n->next = NULL;
  if (head2 == NULL || d < head2->data) {
    n->next = head2;
    head2 = n;
  } else {
    node2_t *c = head2;
    node2_t *p = NULL;
    while (c != NULL && c->data <= d) {
      p = c;
      c = c->next;
    }
    p->next = n;
    n->next = c;
  }
  info.sum += d;
  ASSERT2(info.sum == list_sum(head2));
}

int index_of2(uint64_t d) {
  node2_t *c = head2;
  int i = 0;
  while (c != NULL) {
    if (c->data == d) {
      return i;
    }
    c = c->next;
    i++;
  }
  return -1;
}

int main2() {
  insert_sorted2(1);
  insert_sorted2(3);
  insert_sorted2(5);
  insert_sorted2(2);
  TEST2(info.sum == (1 + 3 + 5 + 2));
  ASSERT2(info.sum == list_sum(head2));
  TEST2(index_of2(2) == 1);
  TEST2(index_of2(5) == 3);
  TEST2(index_of2(42) == -1);
  return 0;
}
