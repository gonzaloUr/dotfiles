#include "hooks.h"
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

enum kind {
    WAITS_ON_FD,
    WRITES_TO_FD,
    EVERY_N_SEC
}

struct arg {
    enum kind kind;
    void *(*new)();
    void (*pass_fd)(int fd);
    int (*get_fd)();
    void (*start)(void* userdata);
    void (*stop)(void* userdata);
    void (*free)(void* userdata);
};

#include "config.h"

int main() {
    size_t n = sizeof(args) / sizeof(struct arg);
    int (*pipefds)[2] = malloc(sizeof(int) * 2 * n);
    void **userdatas = malloc(sizeof(void*) * n);

    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1 failed");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        if (pipe(pipefds[i]) == -1) {
            perror("pipe failed");
            return 1;
        }

        userdatas[i] = args[i].init(pipefds[i]);

        struct epoll_event ev = { .events = EPOLLIN, .data.fd = pipefds[i][0] };
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, pipefds[i][0], &ev) == -1) {
            perror("epoll_ctl failed");
            return 1;
        }
    }

    struct epoll_event *events = malloc(sizeof(struct epoll_event) * n);

    while (1) {
        int count = epoll_wait(epfd, events, n, -1);
        if (count == -1) {
            perror("epoll_wait failed");
            return 1;
        }
    }

    return 0;
}
