/*
Questions to answer at top of client.c:
(You should not need to change the code in client.c)
1. What is the address of the server it is trying to connect to (IP address and
port number).
Ip num is 127.0.0.1 port is 8000
2. Is it UDP or TCP? How do you know?
TCP because aSOCK_STREAM
3. The client is going to send some data to the server. Where does it get this
data from? How can you tell in the code?
Keyboard stdina using read STDIN_FILENO
4. How does the client program end? How can you tell that in the code?
it ends when read form stdin returns <= 1 meaning blank line or eof
*/

/* ================================
   BEGIN CLIENT.C (your original)
   ================================ */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000
#define BUF_SIZE 64
#define ADDR "127.0.0.1"

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main() {
  struct sockaddr_in addr;
  int sfd;
  ssize_t num_read;
  char buf[BUF_SIZE];

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, ADDR, &addr.sin_addr) <= 0) {
    handle_error("inet_pton");
  }

  int res = connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
  if (res == -1) {
    handle_error("connect");
  }

  while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 1) {
    if (write(sfd, buf, num_read) != num_read) {
      handle_error("write");
    }
    printf("Just sent %zd bytes.\n", num_read);
  }

  if (num_read == -1) {
    handle_error("read");
  }

  close(sfd);
  exit(EXIT_SUCCESS);
}

/* ================================
   BEGIN SERVER.C (your original)
   ================================ */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 64
#define LISTEN_BACKLOG 32

#define handle_error_server(msg)                                               \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

// Shared counters
int total_message_count = 0;
int client_id_counter = 1;

// Mutexes
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_id_mutex = PTHREAD_MUTEX_INITIALIZER;

struct client_info {
  int cfd;
  int client_id;
};

void *handle_client(void *arg) {
  struct client_info *client = (struct client_info *)arg;

  int cfd = client->cfd;
  int id = client->client_id;
  free(client);

  char buf[BUF_SIZE];
  ssize_t num_read;

  for (;;) {
    num_read = read(cfd, buf, BUF_SIZE - 1);
    if (num_read <= 0) {
      break;
    }

    buf[num_read] = '\0';

    pthread_mutex_lock(&count_mutex);
    total_message_count++;
    int msg_num = total_message_count;
    pthread_mutex_unlock(&count_mutex);

    printf("Msg #%4d; Client ID %d: %s", msg_num, id, buf);
    if (buf[num_read - 1] != '\n')
      printf("\n");
    fflush(stdout);
  }

  printf("Ending thread for client %d\n", id);
  close(cfd);

  return NULL;
}

/* 🔥 IMPORTANT 🔥
   Renamed server main() → server_main()
   so this combined file has only ONE real main()
*/
int server_main() {
  struct sockaddr_in addr;
  int sfd;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error_server("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
    handle_error_server("bind");
  }

  if (listen(sfd, LISTEN_BACKLOG) == -1) {
    handle_error_server("listen");
  }

  for (;;) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int cfd = accept(sfd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (cfd == -1) {
      if (errno == EINTR)
        continue;
      handle_error_server("accept");
    }

    int my_id;
    pthread_mutex_lock(&client_id_mutex);
    my_id = client_id_counter++;
    pthread_mutex_unlock(&client_id_mutex);

    struct client_info *info = malloc(sizeof(struct client_info));
    info->cfd = cfd;
    info->client_id = my_id;

    printf("New client created! ID %d on socket FD %d\n", my_id, cfd);

    pthread_t tid;
    pthread_create(&tid, NULL, handle_client, info);
    pthread_detach(tid);
  }

  if (close(sfd) == -1) {
    handle_error_server("close");
  }

  return 0;
}
