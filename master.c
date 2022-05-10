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
    int utenti, nodi, status, count, a;
    pid_t * array_utenti,pid_kids;
    struct sigaction sa;
    inizializzazione_valori();
    printf("my pid: %d\n", getpid());
    bzero(&sa, sizeof(sa));
    sa.sa_handler=my_handler;
    sigaction(SIGALRM,&sa,NULL);
    sigaction(SIGUSR1,&sa,NULL);
    alarm(SO_SIM_SEC);

    /*dichiarazione array per inserire pid utenti*/
    array_utenti=malloc(SO_USERS_NUM*sizeof(*array_utenti));

    /*
     * eseguo fork per create i processi utente
     */

    for(utenti=0; utenti<SO_USERS_NUM;utenti++){
        switch(pid_kids=fork()){
            case -1:
                exit(EXIT_FAILURE);
            case 0:
                free(array_utenti);
                printf("Utente #%d\n",utenti);
                exit(0);
                break;
            default:
                array_utenti[utenti]=pid_kids;
                break;

        }
    }

    /*
     * eseguo fork per create i processi nodo
     */
    for(nodi=0; nodi<SO_NODES_NUM;nodi++){
        switch(fork()){
            case -1:
                exit(EXIT_FAILURE);
            case 0:

            printf("Nodo #%d\n",nodi);
            exit(0);

            default:
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
