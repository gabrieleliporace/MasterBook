#define _GNU_SOURCE
#include "master.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>	
#include <time.h>	

int  inizializzazione_valori()
{
	/*Creazione degli stream*/
	FILE* masterbook_init;

	masterbook_init=fopen("./init.txt","r");

	/*Lettura da file dei parametri*/

	fscanf(masterbook_init,"%d",&SO_USERS_NUM);
	fscanf(masterbook_init,"%d",&SO_NODES_NUM);
	fscanf(masterbook_init,"%d",&SO_BUDGET_INIT);
	fscanf(masterbook_init,"%d",&SO_REWARD);
	fscanf(masterbook_init,"%d",&SO_MIN_TRANS_GEN_NSEC);
	fscanf(masterbook_init,"%d",&SO_MAX_TRANS_GEN_NSEC);
	fscanf(masterbook_init,"%d",&SO_RETRY);
	fscanf(masterbook_init,"%d",&SO_TP_SIZE);
	fscanf(masterbook_init,"%d",&SO_MIN_TRANS_PROC_NSEC);
	fscanf(masterbook_init,"%d",&SO_MAX_TRANS_PROC_NSEC);
	fscanf(masterbook_init,"%d",&SO_SIM_SEC);
}

int main()
{
    inizializzazione_valori();
    printf("%i\n",SO_USERS_NUM);
    return 0;
}
