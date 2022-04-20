#define _GNU_SOURCE
#include "master.h"

void my_handler(int signum)
{
    switch(signum){
    
        case SIGALRM:
            i=0;
            break;

        case SIGUSR1:
            break;
    }
}

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

int main()
{
    int utenti, nodi;
    struct sigaction sa;
    inizializzazione_valori();
    bzero(&sa, sizeof(sa));
    sa.sa_handler=my_handler;
    sigaction(SIGALRM,&sa,NULL);
    sigaction(SIGUSR1,&sa,NULL);
    alarm(SO_SIM_SEC);

    /*
     * eseguo fork per create i processi utente
     */
    for(utenti=1; utenti<=SO_USERS_NUM;utenti++){
        if(!fork()){
            printf("Utente #%d\n",utenti);
            exit(0);
        }
    }
    
    /*
     * eseguo fork per create i processi nodo
     */
    for(nodi=1; nodi<=SO_NODES_NUM;nodi++){
        if(!fork()){
            printf("Nodo #%d\n",nodi);
            exit(0);
        }
    }

    while(i){
        /*while(wait(NULL)!= -1){
             *
             * attendo la terminazione di tutti i figli
             * 
        }*/
    }
    return 0;
}
