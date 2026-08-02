CC = cc

DESTDIR = /usr/local

CFLAGS = -std=c23 -pedantic -Wall -Os
LDFLAGS = -lpulse -ludev
