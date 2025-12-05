// server_simple.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9001
#define BUF 1024

ssize_t send_all(int s, const void *buf, size_t len) {
    size_t sent = 0; const char *p = buf;
    while (sent < len) {
        ssize_t n = send(s, p + sent, len - sent, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) break;
        sent += (size_t)n;
    }
    return (ssize_t)sent;
}

int main(void) {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) { perror("socket"); return 1; }

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    struct sockaddr_in srv = {0}, cli;
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr*)&srv, sizeof(srv)) < 0) { perror("bind"); return 1; }
    if (listen(listenfd, 1) < 0) { perror("listen"); return 1; }

    printf("Server running on port %d\n", PORT); fflush(stdout);

    socklen_t len = sizeof(cli);
    while (1) {
        int c = accept(listenfd, (struct sockaddr*)&cli, &len);
        if (c < 0) { perror("accept"); continue; }
        printf("Client connected\n"); fflush(stdout);

        /* -------- 1) Send server_file.txt ------- */
        FILE *fs = fopen("server_file.txt", "rb");
        if (!fs) { fprintf(stderr, "Missing server_file.txt\n"); close(c); continue; }

        char buf[BUF];
        size_t r;
        while ((r = fread(buf, 1, BUF, fs)) > 0) {
            if (send_all(c, buf, r) != (ssize_t)r) {
                perror("send");
                break;
            }
        }
        fclose(fs);

        /* tell client we're done sending */
        shutdown(c, SHUT_WR);
        printf("Sent server_file.txt, now waiting for upload...\n"); fflush(stdout);

        /* -------- 2) Receive client's upload ------- */
        FILE *fr = fopen("received_from_client.txt", "wb");
        if (!fr) { perror("fopen"); close(c); continue; }

        ssize_t n;
        while ((n = recv(c, buf, BUF, 0)) > 0) {
            fwrite(buf, 1, (size_t)n, fr);
        }
        if (n < 0) perror("recv");

        fclose(fr);
        printf("Received upload (maybe empty if client had no file). Connection closing.\n"); fflush(stdout);

        close(c);
    }

    close(listenfd);
    return 0;
}

