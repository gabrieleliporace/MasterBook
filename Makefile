CFLAGS= -std=c89 -pedantic

master: master.c master.h
	gcc $(CFLAGS) master.c -o masterbook

utenti: utenti.c utenti.h
    gcc $(GFLAGS) utenti.c -o 

all: master

clean:
	rm -f *.o masterbook *~

run: all
	./masterbook
