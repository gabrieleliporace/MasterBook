CFLAGS= -std=c89 -pedantic

master: master.c master.h
	gcc $(CFLAGS) master.c -o masterbook

all: master

clean:
	rm -f *.o masterbook *~

run: all
	./masterbook
