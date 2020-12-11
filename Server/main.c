#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "udp_gbn.h"

int main() {
    int server_fd;
    unsigned int client_addr_len = sizeof(client_addr_len);
    struct sockaddr_in server_addr, client_addr;
    struct udp_gbn_frame buffer;
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    recvfrom(server_fd, &buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr *) &client_addr, &client_addr_len);
    printf("%s\n", buffer.data);
    buffer.seq = 1;
    buffer.maxseq = 1;
    strcpy(buffer.data, "HELLO Client");
    sendto(server_fd, &buffer, sizeof(buffer), 0, (struct sockaddr *) &client_addr, client_addr_len);
    printf("MESSAGE SENT\n");
    return 0;
}
