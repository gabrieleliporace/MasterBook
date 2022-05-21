CFLAGS= -std=c89 -pedantic -g -O0

master: master.c master.h
	gcc $(CFLAGS) master.c -o masterbook

transazioni: transazioni.c
	gcc -c $(CFLAGS) transazioni.c -o test/transazioni.o

main: ./test/test-trans-main.c ./test/transazioni.o
	gcc $(CFLAGS) ./test/test-trans-main.c ./test/transazioni.o -o test/trans-main

sender: ./test/test-trans-main.c ./test/transazioni.o
	gcc $(CFLAGS) -D SENDER ./test/test-trans-main.c ./test/transazioni.o -o test/trans-sender

master_test: ./test/transazioni.o utenti.o nodi.o master.c master.h
	gcc $(CFLAGS) master.c utenti.o test/transazioni.o nodi.o -o masterbook

all: master

clean:
	rm -f *.o masterbook ./test/trans-main ./test/*.o ./test/trans-sender *~

run: all
	./masterbook
