#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "master.h"
#include "hashmap.h"

#define MAX_F 100
#define MAX(x, y) x ^ ((x ^ y) & -(x < y))

struct master {
    struct table * fd_interest;
    struct future * futures[MAX_F];
    int fds[MAX_F];
};

static int current_running_task = -1;
static struct master * master_handle;

void master_register_interest(int fd) {
    for (int i = 0; i<MAX_F; i++) {
        if (master_handle->fds[i] == fd) {
            break;
        }
        if (master_handle->fds[i] == 0) {
            insert(master_handle->fd_interest, fd, current_running_task);
            master_handle->fds[i] = fd;
            break;
        }
    }
}

void master_give(struct future * fut) {
    for (int i = 0; i<MAX_F; i++) {
        if (master_handle->futures[i] == NULL) {
            master_handle->futures[i] = fut;

            break;
        }
    }
}

void master_main(void) {
    // give futures possibility to register interest
    for (int i = 0; i<MAX_F; i++) {
        if (master_handle->futures[i] != NULL) {
            struct future *fut = master_handle->futures[i];
            current_running_task = i;
            fut->poll(fut->data);
        }
    }

    fd_set rfds;
    while (1) {
        FD_ZERO(&rfds);
        int nfds = 0;
        for (int i = 0; i<MAX_F; i++) {
            if (master_handle->fds[i] != 0) {
                FD_SET(master_handle->fds[i], &rfds);
                nfds = MAX(nfds, master_handle->fds[i]);
            }
        }

        int retval = select(nfds + 1, &rfds, NULL, NULL, NULL);
        if (retval == -1)
            perror("select()");

        for (int i = 0; i<MAX_F; i++) {
            int fd = master_handle->fds[i];
            if (fd != 0 && FD_ISSET(fd, &rfds)) {
                int task_id = lookup(master_handle->fd_interest, fd);

                struct future *fut = master_handle->futures[task_id];
                current_running_task = task_id;
                fut->poll(fut->data);
            }
        }
    }
}

void master_new(void) {
    master_handle = (struct master *)malloc(sizeof(struct master));
    memset(master_handle->futures, sizeof(master_handle->futures), 0);
    memset(master_handle->fds, sizeof(master_handle->fds), 0);
    master_handle->fd_interest = createTable(10);
}
