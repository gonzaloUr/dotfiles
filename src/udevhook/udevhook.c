#include "udevhook.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

int main(void) {
    struct udev *udev;
    struct udev_monitor *mon;
    int fd;

    // Create udev object
    udev = udev_new();
    if (!udev) {
        fprintf(stderr, "Cannot create udev context.\n");
        return 1;
    }

    // Set up a monitor to listen for all events
    mon = udev_monitor_new_from_netlink(udev, "udev");
    if (!mon) {
        fprintf(stderr, "Cannot create udev monitor.\n");
        udev_unref(udev);
        return 1;
    }

    // Start receiving events
    if (udev_monitor_enable_receiving(mon) < 0) {
        fprintf(stderr, "Cannot enable udev monitor.\n");
        udev_monitor_unref(mon);
        udev_unref(udev);
        return 1;
    }

    fd = udev_monitor_get_fd(mon);
    printf("Listening for udev events...\n");

    while (1) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        // Wait for an event
        int ret = select(fd + 1, &fds, NULL, NULL, NULL);
        if (ret > 0 && FD_ISSET(fd, &fds)) {
            struct udev_device *dev = udev_monitor_receive_device(mon);
            if (dev) {
                printf("Action: %s\n", udev_device_get_action(dev));
                printf("Subsystem: %s\n", udev_device_get_subsystem(dev));
                printf("Devpath: %s\n", udev_device_get_devpath(dev));
                printf("Devnode: %s\n", udev_device_get_devnode(dev));
                printf("---------------------------\n");
                udev_device_unref(dev);
            }
        }
    }

    udev_monitor_unref(mon);
    udev_unref(udev);
    return 0;
}
