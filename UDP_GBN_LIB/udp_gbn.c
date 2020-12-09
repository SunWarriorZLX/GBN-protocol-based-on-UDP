//
// Created by 张力炫 on 2020/12/9.
//

#include "udp_gbn.h"

struct frames_list *gen_frames_list(char *DATA) {
    struct frames_list *head = (struct frames_list *) malloc(sizeof(struct frames_list));
    struct frames_list *current = head;
    unint_32 maxseq = strlen(DATA) / DATA_LEN;
    unint_32 seq = 0;
    for (; seq < maxseq; seq++) {
        current->current.maxseq = maxseq;
        current->current.seq = seq;
        memcpy(current->current.data, DATA + DATA_LEN * seq, DATA_LEN);
        current->next = (struct frames_list *) malloc(sizeof(struct frames_list));
        current = current->next;
    }
    current->current.maxseq = maxseq;
    current->current.seq = seq;
    memcpy(current->current.data, DATA + DATA_LEN * seq, strlen(DATA) - DATA_LEN * seq);
    current->next = NULL;
    return head;
}
