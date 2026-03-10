include config.mk

all: clean pollhook

clean:
	rm -f pollhook.o pollhook

pollhook: pollhook.o
	$(CC) $(LDFLAGS) pollhook.o -o pollhook

pollhook.o: pollhook.c
	$(CC) $(CFLAGS) -c pollhook.c -o pollhook.o
