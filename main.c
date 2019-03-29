#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>

#include "master.h"
#include "hashmap.h"

#define PORT 1234

void poll_udp(void *fut) {
    int fd = (int) fut;
    char buf[255];
    while (1) {
        int len = recvfrom(fd, buf, 255, 0, NULL, 0);
        if (len > 0) {
            printf(buf);
        } else {
            master_register_interest(fd);
            return;
        }
    }
}

void echo_udp(void) {
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in s;

    s.sin_family = AF_INET;
    s.sin_port = htons(PORT);
    s.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(fd, (struct sockaddr*)&s, sizeof(s));
    fcntl(fd, F_SETFL, O_NONBLOCK);

    struct future *fut = (struct future*)malloc(sizeof(struct future));
    fut->data = (void *) fd;
    fut->poll = &poll_udp;
    master_give(fut);
}



int main(){
    master_new();
    echo_udp();
    master_main();
}
