#ifndef _MASTER_H
#define _MASTER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>	
#include <time.h>	

/* definizione di macro */
#define SO_BLOCK_SIZE 10
#define SO_REGISTRY_SIZE 1000

/* definizione semaphore key */
#define SEM_KEY 0x113522

/* dichiarazione di variabili globali*/
int SO_USERS_NUM, SO_NODES_NUM, SO_BUDGET_INIT, SO_REWARD;
int SO_MIN_TRANS_GEN_NSEC,SO_MAX_TRANS_GEN_NSEC,SO_RETRY,SO_TP_SIZE;
int SO_MIN_TRANS_PROC_NSEC,SO_MAX_TRANS_PROC_NSEC,SO_SIM_SEC;
int i=1;

/*definizione funzioni*/

int inizializzazione_valori(); /*Legge i valori dal file init e li assegna*/


#endif
