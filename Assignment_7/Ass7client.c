#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define BUF_SIZE   1024
#define EXPR_SIZE  256
#define MAX_RETRIES 3

int main(void) {
    int sockfd;
    struct sockaddr_in servaddr;
    char sendbuf[BUF_SIZE], recvbuf[BUF_SIZE];
    socklen_t serv_len;
    int seq = 1;

    char server_ip[] = "10.0.0.2";   // <--- HARD CODED SERVER IP
    int port = 9000;                 // <--- HARD CODED PORT

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &servaddr.sin_addr);

    serv_len = sizeof(servaddr);

    printf("UDP Calculator Client\n");
    printf("Connected to server %s:%d\n", server_ip, port);
    printf("Type math expressions (sin(), +, -, *, /)\n");
    printf("Type 'quit' to exit\n\n");

    while (1) {
        char expr[EXPR_SIZE];

        printf("Enter expression: ");
        fflush(stdout);

        if (!fgets(expr, sizeof(expr), stdin)) break;

        size_t len = strlen(expr);
        if (len > 0 && expr[len - 1] == '\n') expr[len - 1] = '\0';

        if (strcmp(expr, "quit") == 0) break;
        if (strlen(expr) == 0) continue;

        snprintf(sendbuf, sizeof(sendbuf), "%d|%s", seq, expr);

        int retries = 0, got_reply = 0;

        while (retries < MAX_RETRIES && !got_reply) {
            sendto(sockfd, sendbuf, strlen(sendbuf), 0,
                   (struct sockaddr *)&servaddr, serv_len);

            ssize_t n = recvfrom(sockfd, recvbuf, sizeof(recvbuf) - 1, 0,
                                 (struct sockaddr *)&servaddr, &serv_len);

            if (n < 0) {
                retries++;
                printf("Timeout, retry %d/%d...\n", retries, MAX_RETRIES);
                continue;
            }

            recvbuf[n] = '\0';

            char *p1 = strchr(recvbuf, '|');
            if (!p1) continue;
            *p1 = '\0';
            int reply_seq = atoi(recvbuf);

            if (reply_seq != seq) continue;

            char *p2 = strchr(p1 + 1, '|');
            *p2 = '\0';

            char *status = p1 + 1;
            char *payload = p2 + 1;

            if (strcmp(status, "OK") == 0)
                printf("Result = %lf\n", atof(payload));
            else
                printf("Error: %s\n", payload);

            got_reply = 1;
        }

        if (!got_reply)
            printf("No response after %d retries.\n", MAX_RETRIES);

        seq++;
    }

    close(sockfd);
    return 0;
}

