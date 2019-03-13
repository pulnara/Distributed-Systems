#include <i386/endian.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include "udp_client.h"
#include "client.h"


int init_udp_socket(char* IP, unsigned int port, struct sockaddr_in client_addr) {
    client_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, IP, &client_addr.sin_addr) < 0) {
        perror("Problem with client socket address initialization\n");
        exit(EXIT_FAILURE);
    }

    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("Problem with client socket initialization\n");
        exit(EXIT_FAILURE);
    }

    if (bind(client_socket, (struct sockaddr*) &client_addr, sizeof(client_addr)) < 0) {
        perror("Problem with binding client socket\n");
        exit(EXIT_FAILURE);
    }
    return client_socket;
}

void send_token_udp(token* msg) {
    NEIGHBOUR_ADDR.sin_port = htons(NEIGHBOUR_PORT);

    printf("Wysylam wiadomosc od %s do %s o tresci %s - typ %d\n", msg -> source_id, msg -> dest_id, msg -> message, msg -> type);

    if (inet_pton(AF_INET, NEIGHBOUR_IP, &NEIGHBOUR_ADDR.sin_addr) < 0) {
        perror("Problem with neighbour socket address initialization\n");
        exit(EXIT_FAILURE);
    }

    if (sendto(CLIENT_SOCKET, msg, sizeof(token), 0, (struct sockaddr*) &NEIGHBOUR_ADDR, sizeof(NEIGHBOUR_ADDR)) < 0) {
        perror("Problem with sending token to neighbour!");
        exit(EXIT_FAILURE);
    }
}

token acquire_token_udp() {
    struct sockaddr_in source_addr;
    int source_addr_len = sizeof(source_addr);
    token token_buf;
    if (recvfrom(CLIENT_SOCKET, &token_buf, sizeof(token), 0, (struct sockaddr *) &source_addr,
                 (socklen_t*) &source_addr_len) != sizeof(token)) {
        perror("Problem with recvfrom execution\n");
        exit(EXIT_FAILURE);
    } else {
        return token_buf;
    }
}


#include "udp_client.h"
