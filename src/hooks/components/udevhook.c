#include "udevhook.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

int main(int argc, char **argv) {
    // Variables.
    struct udev *udev;
    struct udev_monitor *mon;
    int fd;

    // Create udev object.
    udev = udev_new();
    if (!udev) {
        fprintf(stderr, "Cannot create udev context.\n");
        return 1;
    }

    // Set up a monitor to listen for all events.
    mon = udev_monitor_new_from_netlink(udev, "udev");
    if (!mon) {
        fprintf(stderr, "Cannot create udev monitor.\n");
        udev_unref(udev);
        return 1;
    }

    // Start receiving events.
    if (udev_monitor_enable_receiving(mon) < 0) {
        fprintf(stderr, "Cannot enable udev monitor.\n");
        udev_monitor_unref(mon);
        udev_unref(udev);
        return 1;
    }

    // Get fd to listen on.
    fd = udev_monitor_get_fd(mon);

    while (1) {
        // Create fd set for select.
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        // Wait for an event.
        int ret = select(fd + 1, &fds, NULL, NULL, NULL);
        if (!(ret > 0 && FD_ISSET(fd, &fds))) continue;

        // Get udev device.
        struct udev_device *dev = udev_monitor_receive_device(mon);
        if (!dev) continue;

        // Once an event is received iterate over tokens and call corresponding callbacks.
        for (struct token *t = tokens; t != NULL; t = t->next) {
            switch (t->type) {
            case TEXT:
                printf(t->text);
                break;

            case ESCAPE:
                for (int i = 0; i < sizeof(rules) / sizeof(Rule); i++) {
                    Rule rule = rules[i];

                    if (rule.escape == t->escape) {
                        (*rule.callback)(t->escape, dev);
                    } else {
                        printf(t->text);
                    }
                }
                break;
            }
        }
    }

    udev_monitor_unref(mon);
    udev_unref(udev);

    while (tokens != NULL) {
        struct token *next = tokens->next;
        free(tokens);
        tokens = next;
    }

    return 0;
}

void print_device_subsystem(const char escape, struct udev_device *dev) {
    printf(udev_device_get_subsystem(dev));
}
