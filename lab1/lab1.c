#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int main(void) {
    char *line = NULL;
    size_t cap = 0;
    ssize_t n;

    printf("Please enter text: ");
    fflush(stdout);

    n = getline(&line, &cap, stdin);

    // strip newline if present
    if (n > 0 && line[n-1] == '\n') {
        line[n-1] = '\0';
    }

    printf("Tokens:\n");

    char *saveptr = NULL;
    char *token = strtok_r(line, " ", &saveptr);

    while (token != NULL) {
        printf("  %s\n", token);
        token = strtok_r(NULL, " ", &saveptr);
    }

    free(line);
    return 0;
}
