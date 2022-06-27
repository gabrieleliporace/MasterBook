#ifndef _MASTER_H
#define _MASTER_H

#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>
#define MSG_SIZE 8192
#define TEST_ERROR    if (errno) {dprintf(2, \
					   "%s:%d: PID=%5d: Error %d (%s)\n",\
					   __FILE__,\
					   __LINE__,\
					   getpid(),\
					   errno,\
					   strerror(errno));}

/* definizione di macro*/
#define SEND -1
#define SO_BLOCK_SIZE 10
#define SO_REGISTRY_SIZE 1000

/*definizione chiavi semafori e code di messaggi*/
#define SEM_KEY 0x113522
#define MSG_KEY 0x229729
#define SHDM_UTENTI 0x040622
#define SHDM_NODI 0x070622
#define SHDM_MASTRO 0x080622
#define SHDM_REGISTRO 0x080620

/* dichiarazione di variabili globali*/
int SO_USERS_NUM, SO_NODES_NUM, SO_BUDGET_INIT, SO_REWARD;
int SO_MIN_TRANS_GEN_NSEC,SO_MAX_TRANS_GEN_NSEC,SO_RETRY,SO_TP_SIZE;
int SO_MIN_TRANS_PROC_NSEC,SO_MAX_TRANS_PROC_NSEC,SO_SIM_SEC;
int timer = 1, invio=0;

/*coda di messaggi*/

struct mesg_buffer
{
    long pid_nod_type;
    char msg_text[MSG_SIZE];
}message;

struct master
{
	unsigned long registro;
	char *mastro[SO_BLOCK_SIZE*SO_REGISTRY_SIZE];
};



int inizializzazione_valori(); /*Legge i valori dal file init e li assegna*/


#endif
