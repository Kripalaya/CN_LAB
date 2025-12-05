/*Assignment client program
TCP CLIENT in C (client.c)*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // Create TCP socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Server details
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = inet_addr("10.0.0.2");

    // Connect to server
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    // Send "Hi"
    send(sock_fd, "Hi", strlen("Hi") + 1, 0);

    // Receive "Hello"
    recv(sock_fd, buffer, sizeof(buffer), 0);
    printf("Server says: %s\n", buffer);

    close(sock_fd);
    return 0;
}

