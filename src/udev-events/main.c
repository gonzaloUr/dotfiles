#include <stdio.h>
#include <libudev.h>

int main() {
    struct udev *udev = udev_new();

    if (!udev) {
        return 1;
    }

    struct udev_monitor *mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "block", NULL);
    udev_monitor_enable_receiving(mon);

    int fd = udev_monitor_get_fd(mon);

    while (1) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        if (select(fd + 1, &fds, NULL, NULL, NULL) > 0 && FD_ISSET(fd, &fds)) {
            struct udev_device *device = udev_monitor_receive_device(mon);

            if (device) {
                printf("Device event received: %s %s\n", udev_device_get_action(device), udev_device_get_devnode(device));
                udev_device_unref(device);
            }
        }
    }

    return 0;
}
