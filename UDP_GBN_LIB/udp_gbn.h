//
// Created SunWarriorZLX
// email sunknightzlx@outlook.com
//
/*
 * 基于UDP协议设计GBN协议、
 * 以下的内容包括：
 * GBN协议的报文格式
 * 将待发送的内容转换为报文序列的两种方式（线性结构和链式结构）
 * 基于GBN协议发送和接收数据
 *
 * Design GBN protocol based on UDP protocol
 * The following content includes
 * The format of GBN frame
 * Two ways to convert the content to be sent into a message sequence (linear structure and chain structure)
 * Send and receive message based on GBN
 */

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
#define DATA_LEN 16 /* GBN帧的数据区长度 The length of the data area of the GBN frame */
#define WIN_SIZE 20 /* GBN 窗口数量 The amount of GBN Windows */
#define MAX_RESEND 5
#define TIMEOUT 40

#define PORT 8000

/* UDP-GBN帧格式 The format of UDP-GBN frame */
struct udp_gbn_frame {
    bool ack; /* ACK帧标示 */
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

struct udp_gbn_frame gen_ack_frame(int maxseq, int seq);

struct udp_gbn_frame send_rec_request(int sockfd, struct sockaddr *addr, socklen_t *addr_len);

int udp_gbn_send_data(int sockfd, struct sockaddr *addr, socklen_t *addr_len, struct udp_gbn_frame *data,
                      unint_32 data_len);

struct udp_gbn_frame *
udp_gbn_rec_data(int sockfd, struct sockaddr *addr, socklen_t *addr_len,
                 unint_32 data_len);

#endif //UDP_GBN_LIB_UDP_GBN_H
