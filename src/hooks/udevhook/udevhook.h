#ifndef UDEVHOOK_H
#define UDEVHOOK_H

#include <libudev.h>
#include <stdio.h>

#define FS (",")
#define RS ("\n")

char* escape_quotes(const char *str);
void fprint_field(FILE *file, const char *str);

#endif
