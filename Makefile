CFLAGS= -std=c89 -pedantic

master: master.c master.h
	gcc $(CFLAGS) master.c -o masterbook

transazioni: transazioni.c
	gcc -c $(CFLAGS) transazioni.c -o test/transazioni.o

main: ./test/main.c ./test/transazioni.o
	gcc $(CFLAGS) ./test/main.c ./test/transazioni.o -o test/main

sender: ./test/main.c ./test/transazioni.o
	gcc $(CFLAGS) -D SENDER ./test/main.c ./test/transazioni.o -o test/sender

all: master

clean:
	rm -f *.o masterbook ./test/main ./test/*.o ./test/sender *~

run: all
	./masterbook
