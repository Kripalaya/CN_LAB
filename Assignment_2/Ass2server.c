/*TCP FRUIT SERVER (simple, uses int main() only)
Server (fruit_server.c)*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 9000

struct Fruit {
    char name[20];
    int quantity;
    char last_sold[40];
};

int main() {
    int sockfd, clientfd;
    struct sockaddr_in server, client;
    char buffer[1024];

    // Fruit database
    struct Fruit fruits[3] = {
        {"apple", 10, "NA"},
        {"banana", 15, "NA"},
        {"mango", 20, "NA"}
    };

    int unique_customers = 0;
    char customers[20][40];  // store <ip:port>

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr*)&server, sizeof(server));
    listen(sockfd, 10);

    printf("Fruit Server Running on PORT %d...\n", PORT);

    socklen_t csize = sizeof(client);

    while (1) {
        clientfd = accept(sockfd, (struct sockaddr*)&client, &csize);

        char client_id[40];
        sprintf(client_id, "%s:%d", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        // Check if new customer
        int is_new = 1;
        for (int i = 0; i < unique_customers; i++) {
            if (strcmp(customers[i], client_id) == 0) {
                is_new = 0;
                break;
            }
        }
        if (is_new) {
            strcpy(customers[unique_customers], client_id);
            unique_customers++;
        }

        // Receive fruit request
        recv(clientfd, buffer, sizeof(buffer), 0);

        char fruit_name[20];
        int qty;
        sscanf(buffer, "%s %d", fruit_name, &qty);

        int found = 0;
        for (int i = 0; i < 3; i++) {
            if (strcmp(fruits[i].name, fruit_name) == 0) {
                found = 1;

                if (fruits[i].quantity >= qty) {
                    fruits[i].quantity -= qty;

                    // update timestamp
                    time_t now = time(NULL);
                    strcpy(fruits[i].last_sold, ctime(&now));
                    fruits[i].last_sold[strcspn(fruits[i].last_sold, "\n")] = '\0';

                    sprintf(buffer,
                            "SUCCESS\nRemaining %s: %d\nLast sold: %s\nUnique customers: %d\n",
                            fruits[i].name, fruits[i].quantity, fruits[i].last_sold,
                            unique_customers);
                } else {
                    sprintf(buffer, "SORRY! Not enough stock.\nUnique customers: %d\n", unique_customers);
                }
            }
        }

        if (!found) {
            sprintf(buffer, "Fruit not available.\n");
        }
        printf("\n=== Customers who already transacted ===\n");
        for (int i = 0; i < unique_customers; i++) {
            printf("%s\n", customers[i]);
        }
        printf("========================================\n");

        send(clientfd, buffer, strlen(buffer), 0);
        close(clientfd);
    }

    close(sockfd);
    return 0;
}
