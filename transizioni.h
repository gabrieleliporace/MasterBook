#ifndef _TRANSIZIONI_H
#define _TRANSIZIONI_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
/**
 * DEFINIZIONE TIPO TRANSIZIONE*
                              **/

char * string;

typedef struct transazione{

    int timestamp;
    int sender;
    int receiver;
    int quantity;
    int reward;

} transazione;



/**
 *DEFINIZIONE FUNZIONI*
                     **/

char * generaIdentificatore(long sender,long receiver); 




#endif
