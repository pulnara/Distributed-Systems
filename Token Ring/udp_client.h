#ifndef PULNAR_AGNIESZKA_1_UDP_CLIENT_H
#define PULNAR_AGNIESZKA_1_UDP_CLIENT_H

#include "client.h"

int CLIENT_SOCKET;

int init_udp_socket(char* IP, unsigned int port, struct sockaddr_in client_addr);
void send_token_udp(token* msg);
token acquire_token_udp();

#endif //PULNAR_AGNIESZKA_1_UDP_CLIENT_H
