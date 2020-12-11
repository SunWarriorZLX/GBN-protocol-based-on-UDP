#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "udp_gbn.h"


int main() {
    int sockfd;
    unsigned int buffer_len = sizeof(struct udp_gbn_frame);
    struct sockaddr_in server_addr;
    unsigned int server_addr_len = sizeof(server_addr);
    struct udp_gbn_frame buffer;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pthon failed");
        exit(EXIT_FAILURE);
    }
    buffer.maxseq = 1;
    buffer.seq = 1;
    strcpy(buffer.data, "HELLO SERVER");
    sendto(sockfd, &buffer, buffer_len, 0, (struct sockaddr *) &server_addr, sizeof(server_addr));
    printf("HELLO MESSAGE SENT\n");
    recvfrom(sockfd, &buffer, buffer_len, 0, (struct sockaddr *) &server_addr, &server_addr_len);
    printf("FROM SERVER %s", buffer.data);
    return 0;
}