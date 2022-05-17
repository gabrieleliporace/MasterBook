#ifndef _NODI_H
#define _NODI_H

#define _GNU_SOURCE 
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

/*definizione funzioni*/
/*crea la transazione con la somma dei reward*/
char * reward_transaction(long somma_reward,pid_t nodo_corrente);
/*creazione della transaction pool e blocco candidato*/
int trans_block(char * reward_transaction,int block_size,int mp_size);

#endif
