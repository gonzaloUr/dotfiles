# complier.
CC = cc

# instalation prefix.
PREFIX = /usr/local

# includes and libs.
INCS =
LIBS = -lpulse -ludev

# flags
CFLAGS = -std=c23 -pedantic -Wall -Os $(INCS)
LDFLAGS = $(LIBS)
