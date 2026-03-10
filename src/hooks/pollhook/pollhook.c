#include "syshook.h"
#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

int main() {
    int fd = open("/sys/class/net/wlan0/statistics/rx_bytes", O_RDONLY);

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;

    char buf[64];

    while (1) {
        poll(&pfd, 1, -1);

        lseek(fd, 0, SEEK_SET);
        int n = read(fd, buf, sizeof(buf) - 1);
        buf[n] = 0;

        printf("%s\n", buf);
    }
}
