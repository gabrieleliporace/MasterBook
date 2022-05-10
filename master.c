#define _GNU_SOURCE
#include "master.h"

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

    fclose(masterbook_init);
}

int main(int argc, char *argv[])
{ 
	int utenti,nodi;

	/*Creazione processi utente*/
    for(utenti=0;utenti<=20;utenti++){
        if (fork()==0){
	    
		exit(0);
		}
	}
	for(utenti=0;utenti<=20;utenti++)
		wait(NULL);

	/*creazione processi nodo*/
	for(nodi=1;nodi<=10;nodi++){
        if (fork()==0){
	    
		exit(0);
		}
	}
	for(nodi=1;nodi<=20;nodi++)
		wait(NULL);

    return 0;
}
