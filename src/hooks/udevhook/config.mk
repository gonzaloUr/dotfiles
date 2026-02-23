# paths
PREFIX = /usr/local

# includes and libs
INCS =
LIBS = -ludev

# flags
CFLAGS = -std=c23 -pedantic -Wall -Os $(INCS)
LDFLAGS = $(LIBS)

# complier
CC = cc
