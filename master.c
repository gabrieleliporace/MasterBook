#define _GNU_SOURCE
#include "master.h"
#include "utenti.h"
#include "nodi.h"

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
	int utenti,nodi,valore;
	long Bill;
	pid_t * array_utenti,pid_user; /*Tipo integer e rappresenta l'id del processo*/
	pid_t * array_nodi,pid_nod;
	struct sigaction sa; /*Gestione dei segnali*/
	inizializzazione_valori(); /*Richiamo la funzione per le variabili globali*/
	printf("my pid: %d\n",getpid());
	alarm(SO_SIM_SEC);

	/*
	Dichiarazione array per PID utenti e PID nodi
	*/

	array_utenti=malloc(SO_USERS_NUM*sizeof(*array_utenti));
    array_nodi=malloc(SO_NODES_NUM*sizeof(*array_nodi));

	/*
	Creo con fork() i processi utente
	*/

	for(utenti=1;utenti<=SO_USERS_NUM;utenti++){
		switch (pid_user=fork())
		{
		case -1:
		     exit(EXIT_FAILURE);
	    case 0:
		     free(array_utenti);/*La funzione free() dealloca il bloccco di memoria preallocato dalla malloc*/
			 
			 /*#ifdef BILL
		     Bill = bilancio(0,2);
			 #else
			 Bill = bilancio(0,1);
			 #endif*/
             Bill = 100;
             valore = val_transazione(Bill,1);
			 printf("il valore della transazione e' %d\n",valore);

			 exit(0);
			 break;
		
		default:
		     array_utenti[utenti]=pid_user; /*inserisce nell'array utenti il pid del processo figlio*/
			break;
		}
	}

	/*
	creo i processi nodo con fork()
	*/
    for(nodi = 1; nodi<=SO_NODES_NUM;nodi++){
		switch (pid_nod=fork())
		{
		case -1:
		    exit(EXIT_FAILURE);
	    case 0:
		    free(array_nodi);
			/*body*/
			exit(0);
			break;
		
		default:
		    array_nodi[nodi]=pid_nod;
			break;
		}
	}
	
	return 0;
}
