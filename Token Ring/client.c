#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <zconf.h>
#include <string.h>
#include "client.h"
#include "tcp_client.h"
#include "udp_client.h"

int MAX_AWAITING_MESSAGES = 10;
int multicast_socket = -1;
char* my_ID;
char* my_IP;
unsigned int listening_port;
int msg_counter = 0;
token* tokens_queue;
int MULTICAST_PORT = 9999;
char* MULTICAST_IP = "224.1.1.1";


int (*init_socket)(char* IP, unsigned int port, struct sockaddr_in client_socket);
void (*send_token)(token* msg);
void send_multicast(char* log);
token (*acquire_token)();

void generate_new_message(int signum) {
    if (msg_counter < MAX_AWAITING_MESSAGES) {
        printf("\nClient %s - generating message no %d\n", my_ID, msg_counter);

        char to_who[64];
        printf("Input ID of the destination client: ");
        scanf("%s", to_who);

        struct token new_token;
        strcpy(new_token.dest_id, to_who);
        sprintf(new_token.message, "Hi from %s - %d", my_ID, msg_counter);
        tokens_queue[msg_counter++] = new_token;
    } else {
        printf("\nClient %s - couldn't buffer more messages\n", my_ID);
    }
}

void prepare_awaiting_msg(token* buffer) {
    if (msg_counter == 0) return;
    printf("Nadam sobie wiadomosc, bo moge\n");
    buffer -> type = DATA;
    strcpy(buffer -> message, tokens_queue[0].message);
    strcpy(buffer -> source_id, my_ID);
    strcpy(buffer -> dest_id, tokens_queue[0].dest_id);

    for (int i = 0; i < msg_counter; i++) {
        tokens_queue[i] = tokens_queue[i+1];
    }
    --msg_counter;
}

void process_token(struct token received_token) {
    char log[256];
    sprintf(log, "Dostalem wiadomosc od %s do %s o tresci '%s' - typ %d",
            received_token.source_id, received_token.dest_id,
            received_token.message, received_token.type);
    printf("%s\n", log);
    send_multicast(log);

    if (strcmp(received_token.source_id, my_ID) == 0) {
        // nie zastalem odbiorcy
        printf("Przyps, nie zastalem odbiorcy\n");
    }

    if (strcmp(received_token.dest_id, my_ID) == 0) {
        // wiadomosc jest na 100% do mnie
        switch(received_token.type) {
            case CONFIRM:
                // wiadomosc to potwierdzenie
                printf("Potwierdzenie\n");
                received_token.type = EMPTY;
                strcpy(received_token.message, "Token Ring idle");
                strcpy(received_token.dest_id, "none");
                strcpy(received_token.source_id, "none");
                break;
            case DATA:
                // wiadomosc to jakas bardzo wazna informacja
                printf("Dane\n");
                received_token.type = CONFIRM;
                strcpy(received_token.message, "Potwierdzam, ze dostalem wiadomosc, dzieki");
                strcpy(received_token.dest_id, received_token.source_id);
                strcpy(received_token.source_id, my_ID);
                break;
            default:
                // to niespodzianka
//                printf("//////////////1\n");
                break;
        }
    } else {
        switch(received_token.type) {
            case EMPTY:
                // krazy sobie pusty token - moge nadac cos od siebie
                printf("Pusta\n");
                prepare_awaiting_msg(&received_token);
                break;
            case CONNECT: {
                // jakis ziomek chce sie podlaczyc
                printf("Proba podlaczenia\n");
                token t = acquire_token();
                while (t.type != EMPTY) {
                    printf("Ojojojojooj\n");
                    printf("%s %d %s\n\n", t.message, t.type, t.source_id);
                    process_token(t);
                    t = acquire_token();
                }
                t.type = REWIRE;
                strcpy(t.message, received_token.message);
//                strcpy(t.source_id, received_token.source_id);
                sprintf(t.source_id, "%s:%d", my_IP, listening_port);
                sleep(1);
                send_token(&t);
                return;
            }
            case REWIRE:
                // przepinamy lancuch, zeby wcisnac nowego kolege przed jego sasiada
                printf("Przepinamy\n");
                char buf[64];
                sprintf(buf, "%s:%d", NEIGHBOUR_IP, NEIGHBOUR_PORT);
                if (strcmp(received_token.source_id, buf) == 0) {
                    char buffer[128];
                    strcpy(buffer, received_token.message);
                    strcpy(NEIGHBOUR_IP, strtok(buffer, ":"));
                    NEIGHBOUR_PORT = (unsigned int) atoi(strtok(NULL, ":"));
                    strcpy(received_token.dest_id, strtok(NULL, ":"));
                    strcpy(received_token.message, "Witam w ringu");
                    received_token.type = CONFIRM;
                    strcpy(received_token.source_id, my_ID);
                }
                break;
            default:
                // przekaz dalej
                //printf("//////////////2 - %d\n", received_token.type);
                break;
        }
    }

    sleep(1);
    send_token(&received_token);
}

