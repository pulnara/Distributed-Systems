#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <zconf.h>
#include "tcp_client.h"
#include "client.h"

int init_tcp_socket(char* IP, unsigned int port, struct sockaddr_in client_addr) {
    int MAX_CONNECTIONS = 50;

    client_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, IP, &client_addr.sin_addr) < 0) {
        perror("Problem with client socket address initialization\n");
        exit(EXIT_FAILURE);
    }

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Problem with client socket initialization\n");
        exit(EXIT_FAILURE);
    }

    if (bind(client_socket, (struct sockaddr*) &client_addr, sizeof(client_addr)) < 0) {
        perror("Problem with binding client socket\n");
        exit(EXIT_FAILURE);
    }

    if (listen(client_socket, MAX_CONNECTIONS) < 0) {
        perror("Problem with listening to client socket\n");
        exit(EXIT_FAILURE);
    }

    return client_socket;
}

void send_token_tcp(token* msg) {
    printf("Wysylam wiadomosc od %s do %s o tresci %s - typ %d\n", msg -> source_id, msg -> dest_id, msg -> message, msg -> type);
    NEIGHBOUR_ADDR.sin_port = htons(NEIGHBOUR_PORT);
    if (inet_pton(AF_INET, NEIGHBOUR_IP, &NEIGHBOUR_ADDR.sin_addr) < 0) {
        perror("Problem with neighbour socket address initialization\n");
        exit(EXIT_FAILURE);
    }

    NEIGHBOUR_SOCKET = socket(AF_INET, SOCK_STREAM, 0);
    if (NEIGHBOUR_SOCKET < 0) {
        perror("Problem with creating neighbour socket\n");
        exit(EXIT_FAILURE);
    }

    if (connect(NEIGHBOUR_SOCKET, (const struct sockaddr*) &NEIGHBOUR_ADDR, sizeof(NEIGHBOUR_ADDR)) < 0) {
        perror("Problem with connecting with neighbour\n");
        exit(EXIT_FAILURE);
    }

    if (sendto(NEIGHBOUR_SOCKET, msg, sizeof(token), 0, (const struct sockaddr*) &NEIGHBOUR_ADDR, sizeof(NEIGHBOUR_ADDR)) != sizeof(token)) {
        perror("Problem with sending message\n");
        exit(EXIT_FAILURE);
    }

    if (close(NEIGHBOUR_SOCKET) < 0) {
        perror("Problem with closing\n");
        exit(EXIT_FAILURE);
    }
}


token acquire_token_tcp() {
    struct sockaddr_in source_addr;
    token token_buf;
    int reading_socket = accept(CLIENT_SOCKET, NULL, NULL);
    if (reading_socket < 0) {
        perror("Problem with accepting connection");
        exit(EXIT_FAILURE);
    }

    if (recvfrom(reading_socket, &token_buf, sizeof(token), 0, (struct sockaddr*) &source_addr, NULL) != sizeof(token)) {
        perror("Problem with read execution\n");
        exit(EXIT_FAILURE);
    }

    return token_buf;
}

#include "tcp_client.h"
