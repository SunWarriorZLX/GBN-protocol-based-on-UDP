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

struct udp_gbn_frame gen_ack_frame(int maxseq) {
    struct udp_gbn_frame ret;
    ret.seq = maxseq;
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
