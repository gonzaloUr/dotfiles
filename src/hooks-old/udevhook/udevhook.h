#ifndef UDEVHOOK_H
#define UDEVHOOK_H

#include <libudev.h>

typedef void (*callback_fn)(const char escape, struct udev_device *dev);

typedef struct Rule {
    char escape;
    callback_fn callback;
} Rule;

void print_device_subsystem(const char escape, struct udev_device *dev);

#endif
