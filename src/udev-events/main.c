#include <stdio.h>
#include <stdlib.h>
#include <libudev.h>

void example1() {
    struct udev *udev = udev_new();

    if (!udev) {
        exit(1);
    }

    struct udev_monitor *mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "backlight", NULL);
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
                
                struct udev_list_entry *attrs = udev_device_get_sysattr_list_entry(device);
                struct udev_list_entry *entry;

                udev_list_entry_foreach(entry, attrs) {
                    const char *key = udev_list_entry_get_name(entry);
                    const char *value = udev_device_get_sysattr_value(device, key);
                    printf("Attr: %s = %s\n", key, value);
                }

                udev_device_unref(device);
            }
        }
    }
}

void example2() {
    struct udev *udev = udev_new();

    if (!udev) {
        fprintf(stderr, "new udev");
        exit(1);
    }

    struct udev_enumerate *enumerate = udev_enumerate_new(udev);

    if (!enumerate) {
        udev_unref(udev);
        fprintf(stderr, "new enumerate failed");
        exit(1);
    }

    udev_enumerate_add_match_subsystem(enumerate, "backlight");

    if (udev_enumerate_scan_devices(enumerate) < 0) {
        udev_enumerate_unref(enumerate);
        udev_unref(udev);
        fprintf(stderr, "scan devices failed");
        exit(1);
    }

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);

    if (!devices) {
        udev_enumerate_unref(enumerate);
        udev_unref(udev);
        fprintf(stderr, "get list entry failed");
        exit(1);
    }

    struct udev_list_entry *entry;
    struct udev_device *device = NULL;

    udev_list_entry_foreach(entry, devices) {
        const char *name = udev_list_entry_get_name(entry);
        struct udev_device *device = udev_device_new_from_syspath(udev, name);

        if (!device) {
            udev_device_unref(device);
            udev_enumerate_unref(enumerate);
            udev_unref(udev);
            exit(1);
        }

        struct udev_list_entry *attrs = udev_device_get_sysattr_list_entry(device);

        udev_list_entry_foreach(entry, attrs) {
            const char *key = udev_list_entry_get_name(entry);
            const char *value = udev_device_get_sysattr_value(device, key);
            printf("Attr: %s = %s\n", key, value);
        }

        udev_device_unref(device);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);
}

int main() {
    example2();

    return 0;
}
