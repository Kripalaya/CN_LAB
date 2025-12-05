/*
 chat_client.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP   "10.0.0.2"  
#define SERVER_PORT 9000
#define BUF_SIZE    1024

static int sockfd = -1;
static volatile int running = 1;

void *recv_thread(void *arg) {
    (void)arg;
    char buf[BUF_SIZE];
    while (running) {
        ssize_t n = recv(sockfd, buf, sizeof(buf)-1, 0);
        if (n <= 0) {
            printf("Disconnected from server.\n");
            running = 0;
            break;
        }
        buf[n] = '\0';
        printf("%s", buf); // server already includes newline if client sent one
        fflush(stdout);
    }
    return NULL;
}

int main() {
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { perror("socket"); return 1; }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid server IP\n");
        close(sockfd);
        return 1;
    }

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    printf("Connected to chat server %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("Type messages and press Enter to send. Ctrl+D (EOF) or Ctrl+C to quit.\n");

    pthread_t rtid;
    if (pthread_create(&rtid, NULL, recv_thread, NULL) != 0) {
        perror("pthread_create");
        close(sockfd);
        return 1;
    }

    // Read from stdin and send to server
    char line[BUF_SIZE];
    while (running && fgets(line, sizeof(line), stdin) != NULL) {
        // Ensure newline at end — fgets keeps newline
        ssize_t sent = send(sockfd, line, strlen(line), 0);
        if (sent <= 0) {
            printf("Send failed or connection closed.\n");
            break;
        }
    }

    // Cleanup
    running = 0;
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    pthread_join(rtid, NULL);
    printf("Client exiting.\n");
    return 0;
}

