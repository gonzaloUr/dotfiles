# complier and tools.
CC = cc
YACC = bison -d
LEX = flex

# instalation prefix.
PREFIX = /usr/local

# includes and libs per binary.
INCS_PAHOOK =
LIBS_PAHOOK = -lpulse

INCS_UDEVHOOK =
LIBS_UDEVHOOK= -ludev

INCS_PARSER=
LIBS_PARSER=-ll

# flags per binary.
CFLAGS_PAHOOK = -std=c23 -pedantic -Wall -Os $(INCS_PAHOOK)
LDFLAGS_PAHOOK = $(LIBS_PAHOOK)

CFLAGS_UDEVHOOK = -std=c23 -pedantic -Wall -Os $(INCS_UDEVHOOK)
LDFLAGS_UDEVHOOK = $(LIBS_UDEVHOOK)

CFLAGS_PARSER = -Os -D_POSIX_C_SOURCE=200809L $(INCS_PARSER)
LDFLAGS_PARSER = $(LIBS_PARSER)
