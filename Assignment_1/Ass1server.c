/*Assigment 1 server program
TCP SERVER in C (server.c)*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to IP/Port
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Listen
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server is listening on port 5000...\n");

    // Accept client connection
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("Accept failed");
        exit(1);
    }

    printf("Client connected!\n");

    // Receive "Hi"
    recv(client_fd, buffer, sizeof(buffer), 0);
    printf("Client says: %s\n", buffer);

    // Send "Hello"
    send(client_fd, "Hello", strlen("Hello") + 1, 0);

    close(client_fd);
    close(server_fd);
    
    return 0;
}
