CFLAGS= -std=c89 -pedantic

master: master.c master.h
	gcc $(CFLAGS) master.c -o masterbook

transizioni: transizioni.c
	gcc -c $(CFLAGS) transizioni.c

main: main.c transizioni.o
	gcc $(CFLAGS) main.c transizioni.o -o main

all: master

clean:
	rm -f *.o masterbook main *~

run: all
	./masterbook
