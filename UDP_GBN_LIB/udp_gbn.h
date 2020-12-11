//
// Created by 张力炫 on 2020/12/9.
//

#ifndef UDP_GBN_LIB_UDP_GBN_H
#define UDP_GBN_LIB_UDP_GBN_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define unint_32 unsigned int
#define DATA_LEN 16
#define WIN_SIZE 20;
#define MAX_RESEND 5

#define PORT 8000

struct udp_gbn_frame {
    bool ack;
    unint_32 maxseq;
    unint_32 seq;
    char data[DATA_LEN];
};
struct frames_list {
    struct udp_gbn_frame current;
    struct frames_list *next;
};


struct frames_list *gen_frames_list(char *DATA, unsigned int length);

struct udp_gbn_frame *gen_frames_arry(char *DATA, unsigned int length);

struct udp_gbn_frame gen_ack_frame(int maxseq);

struct udp_gbn_frame send_rec_request(int sockfd, struct sockaddr *addr, socklen_t *addr_len);

#endif //UDP_GBN_LIB_UDP_GBN_H
