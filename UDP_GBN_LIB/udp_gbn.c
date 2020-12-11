//
// Created by 张力炫 on 2020/12/9.
//

#include "udp_gbn.h"

struct frames_list *gen_frames_list(char *DATA) {
    if (DATA == NULL)
        return NULL;
    unint_32 maxseq = strlen(DATA) / DATA_LEN + (strlen(DATA) % DATA_LEN > 0);
    unint_32 seq = 1;
    struct frames_list *head = (struct frames_list *) malloc(sizeof(struct frames_list));
    struct frames_list *current = head;
    for (; seq < maxseq; seq++) {
        current->current.maxseq = maxseq;
        current->current.seq = seq;
        memcpy(current->current.data, DATA + DATA_LEN * (seq - 1), DATA_LEN);
        current->next = (struct frames_list *) malloc(sizeof(struct frames_list));
        current = current->next;
    }
    current->current.maxseq = maxseq;
    current->current.seq = seq;
    memcpy(current->current.data, DATA + DATA_LEN * (seq - 1), strlen(DATA) - DATA_LEN * (seq-1));
    current->next = NULL;
    return head;
}

struct udp_gbn_frame *gen_frames_arry(char *DATA) {
    if (DATA == NULL)
        return NULL;
    unint_32 maxseq = strlen(DATA) / DATA_LEN + (strlen(DATA) % DATA_LEN > 0);
    unint_32 seq = 1;
    struct udp_gbn_frame *ret = (struct udp_gbn_frame *) malloc(maxseq);
    for (; seq < maxseq; seq++) {
        ret[seq - 1].seq = seq;
        ret[seq - 1].maxseq = maxseq;
        memcpy(ret[seq - 1].data, DATA + DATA_LEN * (seq - 1), DATA_LEN);
    }
    ret[seq - 1].seq = seq;
    ret[seq - 1].maxseq = maxseq;
    memcpy(ret[seq - 1].data, DATA + DATA_LEN * (seq - 1), strlen(DATA) - DATA_LEN * (seq-1));
    return ret;
}
