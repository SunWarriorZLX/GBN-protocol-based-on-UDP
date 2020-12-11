//
// Created by 张力炫 on 2020/12/9.
//

#ifndef UDP_GBN_LIB_UDP_GBN_H
#define UDP_GBN_LIB_UDP_GBN_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define unint_32 unsigned int
#define DATA_LEN 16

#define PORT 8000

struct udp_gbn_frame {
    unint_32 maxseq;
    unint_32 seq;
    char data[DATA_LEN];
};
struct frames_list {
    struct udp_gbn_frame current;
    struct frames_list *next;
};


struct frames_list *gen_frames_list(char *DATA);

struct udp_gbn_frame *gen_frames_arry(char *DATA);

#endif //UDP_GBN_LIB_UDP_GBN_H
