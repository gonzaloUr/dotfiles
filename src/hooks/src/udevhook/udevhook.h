#ifndef UDEVHOOK_H
#define UDEVHOOK_H

#include <libudev.h>
#include <stdio.h>

typedef void (*callback_fn)(
    FILE *out,
    struct udev_device *dev
);

typedef Rule struct {
    const char *escape;
    const char *text;
    const callback_fn *callback;
} Rule;

#define FS (",")
#define RS ("\n")

char* escape_quotes(const char *str);
void fprint_field(FILE *file, const char *str);

#endif
