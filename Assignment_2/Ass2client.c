/*✅ TCP FRUIT CLIENT (simple, uses int main() only)
Client (fruit_client.c)*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9000

int main() {
    int sockfd;
    struct sockaddr_in server;
    char buffer[1024];
    char fruit[20];
    int qty;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    // Change for your Mininet host server IP
    server.sin_addr.s_addr = inet_addr("10.0.0.2");

    connect(sockfd, (struct sockaddr*)&server, sizeof(server));

    printf("Enter fruit name: ");
    scanf("%s", fruit);
    printf("Enter quantity: ");
    scanf("%d", &qty);

    sprintf(buffer, "%s %d", fruit, qty);

    send(sockfd, buffer, strlen(buffer), 0);

    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("\nServer Response:\n%s\n", buffer);

    close(sockfd);
    return 0;
}

