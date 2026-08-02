include config.mk

.PHONY: all install clean

all: stbar

install: all
	mkdir -p $(DESTDIR)/bin
	cp stbar $(DESTDIR)/bin

uninstall:
	rm -rf $(DESTDIR)/bin/stbar

clean:
	rm -f *.o
	rm -f stbar

stbar: stbar.o components/pulseaudio.o
	$(CC) $(LDFLAGS) stbar.o components/pulseaudio.o -o stbar

stbar.o: stbar.c stbar.h config.h
	$(CC) $(CFLAGS) -c stbar.c -o stbar.o

config.h: config.def.h
	cp config.def.h config.h

components/pulseaudio.o: components/pulseaudio.c components/pulseaudio.h
	$(CC) $(CFLAGS) -c components/pulseaudio.c -o components/pulseaudio.o
