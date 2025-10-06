#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
extern void *sbrk(intptr_t increment);
#define EXTRA_BYTES 256
#define BLOCK_SIZE 128
#define BUF_SIZE 256
struct header {
  uint64_t size;
  struct header *next;
};
static void print_out(const char *format, const void *data, size_t data_size) {
  char buf[BUF_SIZE];
  ssize_t len;
  if (data_size == sizeof(uint64_t)) {
    len = snprintf(buf, BUF_SIZE, format, *(const uint64_t *)data);
  } else {
    len = snprintf(buf, BUF_SIZE, format, *(void *const *)data);
  }
  write(STDOUT_FILENO, buf, (size_t)len);
}
int main() {
  void *base = sbrk(EXTRA_BYTES);
  struct header *b1 = (struct header *)base;
  struct header *b2 = (struct header *)((char *)base + BLOCK_SIZE);
  b1->size = BLOCK_SIZE;
  b1->next = NULL;
  b2->size = BLOCK_SIZE;
  b2->next = b1;
  print_out("first block:      %p\n", &b1, sizeof(&b1));
  print_out("second block:     %p\n", &b2, sizeof(&b2));
  print_out("first block size: %lu\n", &b1->size, sizeof(&b1->size));
  print_out("first block next: %p\n", &b1->next, sizeof(&b1->next));
  print_out("second block size: %lu\n", &b2->size, sizeof(&b2->size));
  print_out("second block next: %p\n", &b2->next, sizeof(&b2->next));

  size_t header_sz = sizeof(struct header);
  size_t data_len = BLOCK_SIZE - header_sz;
  unsigned char *b1_data = (unsigned char *)b1 + header_sz;
  unsigned char *b2_data = (unsigned char *)b2 + header_sz;
  memset(b1_data, 0, data_len);
  memset(b2_data, 1, data_len);
  for (size_t i = 0; i < data_len; i++) {
    uint64_t v = b1_data[i];
    print_out("%lu\n", &v, sizeof(v));
  }
  for (size_t i = 0; i < data_len; i++) {
    uint64_t v = b2_data[i];
    print_out("%lu\n", &v, sizeof(v));
  }
}
