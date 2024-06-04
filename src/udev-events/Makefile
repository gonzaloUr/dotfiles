udev-events: main.o
	cc main.o `pkg-config -libs libudev` -o udev-events

main.o: main.c
	cc -c main.c -o main.o
