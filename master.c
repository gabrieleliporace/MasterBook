#define _GNU_SOURCE
#include "master.h"
#include "utenti.h"
#include "nodi.h"

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

int main(int argc, char *argv[])
{ 
	int utenti,nodi,bilancio,creo_transazione,count,status,t_attesa;
    char * id_transazione;
	long Bill;
    pid_t * array_utenti,pid_user; /*Tipo integer e rappresenta l'id del processo*/
    pid_t * array_nodi,pid_nod;
    struct sigaction sa; /*Gestione dei segnali*/
    inizializzazione_valori(); /*Richiamo la funzione per le variabili globali*/
    printf("my pid: %d\n",getpid());
    bzero(&sa, sizeof(sa));
    sa.sa_handler=my_handler;
    sigaction(SIGALRM,&sa,NULL);
    sigaction(SIGUSR1,&sa,NULL);
    alarm(SO_SIM_SEC);
    
	/*
    Dichiarazione array per PID utenti e PID nodi
    */

    array_utenti=malloc(SO_USERS_NUM*sizeof(*array_utenti));
    array_nodi=malloc(SO_NODES_NUM*sizeof(*array_nodi));

    /*
     Creo con fork() i processi utente
     */

    for(utenti=0; utenti<SO_USERS_NUM;utenti++){
        switch(pid_user=fork())
          {
          case -1:
             exit(EXIT_FAILURE);
          case 0:
            /*La funzione free() dealloca il bloccco di memoria preallocato dalla malloc*/
            free(array_utenti);
			/*calcolo l'attesa*/
			t_attesa = get_attesa(SO_MAX_TRANS_GEN_NSEC,SO_MIN_TRANS_GEN_NSEC);
			/*calcolo il bilancio*/
			bilancio = get_balance(SO_BUDGET_INIT); 
			/*creo l'id della transazione*/
			/*id_transazione = id(array_utenti,pid_user);*/
			/*creo la transazione*/
			creo_transazione = all_utenti(SO_REWARD,bilancio,id_transazione);
			printf("l'id della transazione e' %s \n",id_transazione);
			usleep(t_attesa);
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
    while(i){

        /* verifico che i pid che mi ritornino siano quelli utente,
         * cosi' quando a zero termino
         */

        for(count=0; count<SO_USERS_NUM;count++){
            while(waitpid(array_utenti[count],&status,WUNTRACED | WCONTINUED)!= -1){
                 /*
                 * attendo la terminazione di tutti i processi utente
                 */ 

                /*controllo causa uscita e riduco di uno gli utenti attivi*/
                if(WIFEXITED(status)){
                    utenti--;
                }
                if(WIFSIGNALED(status)){
                    utenti--;
                }   
                printf("utenti rimasti: %d\n", utenti);

                /*alzo un sigalrm che setta i a 0*/
                raise(SIGALRM);
            }
        }
    }
    free(array_utenti);

    return 0;
}
