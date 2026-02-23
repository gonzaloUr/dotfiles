#define _POSIX_C_SOURCE 200809L

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

                // Create memstream to buffer the printing of the whole record so it happens at once.
                char *buffer = NULL;
                size_t size = 0;
                FILE *memstream = open_memstream(&buffer, &size);

                fprintf(memstream, "subsystem");
                fprintf(memstream, FS);
                fprintf(memstream, "\"%s\"", udev_device_get_subsystem(dev));
                fprintf(memstream, FS);
                fprintf(memstream, "devtype");
                fprintf(memstream, FS);
                fprintf(memstream, "\"%s\"", udev_device_get_devtype(dev));
                fprintf(memstream, FS);
                fprintf(memstream, "devpath");
                fprintf(memstream, FS);
                fprintf(memstream, "\"%s\"", udev_device_get_devpath(dev));
                fprintf(memstream, FS);
                fprintf(memstream, "syspath");
                fprintf(memstream, FS);
                fprintf(memstream, "\"%s\"", udev_device_get_syspath(dev));
                fprintf(memstream, FS);
                fprintf(memstream, "sysname");
                fprintf(memstream, FS);
                fprintf(memstream, "\"%s\"", udev_device_get_sysname(dev));
                fprintf(memstream, FS);
                fprintf(memstream, "sysnum");
                fprintf(memstream, FS);
                fprintf(memstream, "\"%s\"", udev_device_get_sysnum(dev));
                fprintf(memstream, FS);
                fprintf(memstream, "devnode");
                fprintf(memstream, FS);
                fprintf(memstream, "\"%s\"", udev_device_get_devnode(dev));
                fprintf(memstream, FS);
                fprintf(memstream, "initialized");
                fprintf(memstream, FS);
                fprintf(memstream, "%d", udev_device_get_is_initialized(dev));
                fprintf(memstream, FS);
                fprintf(memstream, "driver");
                fprintf(memstream, FS);
                fprintf(memstream, "\"%s\"", udev_device_get_driver(dev));
                fprintf(memstream, FS);

                dev_t devnum = udev_device_get_devnum(dev);
                fprintf(memstream, "devnum");
                fprintf(memstream, FS);
                fprintf(memstream, "\"%d-%d\"", major(devnum), minor(devnum));
                fprintf(memstream, FS);
                fprintf(memstream, "action");
                fprintf(memstream, FS);
                fprintf(memstream, "\"%s\"", udev_device_get_action(dev));
                fprintf(memstream, FS);
                fprintf(memstream, "seqnum");
                fprintf(memstream, FS);
                fprintf(memstream, "%llu", udev_device_get_seqnum(dev));
                fprintf(memstream, FS);
                fprintf(memstream, "usec_since_initialized");
                fprintf(memstream, FS);
                fprintf(memstream, "%llu", udev_device_get_usec_since_initialized(dev));
                fprintf(memstream, FS);

                // For printing devlinks, properties, tags and sysfs attrs.
                struct udev_list_entry *list, *entry, *next;
                unsigned int idx, len;

                // Print all devlinks.
                list = udev_device_get_devlinks_list_entry(dev);

                len = 0;
                udev_list_entry_foreach(entry, list) {
                    len++;
                }

                fprintf(memstream, "len_devlinks");
                fprintf(memstream, FS);
                fprintf(memstream, "%d", len);
                fprintf(memstream, FS);

                idx = 0;
                udev_list_entry_foreach(entry, list) {
                    fprintf(memstream, "devlink_%d", idx);
                    fprintf(memstream, FS);
                    fprint_field(memstream, udev_list_entry_get_name(entry));
                    fprintf(memstream, FS);
                    idx++;
                }

                // Print all properties.
                list = udev_device_get_properties_list_entry(dev);

                len = 0;
                udev_list_entry_foreach(entry, list) {
                    len++;
                }

                fprintf(memstream, "len_properties");
                fprintf(memstream, FS);
                fprintf(memstream, "%d", len);
                fprintf(memstream, FS);

                idx = 0;
                udev_list_entry_foreach(entry, list) {
                    fprintf(memstream, "property_name_%d", idx);
                    fprintf(memstream, FS);
                    fprint_field(memstream, udev_list_entry_get_name(entry));
                    fprintf(memstream, FS);
                    fprintf(memstream, "property_value_%d", idx);
                    fprintf(memstream, FS);
                    fprint_field(memstream, udev_list_entry_get_value(entry));
                    fprintf(memstream, FS);
                    idx++;
                }

                // Print all tags.
                list = udev_device_get_tags_list_entry(dev);

                len = 0;
                udev_list_entry_foreach(entry, list) {
                    len++;
                }

                fprintf(memstream, "len_tags");
                fprintf(memstream, FS);
                fprintf(memstream, "%d", len);
                fprintf(memstream, FS);

                idx = 0;
                udev_list_entry_foreach(entry, list) {
                    fprintf(memstream, "tag_%d", idx);
                    fprintf(memstream, FS);
                    fprint_field(memstream, udev_list_entry_get_name(entry));
                    fprintf(memstream, FS);
                    idx++;
                }

                // Print all current tags.
                list = udev_device_get_current_tags_list_entry(dev);

                len = 0;
                udev_list_entry_foreach(entry, list) {
                    len++;
                }

                fprintf(memstream, "len_current_tags");
                fprintf(memstream, FS);
                fprintf(memstream, "%d", len);
                fprintf(memstream, FS);

                idx = 0;
                udev_list_entry_foreach(entry, list) {
                    fprintf(memstream, "current_tag_%d", idx);
                    fprintf(memstream, FS);
                    fprint_field(memstream, udev_list_entry_get_name(entry));
                    fprintf(memstream, FS);
                    idx++;
                }

                // Print all sysfs attributes.
                list = udev_device_get_sysattr_list_entry(dev);

                len = 0;
                udev_list_entry_foreach(entry, list) {
                    len++;
                }

                fprintf(memstream, "len_sysattrs");
                fprintf(memstream, FS);
                fprintf(memstream, "%d", len);
                fprintf(memstream, FS);

                idx = 0;
                for (entry = list; entry != NULL; entry = next) {
                    const char *name = udev_list_entry_get_name(entry);
                    const char *value = udev_device_get_sysattr_value(dev, name);

                    fprintf(memstream, "sysattr_name_%d", idx);
                    fprintf(memstream, FS);
                    fprint_field(memstream, name);
                    fprintf(memstream, FS);
                    fprintf(memstream, "sysattr_value_%d", idx);
                    fprintf(memstream, FS);
                    fprint_field(memstream, value);

                    next = udev_list_entry_get_next(entry);
                    if (next != NULL) fprintf(memstream, FS);

                    idx++;
                }

                fprintf(memstream, RS);
                fclose(memstream);

                if (buffer) {
                    fputs(buffer, stdout);
                    fflush(stdout);
                    free(buffer);
                }

                udev_device_unref(dev);
            }
        }
    }

    udev_monitor_unref(mon);
    udev_unref(udev);

    return 0;
}
