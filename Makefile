CFLAGS= -std=c89 -pedantic -g -O0

master: ./test/transazioni.o ./test/utenti.o ./test/nodi.o master.c master.h
	gcc $(CFLAGS) master.c test/utenti.o test/transazioni.o test/nodi.o -o masterbook

transazioni: transazioni.c
	gcc -c $(CFLAGS) transazioni.c -o test/transazioni.o

utenti: utenti.c
	gcc -c $(CFLAGS) utenti.c -o test/utenti.o

nodi: nodi.c
	gcc -c $(CFLAGS) nodi.c -o test/nodi.o

main: ./test/test-trans-main.c ./test/transazioni.o
	gcc $(CFLAGS) ./test/test-trans-main.c ./test/transazioni.o -o test/trans-main

sender: ./test/test-trans-main.c ./test/transazioni.o
	gcc $(CFLAGS) -D SENDER ./test/test-trans-main.c ./test/transazioni.o -o test/trans-sender

all: utenti nodi transazioni master

clean:
	rm -f *.o masterbook ./test/trans-main ./test/*.o ./test/trans-sender *~