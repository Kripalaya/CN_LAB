/*
 chat_server.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 9000
#define MAX_CLIENTS 100
#define BUF_SIZE 1024
#define LOG_FILE "log.txt"

static int clients[MAX_CLIENTS];
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Add a client socket to the clients array */
void add_client(int clientfd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] == 0) {
            clients[i] = clientfd;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Remove a client socket from the clients array */
void remove_client(int clientfd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] == clientfd) {
            clients[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Broadcast message to all connected clients */
void broadcast_message(const char *msg, int except_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        int fd = clients[i];
        if (fd != 0 && fd != except_fd) {
            ssize_t s = send(fd, msg, strlen(msg), 0);
            (void)s; // ignore send error here (could log)
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Append message with timestamp to log file */
void write_log(const char *msg) {
    pthread_mutex_lock(&log_mutex);
    FILE *f = fopen(LOG_FILE, "a");
    if (f) {
        time_t now = time(NULL);
        struct tm tm;
        localtime_r(&now, &tm);
        char timestr[64];
        strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", &tm);
        fprintf(f, "[%s] %s\n", timestr, msg);
        fclose(f);
    }
    pthread_mutex_unlock(&log_mutex);
}

/* Per-client handler thread */
void *client_handler(void *arg) {
    int clientfd = *((int*)arg);
    free(arg);              // allocated in main when passed
    char buf[BUF_SIZE];
    char peer[INET_ADDRSTRLEN+8] = {0};

    // Optionally get peer address for logging
    struct sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    if (getpeername(clientfd, (struct sockaddr*)&peeraddr, &len) == 0) {
        snprintf(peer, sizeof(peer), "%s:%d", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
    } else {
        strncpy(peer, "unknown", sizeof(peer));
    }

    // Announce join
    char joinmsg[BUF_SIZE];
    snprintf(joinmsg, sizeof(joinmsg), "%s joined the chat", peer);
    printf("%s\n", joinmsg);
    write_log(joinmsg);
    broadcast_message(joinmsg, clientfd);

    while (1) {
        ssize_t n = recv(clientfd, buf, sizeof(buf)-1, 0);
        if (n <= 0) break; // client closed or error
        buf[n] = '\0';

        // Prepare broadcast message with client identifier
        char out[BUF_SIZE + 64];
        snprintf(out, sizeof(out), "%s: %s", peer, buf);

        // Write to server console, log, and broadcast
        printf("%s", out);
        write_log(out);
        broadcast_message(out, clientfd);
        // Optionally echo back to sender as well; here we skip sending back to sender (except_fd)
    }

    // Client disconnect
    close(clientfd);
    remove_client(clientfd);
    char leave[BUF_SIZE];
    snprintf(leave, sizeof(leave), "%s left the chat", peer);
    printf("%s\n", leave);
    write_log(leave);
    broadcast_message(leave, -1);
    return NULL;
}

int main() {
    int sockfd, newfd;
    struct sockaddr_in servaddr, clientaddr;
    socklen_t addrlen = sizeof(clientaddr);

    // Zero client slots
    for (int i = 0; i < MAX_CLIENTS; ++i) clients[i] = 0;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { perror("socket"); return 1; }

    // Allow quick reuse
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    if (listen(sockfd, 10) < 0) {
        perror("listen");
        close(sockfd);
        return 1;
    }

    printf("Chat server listening on port %d\n", SERVER_PORT);
    // Ensure log file exists
    FILE *f = fopen(LOG_FILE, "a");
    if (f) fclose(f);

    while (1) {
        newfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addrlen);
        if (newfd < 0) {
            perror("accept");
            continue;
        }

        // Add to clients list
        add_client(newfd);

        // Pass newfd to thread via heap-allocated int
        int *pclient = malloc(sizeof(int));
        if (!pclient) {
            close(newfd);
            continue;
        }
        *pclient = newfd;

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, pclient) != 0) {
            perror("pthread_create");
            free(pclient);
            close(newfd);
            remove_client(newfd);
            continue;
        }
        pthread_detach(tid); // we won't pthread_join
    }

    // never reached normally
    close(sockfd);
    return 0;
}
