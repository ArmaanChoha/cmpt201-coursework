#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HISTORY 5

int main() {
  char *hist[HISTORY] = {0};
  size_t seen = 0;
  char *line = NULL;
  size_t cap = 0;
  for (;;) {
    printf("input: ");
    ssize_t len = getline(&line, &cap, stdin);
    if (len == -1)
      break;
    size_t slot = seen % HISTORY;
    free(hist[slot]);
    hist[slot] = strdup(line);
    seen++;
    if (strcmp(line, "print\n") == 0) {
      size_t start = (seen > HISTORY) ? (seen - HISTORY) : 0;
      for (size_t i = start; i < seen; i++)
        if (hist[i % HISTORY])
          fputs(hist[i % HISTORY], stdout);
    }
  }
  free(line);
  for (int i = 0; i < HISTORY; i++)
    free(hist[i]);
  return 0;
}
