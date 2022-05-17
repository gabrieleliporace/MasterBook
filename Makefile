CFLAGS=-std=c89 -pedantic -g -O0

master: master.c master.h
	gcc -c $(CFLAGS) master.c -o masterbook	
	
nodi: nodi.c nodi.h
	gcc -c $(CFLAGS) nodi.c -o nodi	

utenti: utenti.c utenti.h
	gcc -c $(CFLAGS)utenti.c -o utenti

transazioni: transazioni.c
	gcc -c $(CFLAGS) transazioni.c -o test/transazioni.o

all: master

clean:
	rm -f *.o masterbook *~

run: all
	./masterbook
