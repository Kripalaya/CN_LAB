/*🟦 UDP FRUIT CLIENT (same as TCP client)

File: fruit_client_udp.c*/

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

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("10.0.0.2"); // server host

    printf("Enter fruit name: ");
    scanf("%s", fruit);
    printf("Enter quantity: ");
    scanf("%d", &qty);

    sprintf(buffer, "%s %d", fruit, qty);

    // Send request
    sendto(sockfd, buffer, strlen(buffer), 0,
           (struct sockaddr*)&server, sizeof(server));

    // Receive response
    socklen_t slen = sizeof(server);
    recvfrom(sockfd, buffer, sizeof(buffer), 0,
             (struct sockaddr*)&server, &slen);

    printf("\nServer Response:\n%s\n", buffer);

    close(sockfd);
    return 0;
}
