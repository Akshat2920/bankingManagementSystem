#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "customer_handler.c"
#include "employee_handler.c"
#define PORT 8080

void menu(int new_socket) {
    char buffer[1024] = {0};
    int choice;

    strcpy(buffer, "Choose the type of access:\n1. Customer\n2. Bank Employee\n3. Manager\n4. Admin\n");
    send(new_socket, buffer, strlen(buffer), 1);
    memset(buffer, 0, sizeof(buffer));
    read(new_socket, &choice, sizeof(choice));
    switch(choice) {
        case 1:
            send(new_socket, "CUSTOMER_BASE", strlen("CUSTOMER_BASE"), 0);
            handle_customer(new_socket);
            break;
        case 2:
            send(new_socket, "EMPLOYEE_BASE", strlen("EMPLOYEE_BASE"), 0);
            handle_employee(new_socket);
            break;
        case 3:
            strcpy(buffer, "Manager access chosen\n");
            break;
        case 4:
            strcpy(buffer, "Admin access chosen\n");
            break;
        default:
            strcpy(buffer, "Invalid choice\n");
    }
    send(new_socket, buffer, strlen(buffer), 0);
}

void *handle_client(void *socket_desc) {
    int new_socket = *(int*)socket_desc;

    menu(new_socket);

    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    pthread_t thread_id;

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        if (pthread_create(&thread_id, NULL, handle_client, (void *)&new_socket) < 0) {
            perror("could not create thread");
            return 1;
        }
    }

    return 0;
}
