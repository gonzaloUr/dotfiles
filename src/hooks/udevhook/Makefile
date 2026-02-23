include config.mk

all: clean udevhook

clean:
	rm -f udevhook.o udevhook

udevhook: udevhook.o
	$(CC) $(LDFLAGS) udevhook.o -o udevhook

udevhook.o: udevhook.c
	$(CC) $(CFLAGS) -c udevhook.c -o udevhook.o
