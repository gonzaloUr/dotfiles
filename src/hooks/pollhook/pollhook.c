/*
#include "pollhook.h"
#include <argp.h>

static char doc[] = "poll multiple files and output in csv format";

static char args_doc[] = "";

static struct argp_option options[] = {
    {"file", 'f', "FILE", 0, "input file"}
};

struct arguments {
    char *filename;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *args = state->input;

    switch (key) {
        case 'f':
            args->filename = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv) {
    struct arguments args;
    args.filename = NULL;

    argp_parse(&argp, argc, argv, 0, 0, &args);
    printf("f: %s\n", args.filename);
}
*/

#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

int main() {
    int fd = open("/sys/class/net/wlan0/statistics/rx_bytes", O_RDONLY);

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLPRI;

    char buf[64];

    while (1) {
        poll(&pfd, 1, -1);

        lseek(fd, 0, SEEK_SET);
        int n = read(fd, buf, sizeof(buf) - 1);
        buf[n] = 0;

        printf("%s\n", buf);
    }
}
