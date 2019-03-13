#ifndef PULNAR_AGNIESZKA_1_CLIENT_H
#define PULNAR_AGNIESZKA_1_CLIENT_H


struct sockaddr_in NEIGHBOUR_ADDR;
unsigned int NEIGHBOUR_PORT;
char* NEIGHBOUR_IP;


typedef enum token_type {
    EMPTY, CONNECT, REWIRE, DISCONNECT, DATA, CONFIRM
} token_type;

typedef struct token {
    token_type type;
    char source_id[64];
    char dest_id[64];
    char message[128];
} token;


#endif //PULNAR_AGNIESZKA_1_CLIENT_H
