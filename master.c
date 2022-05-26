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

        case SIGTERM:
            break;
    }
}

/*int get_casual_pid(pid_t * array){
    int r,reciver;
    struct timespec spec; 
    size_t u;
    clock_gettime(CLOCK_REALTIME,&spec);
    srand(spec.tv_nsec);
    u = sizeof(array)/sizeof(array[0]);
    r = rand() % u;
    reciver = array[r];
    return reciver;
}  */

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
/*inizio main*/
int main(int argc, char *argv[])
{ 
	int utenti,nodi,bilancio,count,status,t_attesa;
    int sender,reciver,rec;
    size_t u;
    int msg_id;
    int sem_id;  
    char * transaction;
	long Bill;
    pid_t * array_utenti,pid_user; /*Tipo integer e rappresenta l'id del processo*/
    pid_t * array_nodi,pid_nod;
    struct timespec spec;
    struct sigaction sa; /*Gestione dei segnali*/
    struct sembuf sops; /*Struct per operazioni sui semafori*/
    inizializzazione_valori(); /*Richiamo la funzione per le variabili globali*/
    bzero(&sa, sizeof(sa));
    sa.sa_handler=my_handler;
    sigaction(SIGALRM,&sa,NULL);
    sigaction(SIGUSR1,&sa,NULL);

    /*creazioe coda di messaggi per mandare la transazione*/
    msg_id = msgget(MSG_KEY,IPC_CREAT | 0600 );
    /* creazione di semaforo per la sincronizzazione dei processi */
	sem_id=semget(SEM_KEY,3,IPC_CREAT|0600);
	semctl(sem_id,0,SETVAL,0);
	semctl(sem_id,1,SETVAL,0);
	semctl(sem_id,2,SETVAL,0);

    sops.sem_flg=0;

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

            /*Semaforo per inizializzazione array_utenti*/
            sops.sem_num=0;	
            sops.sem_op=1;
            semop(sem_id,&sops,1);

            /*Blocco per sincronizzare con l'avvio del timer*/
            sops.sem_num=2;
            sops.sem_op=1;
            semop(sem_id,&sops,1);
            
            printf("Utente #%d\n",utenti);

            /*creazione transazione*/
            bilancio = get_balance(SO_BUDGET_INIT);
            if(bilancio >= 2){
                sender = getpid();
                /*u = sizeof(array_utenti)/sizeof(array_utenti[0]);*/
                clock_gettime(CLOCK_REALTIME,&spec);
                srand(spec.tv_nsec);
                rec = rand() % SO_USERS_NUM-1;
                reciver = &array_utenti[rec];
                printf("il reciver e' %d\n",reciver);
                /*transaction = creazione_transazione(SO_REWARD,bilancio,reciver,sender);
                printf("la transazione e' %s \n",transaction);*/
            }


			exit(0);
            if (utenti == 10 || utenti==19){
                sleep(5);
            }else{
                sleep(35);
            }
            exit(0);
            break;

            default:
                /*inserisce nell'array utenti il pid del processo figlio*/
                array_utenti[utenti]=pid_user;
                break;
        }
    }

    /*
    creo i processi nodo con fork()
    */
    for(nodi = 0; nodi<=SO_NODES_NUM;nodi++){
        switch (pid_nod=fork())
        {
        case -1:
            exit(EXIT_FAILURE);
        case 0:

		    free(array_nodi);

            /*Semaforo per inizializzazione array_utenti*/
            sops.sem_num=1;	
            sops.sem_op=1;
            semop(sem_id,&sops,1);

            /*Blocco per sincronizzare con l'avvio del timer*/
            sops.sem_num=2;
            sops.sem_op=1;
            semop(sem_id,&sops,1);

            /*printf("Nodo #%d\n",nodi);*/
            sleep(30);
            exit(0);

        default:
			array_nodi[nodi]=pid_nod;
             break;
        }
    }

    /*Lascio partire gli utenti*/
	sops.sem_num=0;	
	sops.sem_op=-utenti;
	semop(sem_id,&sops,1);
    
    /*Lascio partire i nodi*/
	sops.sem_num=1;	
	sops.sem_op=-nodi;
	semop(sem_id,&sops,1);

    /*Avvio la simulazione e il timer*/
	sops.sem_num=2;	
	sops.sem_op=-(utenti+nodi);
	semop(sem_id,&sops,1);

    alarm(SO_SIM_SEC);

    while(i){

        
        /* verifico che i pid che mi ritornino siano quelli utente,
         * cosi' quando a zero termino
         */

        /*for(count=0; count<SO_USERS_NUM;count++){*/
        while(waitpid(array_utenti[utenti],&status,WUNTRACED | WCONTINUED)!= -1){
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
            /*printf("utenti rimasti: %d\n", utenti);*/

            /*alzo un sigalrm che setta i a 0*/
            raise(SIGALRM);
        }
        /*}*/

    }

    sigaction(SIGTERM,&sa,NULL);

    /*Termino tutti gli utenti rimasti*/
    for (count=0; count < utenti; count++){
        kill(array_utenti[utenti], SIGTERM);
    }
    
    /*Termino tutti i nodi rimasti*/
    for (count=0; count < nodi; count++){
        kill(array_nodi[nodi], SIGTERM);
    }
	/*Rimuovo il semaforo*/
	semctl(sem_id,0,IPC_RMID);

    free(array_utenti);
    free(array_nodi);

    return 0;
}