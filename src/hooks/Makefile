include config.mk

.PHONY: all clean install uninstall

all: hooks

install: all
	mkdir -p $(PREFIX)/bin
	cp hooks $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/hooks

clean:
	rm -f hooks.o hooks
	rm -f components/*.o

hooks: hooks.o components/pahook.o
	$(CC) $(LDFLAGS) hooks.o components/*.o -o hooks

hooks.o: hooks.c hooks.h config.h
	$(CC) $(CFLAGS) -c hooks.c -o hooks.o

config.h: config.def.h
	cp config.def.h config.h

components/pahook.o: components/pahook.c components/pahook.h
	$(CC) $(CFLAGS) -c components/pahook.c -o components/pahook.o
