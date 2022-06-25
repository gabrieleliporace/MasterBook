#ifndef _TRANSIZIONI_H
#define _TRANSIZIONI_H

/*Inclusione librerie*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h> 
#include <string.h> /*Manipolazione stringhe*/
#include <stdlib.h> /*Conversione da numeri a stringhe e viceversa, anche generatori casuali*/
#include <sys/types.h> 
#include <unistd.h> 
#include <time.h> /*Manipolazione date e valori temporali*/
#include <sys/time.h> 
#include <sys/wait.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

/*
 * DEFINIZIONE TIPO TRANSIZIONE
 */

typedef struct transazione{

    int timestamp;
    int sender;
    int receiver;
    int quantity;
    int reward;

} transazione;



/*
 * DEFINIZIONE FUNZIONI
 */

/* Genera identificatore per la transazione*/
char * genera_Identificatore(long sender,long receiver);  

/* Genera la stringa completa della transazione*/
char * transaction_create(char * id,long quantity,long reward);

/* Recupera dalla stringa della transazione il valore di reward*/
long get_reward(char * transaction);

/* Recupera dalla stringa della transazione il valore quantity*/
long get_quantity(pid_t miopid, char * transaction);

long get_sender(char * transaction);

long get_receiver(char * transaction);

#endif