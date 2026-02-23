include config.mk

all: clean pahook

clean:
	rm -f pahook.o pahook

pahook: pahook.o
	$(CC) $(LDFLAGS) pahook.o -o pahook

pahook.o: pahook.c
	$(CC) $(CFLAGS) -c pahook.c -o pahook.o
