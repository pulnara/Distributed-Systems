#ifndef PULNAR_AGNIESZKA_1_TCP_CLIENT_H
#define PULNAR_AGNIESZKA_1_TCP_CLIENT_H

#include "client.h"

int NEIGHBOUR_SOCKET;
int CLIENT_SOCKET;
int init_tcp_socket(char* IP, unsigned int port, struct sockaddr_in client_addr);
void send_token_tcp(token* msg);
token acquire_token_tcp();

#endif //PULNAR_AGNIESZKA_1_TCP_CLIENT_H
