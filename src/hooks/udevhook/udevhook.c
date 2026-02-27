#include "udevhook.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

char* escape_quotes(const char *str) {
    if (!str) return strdup("");

    size_t len = 0;

    for (const char *c = str; *c; c++)
        len += (*c == '"') ? 2 : 1;

    char *ret = malloc(len + 1);
    char *dst = ret;

    for (const char *c = str; *c; c++) {
        if (*c == '"') {
            *dst++ = '"';
            *dst++ = '"';
        } else {
            *dst++ = *c;
        }
    }

    *dst = '\0';

    return ret;
}

void fprint_field(FILE *file, const char *str) {
    char *str_escaped = escape_quotes(str);
    fprintf(file, "\"%s\"", str_escaped);
    free(str_escaped);
}

int main() {
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
        // fd set for select.
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        // Wait for an event.
        int ret = select(fd + 1, &fds, NULL, NULL, NULL);

        if (ret > 0 && FD_ISSET(fd, &fds)) {
            struct udev_device *dev = udev_monitor_receive_device(mon);
            if (dev) {

                printf("subsystem");
                printf(FS);
                printf("\"%s\"", udev_device_get_subsystem(dev));
                printf(FS);
                printf("devtype");
                printf(FS);
                printf("\"%s\"", udev_device_get_devtype(dev));
                printf(FS);
                printf("devpath");
                printf(FS);
                printf("\"%s\"", udev_device_get_devpath(dev));
                printf(FS);
                printf("syspath");
                printf(FS);
                printf("\"%s\"", udev_device_get_syspath(dev));
                printf(FS);
                printf("sysname");
                printf(FS);
                printf("\"%s\"", udev_device_get_sysname(dev));
                printf(FS);
                printf("sysnum");
                printf(FS);
                printf("\"%s\"", udev_device_get_sysnum(dev));
                printf(FS);
                printf("devnode");
                printf(FS);
                printf("\"%s\"", udev_device_get_devnode(dev));
                printf(FS);
                printf("initialized");
                printf(FS);
                printf("%d", udev_device_get_is_initialized(dev));
                printf(FS);
                printf("driver");
                printf(FS);
                printf("\"%s\"", udev_device_get_driver(dev));
                printf(FS);

                dev_t devnum = udev_device_get_devnum(dev);
                printf("devnum");
                printf(FS);
                printf("\"%d-%d\"", major(devnum), minor(devnum));
                printf(FS);
                printf("action");
                printf(FS);
                printf("\"%s\"", udev_device_get_action(dev));
                printf(FS);
                printf("seqnum");
                printf(FS);
                printf("%llu", udev_device_get_seqnum(dev));
                printf(FS);
                printf("usec_since_initialized");
                printf(FS);
                printf("%llu", udev_device_get_usec_since_initialized(dev));
                printf(FS);

                // For printing devlinks, properties, tags and sysfs attrs.
                struct udev_list_entry *list, *entry, *next;
                unsigned int idx, len;

                // Print all devlinks.
                list = udev_device_get_devlinks_list_entry(dev);

                len = 0;
                udev_list_entry_foreach(entry, list) {
                    len++;
                }

                printf("len_devlinks");
                printf(FS);
                printf("%d", len);
                printf(FS);

                idx = 0;
                udev_list_entry_foreach(entry, list) {
                    printf("devlink_%d", idx);
                    printf(FS);
                    fprint_field(stdout, udev_list_entry_get_name(entry));
                    printf(FS);
                    idx++;
                }

                // Print all properties.
                list = udev_device_get_properties_list_entry(dev);

                len = 0;
                udev_list_entry_foreach(entry, list) {
                    len++;
                }

                printf("len_properties");
                printf(FS);
                printf("%d", len);
                printf(FS);

                idx = 0;
                udev_list_entry_foreach(entry, list) {
                    printf("property_name_%d", idx);
                    printf(FS);
                    fprint_field(stdout, udev_list_entry_get_name(entry));
                    printf(FS);
                    printf("property_value_%d", idx);
                    printf(FS);
                    fprint_field(stdout, udev_list_entry_get_value(entry));
                    printf(FS);
                    idx++;
                }

                // Print all tags.
                list = udev_device_get_tags_list_entry(dev);

                len = 0;
                udev_list_entry_foreach(entry, list) {
                    len++;
                }

                printf("len_tags");
                printf(FS);
                printf("%d", len);
                printf(FS);

                idx = 0;
                udev_list_entry_foreach(entry, list) {
                    printf("tag_%d", idx);
                    printf(FS);
                    fprint_field(stdout, udev_list_entry_get_name(entry));
                    printf(FS);
                    idx++;
                }

                // Print all current tags.
                list = udev_device_get_current_tags_list_entry(dev);

                len = 0;
                udev_list_entry_foreach(entry, list) {
                    len++;
                }

                printf("len_current_tags");
                printf(FS);
                printf("%d", len);
                printf(FS);

                idx = 0;
                udev_list_entry_foreach(entry, list) {
                    printf("current_tag_%d", idx);
                    printf(FS);
                    fprint_field(stdout, udev_list_entry_get_name(entry));
                    printf(FS);
                    idx++;
                }

                // Print all sysfs attributes.
                list = udev_device_get_sysattr_list_entry(dev);

                len = 0;
                udev_list_entry_foreach(entry, list) {
                    len++;
                }

                printf("len_sysattrs");
                printf(FS);
                printf("%d", len);
                printf(FS);

                idx = 0;
                for (entry = list; entry != NULL; entry = next) {
                    const char *name = udev_list_entry_get_name(entry);
                    const char *value = udev_device_get_sysattr_value(dev, name);

                    printf("sysattr_name_%d", idx);
                    printf(FS);
                    fprint_field(stdout, name);
                    printf(FS);
                    printf("sysattr_value_%d", idx);
                    printf(FS);
                    fprint_field(stdout, value);

                    next = udev_list_entry_get_next(entry);
                    if (next != NULL) printf(FS);

                    idx++;
                }

                printf(RS);
                fflush(stdout);
                udev_device_unref(dev);
            }
        }
    }

    udev_monitor_unref(mon);
    udev_unref(udev);

    return 0;
}