void initialize_token_ring() {
    token starting_token;
    starting_token.type = EMPTY;
    strcpy(starting_token.source_id, my_ID);
    strcpy(starting_token.dest_id, "none");
    sprintf(starting_token.message, "Token Ring idle");
    send_token(&starting_token);
}

void connect_to_token_ring() {
    token init_token;
    init_token.type = CONNECT;
    strcpy(init_token.source_id, my_ID);
    strcpy(init_token.dest_id, "unknown");
    sprintf(init_token.message, "%s:%d:%s", my_IP, listening_port, my_ID);
    send_token(&init_token);
}

void init_multicast() {
    if ((multicast_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
}

void send_multicast(char* log) {
    struct sockaddr_in multicast_addr;
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_IP);
    multicast_addr.sin_port = htons(MULTICAST_PORT);

    char msg[256];
    sprintf(msg, "%s :: %s", my_ID, log);

    if ((sendto(multicast_socket, msg, strlen(msg), 0, (struct sockaddr *) &multicast_addr, sizeof(multicast_addr))) < 0) {
        perror("sendto");
        exit(1);
    }
}

void disconnect(int signum) {
    printf("\nClient %s - disconnecting\n", my_ID);
    exit(EXIT_SUCCESS);
}

void close_socket() {
    if (close(CLIENT_SOCKET) < 0) {
        perror("Problem with closing socket\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 8) {
        printf("Invalid number of parameters given\n"
               "Proper call: ./client <ID> <IP> <listening port> <neighbour my_IP> "
           "<neigbour port> <token posession flag> <protocol>");
        exit(EXIT_FAILURE);
    }

    my_ID = argv[1];
    my_IP = argv[2];
    listening_port = (unsigned int) strtol(argv[3], NULL, 10);
    NEIGHBOUR_IP = argv[4];
    NEIGHBOUR_PORT = (unsigned int) strtol(argv[5], NULL, 10);
    int has_token = atoi(argv[6]);
    char* protocol = argv[7];

    printf("START client %s (%s:%d)\n", my_ID, my_IP, listening_port);
    printf("Reporting to %s:%d\n", NEIGHBOUR_IP, NEIGHBOUR_PORT);
    printf("Protocol: %s\n", protocol);

    tokens_queue = (token*) malloc(MAX_AWAITING_MESSAGES * sizeof(token));
    signal(SIGTSTP, generate_new_message);
    signal(SIGINT, disconnect);

    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    NEIGHBOUR_ADDR.sin_family = AF_INET;

    if (strcmp(protocol, "TCP") == 0) {
        init_socket = init_tcp_socket;
        send_token = send_token_tcp;
        acquire_token = acquire_token_tcp;
    } else if (strcmp(protocol, "UDP") == 0){
        init_socket = init_udp_socket;
        send_token = send_token_udp;
        acquire_token = acquire_token_udp;
    } else {
        perror("Invalid protocol provided.\n");
        exit(EXIT_FAILURE);
    }

    if (atexit(close_socket) < 0) {
        perror("Problem with configuring atexit.\n");
        exit(EXIT_FAILURE);
    }

    CLIENT_SOCKET = init_socket(my_IP, listening_port, client_addr);
    init_multicast();

    if (has_token == 1) {
        initialize_token_ring();
    } else if (has_token == 0) {
        connect_to_token_ring();
    } else {
        perror("Invalid has_token flag value provided.\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        process_token(acquire_token());
    }

}