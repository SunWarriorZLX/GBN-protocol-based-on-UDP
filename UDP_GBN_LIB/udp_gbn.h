//
// Created by 张力炫 on 2020/12/9.
//

#ifndef UDP_GBN_LIB_UDP_GBN_H
#define UDP_GBN_LIB_UDP_GBN_H
#define DATA_LEN 16
#define unint_32 unsigned int
#include <string.h>
#include <stdlib.h>
struct udp_gbn_frame {
    unint_32 maxseq;
    unint_32 seq;
    char data[DATA_LEN];
};
struct frames_list {
    struct udp_gbn_frame current;
    struct frames_list *next;
};

struct frames_list* gen_frames_list(char *DATA);

#endif //UDP_GBN_LIB_UDP_GBN_H
