// client_simple.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_IP "10.0.0.2"
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
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    struct sockaddr_in srv = {0};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &srv.sin_addr) != 1) { fprintf(stderr,"bad ip\n"); return 1; }

    if (connect(sock, (struct sockaddr*)&srv, sizeof(srv)) < 0) { perror("connect"); return 1; }
    printf("Connected to server\n"); fflush(stdout);

    /* -------- 1) Download server file ------- */
    FILE *fd = fopen("downloaded_from_server.txt", "wb");
    if (!fd) { perror("fopen"); close(sock); return 1; }

    char buf[BUF];
    ssize_t n;
    while ((n = recv(sock, buf, BUF, 0)) > 0) {
        fwrite(buf, 1, (size_t)n, fd);
    }
    if (n < 0) perror("recv");
    fclose(fd);

    printf("Download complete (server finished sending). Now uploading client_file.txt\n"); fflush(stdout);

    /* -------- 2) Upload client file ------- */
    FILE *fu = fopen("client_file.txt", "rb");
    if (!fu) { fprintf(stderr, "Missing client_file.txt\n"); close(sock); return 1; }

    size_t r;
    while ((r = fread(buf, 1, BUF, fu)) > 0) {
        if (send_all(sock, buf, r) != (ssize_t)r) {
            perror("send");
            break;
        }
    }
    fclose(fu);

    /* tell server we're done uploading */
    shutdown(sock, SHUT_WR);
    printf("Upload done. Closing socket.\n"); fflush(stdout);
    close(sock);
    return 0;
}

