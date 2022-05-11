CFLAGS= -std=c89 -pedantic

master: master.c master.h
	gcc $(CFLAGS) master.c -o masterbook

utenti: utenti.c utenti.h
    gcc $(GFLAGS) utenti.c -o 

bilancio: master.c master.o
    gcc $(GFLAGS) -D master.c master.o  -o test-bilancio

all: master

clean:
	rm -f *.o masterbook *~

run: all
	./masterbook
