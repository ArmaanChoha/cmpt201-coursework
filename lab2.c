#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
int main() {
  char programPath[256];
  printf("Enter program to run\n");
  while (1) {
    printf(">");
    fgets(programPath, sizeof(programPath), stdin);
    programPath[strcspn(programPath, "\n")] = '\0';
    pid_t childID = fork();
    if (childID == 0) {
      execl(programPath, programPath, (char *)NULL);
    } else {
      waitpid(childID, NULL, 0);
    }
  }
}
