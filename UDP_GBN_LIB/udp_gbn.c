//
// Created by 张力炫 on 2020/12/9.
//

#include "udp_gbn.h"

struct frames_list *gen_frames_list(char *DATA, unsigned int length) {
    if (DATA == NULL)
        return NULL;
    unint_32 maxseq = length / DATA_LEN + (length % DATA_LEN > 0);
    unint_32 seq = 1;
    bool ack = false;
    struct frames_list *head = (struct frames_list *) malloc(sizeof(struct frames_list));
    struct frames_list *current = head;
    for (; seq < maxseq; seq++) {
        current->current.maxseq = maxseq;
        current->current.seq = seq;
        current->current.ack = ack;
        memcpy(current->current.data, DATA + DATA_LEN * (seq - 1), DATA_LEN);
        current->next = (struct frames_list *) malloc(sizeof(struct frames_list));
        current = current->next;
    }
    current->current.maxseq = maxseq;
    current->current.seq = seq;
    current->current.ack = ack;
    memcpy(current->current.data, DATA + DATA_LEN * (seq - 1), length - DATA_LEN * (seq - 1));
    current->next = NULL;
    return head;
}

struct udp_gbn_frame *gen_frames_arry(char *DATA, unsigned int length) {
    if (DATA == NULL)
        return NULL;
    unint_32 maxseq = length / DATA_LEN + (length % DATA_LEN > 0);
    unint_32 seq = 1;
    bool ack = false;
    struct udp_gbn_frame *ret = (struct udp_gbn_frame *) malloc(maxseq);
    for (; seq < maxseq; seq++) {
        ret[seq - 1].seq = seq;
        ret[seq - 1].maxseq = maxseq;
        ret[seq - 1].ack = ack;
        memcpy(ret[seq - 1].data, DATA + DATA_LEN * (seq - 1), DATA_LEN);
    }
    ret[seq - 1].seq = seq;
    ret[seq - 1].maxseq = maxseq;
    ret[seq - 1].ack = ack;
    memcpy(ret[seq - 1].data, DATA + DATA_LEN * (seq - 1), length - DATA_LEN * (seq - 1));
    return ret;
}

struct udp_gbn_frame gen_ack_frame(int maxseq, int seq) {
    struct udp_gbn_frame ret;
    ret.seq = seq;
    ret.maxseq = maxseq;
    ret.ack = true;
    printf("\ngen ack frame , maxseq %d\n", maxseq);
    return ret;
}

struct udp_gbn_frame send_rec_request(int sockfd, struct sockaddr *addr, socklen_t *addr_len) {
    struct udp_gbn_frame request;/*请求帧*/
    struct udp_gbn_frame ret;/*返回帧*/
    /*生成请求帧，dada[0]初始化为全1作为辨识标志*/
    request.maxseq = 1;
    request.ack = false;
    request.seq = 1;
    request.data[0] = 0x11;
    int reclen;/*返回帧长*/
    for (int i = 0; i < MAX_RESEND; i++) {
        sendto(sockfd, &request, sizeof(struct udp_gbn_frame), 0, addr, *addr_len);
        printf("\n request sent\n");
        reclen = recvfrom(sockfd, &ret, sizeof(struct udp_gbn_frame), 0, addr, addr_len);/*非阻塞模式接收*/
        if (reclen == -1) {
            printf("\n%d can not receive ack , 20s later retry\n", i);
            sleep(20);
            continue;
        }
        printf("\nreceive ack\n");
        return ret;
    }
    perror("can not receive ack , request failed");
    exit(EXIT_FAILURE);
}

int udp_gbn_send_data(int sockfd, struct sockaddr *addr, socklen_t *addr_len, struct udp_gbn_frame *data,
                      unint_32 data_len) {
    bool *rcvpkt = (bool *) malloc(sizeof(bool) * (data_len + 1));
    rcvpkt[0] = true;
    /*用于判断ack是否连续，通过将rcvpkt[0]设置为1，每次接收新的分组时只需要检查rcvpkt[k-1]是否为1即可知道ack包是否连续*/
    int base = 1;
    int nextseq = 1;
    int maxseq = data[0].maxseq;
    clock_t start;
    bool flag = false;/*计时标志 Timing flag*/
    printf("\nSend message by GBN-UDP to Client\n");
    while (base <= maxseq) {
        if (nextseq < base + WIN_SIZE && nextseq <= maxseq) {
            sendto(sockfd, data + nextseq - 1, sizeof(struct udp_gbn_frame), 0, (struct sockaddr *) addr,
                   *addr_len);
            printf("\nSend frame %d\n", nextseq);
            rcvpkt[nextseq] = false;
            if (base == nextseq) {
                printf("\nStart Timer\n");
                start = clock();
                flag = true;
            }
            nextseq += 1;
            sleep(2);
        }
        if (flag && clock() - start > TIMEOUT) {
            printf("\nTime out\n");
            printf("\nStart Timer\n");
            start = clock();
            flag = true;
            for (int i = base; i < nextseq; i++) {
                sendto(sockfd, data + i - 1, sizeof(struct udp_gbn_frame), 0, (struct sockaddr *) addr,
                       *addr_len);
                printf("\nResend frame %d\n", i);
            }
        }
        struct udp_gbn_frame ack;
        if (recvfrom(sockfd, &ack, sizeof(struct udp_gbn_frame), 0, (struct sockaddr *) addr, addr_len) > 0) {
            printf("\nReceive ack frame %d\n", ack.seq);
            rcvpkt[ack.seq] = true;
            if (ack.seq == base) {
                while (rcvpkt[base] == true && base < nextseq)
                    base += 1;
                printf("\nSlide window to base = %d\n", base);
            }
            if (base == nextseq) {
                flag = false;
            } else {
                start = clock();
                flag = true;
            }
        }
    }
    printf("\nFinished\n");
    return 1;
}

struct udp_gbn_frame *
udp_gbn_rec_data(int sockfd, struct sockaddr *addr, socklen_t *addr_len,
                 unint_32 data_len) {
    struct udp_gbn_frame *ret = (struct udp_gbn_frame *) malloc(sizeof(struct udp_gbn_frame) * data_len);
    struct udp_gbn_frame buffer;
    struct udp_gbn_frame ack;
    int expected = 1;
    for (; expected <= data_len;) {
        if (recvfrom(sockfd, &buffer, sizeof(struct udp_gbn_frame), 0, addr, addr_len) > 0) {
            printf("\nReceive frame %d\n", buffer.seq);
            if (buffer.seq == expected) {
                *(ret + expected - 1) = buffer;
                ack = gen_ack_frame(data_len, expected);
                sendto(sockfd, &ack, sizeof(struct udp_gbn_frame), 0, addr, *addr_len);
                printf("\nFrame %d accept\n", expected);
                expected += 1;
            }
        }
    }
    printf("\nFinish\n");
    return ret;
}