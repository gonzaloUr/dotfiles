build: main.o
	gcc -o status main.o `pkg-config --libs dbus-1`

main.o: main.c
	gcc -c -o main.o main.c `pkg-config --cflags dbus-1`
