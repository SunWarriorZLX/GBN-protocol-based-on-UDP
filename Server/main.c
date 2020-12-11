#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "udp_gbn.h"

char *LoadFile(int *length) {
    /************读取待发送文本 Loading file*************/
    FILE *fp;
    if ((fp = fopen("/Users/zhanglixuan/GBN-protocol-based-on-UDP/Server/Saying Good-bye to Cambridge Again.txt",
                    "r")) == NULL) {
        perror("file open failed");
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0, SEEK_END);
    *length = ftell(fp);
    char *data = (char *) malloc(ftell(fp));
    char *cache = (char *) malloc(255);
    rewind(fp);
    while ((fgets(cache, 255, fp)) != NULL)
        strcat(data, cache);
    fclose(fp);
    free(cache);
    return data;
    /*************************************************/
}

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

    /**等待用户发出请求 waiting for request from client***/
    for (int i = 0; i < 100; i++) {
        printf("\nwaiting for request\n");
        int reclen;
        int sendlen;
        reclen = recvfrom(server_fd, &buffer, sizeof(struct udp_gbn_frame), MSG_WAITALL,
                          (struct sockaddr *) &client_addr,
                          &client_addr_len);
        if (buffer.data[0] == 0x11) {
            printf("\nreceive request frame from client\n");
            int length;
            char *data = LoadFile(&length);
            int maxseq = length / DATA_LEN + (length % DATA_LEN > 0);
            struct udp_gbn_frame ack = gen_ack_frame(maxseq);
            struct udp_gbn_frame *frame_arry;
            printf("\nsend ack frame\n");
            sendlen = sendto(server_fd, &ack, sizeof(struct udp_gbn_frame), 0, (struct sockaddr *) &client_addr,
                             client_addr_len);
            sleep(5);
            frame_arry = gen_frames_arry(data, length);
            /*
            for(int j=0;j<length;j++){
                printf("%s",frame_arry[j].data);
            }
            */
        }
    }
    /*************************************************/


    /*
    recvfrom(server_fd, &buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr *) &client_addr, &client_addr_len);
    printf("%s\n", buffer.data);
    buffer.seq = 1;
    buffer.maxseq = 1;
    strcpy(buffer.data, "HELLO Client");
    sendto(server_fd, &buffer, sizeof(buffer), 0, (struct sockaddr *) &client_addr, client_addr_len);
    printf("MESSAGE SENT\n");
    */
    return 0;
}
