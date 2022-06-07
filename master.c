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

int get_casual_pid(pid_t * array,int un){ 
    struct timespec spec;
    int rec,pid;
    clock_gettime(CLOCK_REALTIME,&spec);
    srand(spec.tv_nsec);
    rec = rand() % un;
    pid = array[rec];
    return pid;
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
/*inizio main*/
int main(int argc, char *argv[])
{ 
    /*dichiarazioni per processi utente*/ 
    struct mesg_buffer message;
    struct timespec spec;
    pid_t * array_utenti,pid_user;
    int utenti,bilancio,count,status;
    int sender,reciver,rec;
    long t_attesa;
    char * transaction,nod_transaction;

    /*dichiarazioni per processi nodo*/   
    pid_t * array_nodi,pid_nod;
    int nodi,casual_nod,numnodi; 

    /*dichiarazioni varie*/ 
    struct sigaction sa; /*Gestione dei segnali*/
    struct sembuf sops; /*Struct per operazioni sui semafori*/
    pid_t u;
    pid_t miopid;
    int myretry;
    int vivi;
    int msg_id;
    int sem_id;  
    sa.sa_handler=my_handler;
    bzero(&sa, sizeof(sa));
    sigaction(SIGUSR1,&sa,NULL);
    inizializzazione_valori(); /*Richiamo la funzione per le variabili globali*/
    
    /*creazioe coda di messaggi per mandare la transazione*/
    msg_id = msgget(MSG_KEY,IPC_CREAT | 0600 );
    if (msg_id == -1) {
        perror("msgget");
        return EXIT_FAILURE;
    }

    /* creazione di semaforo per la sincronizzazione dei processi */
	sem_id=semget(SEM_KEY,3,IPC_CREAT|0600);
	semctl(sem_id,0,SETVAL,0);
	semctl(sem_id,1,SETVAL,0);
	semctl(sem_id,2,SETVAL,0);

    miopid=getpid();
    sops.sem_flg=0;

    /*
    Dichiarazione array per PID utenti e PID nodi
    */

    array_utenti=malloc(SO_USERS_NUM*sizeof(*array_utenti));
    array_nodi=malloc(SO_NODES_NUM*sizeof(*array_nodi));

    /*
    creo i processi nodo con fork()
    */
    
    printf("miopid: %d\n", getpid());
    for(nodi = 0; nodi<SO_NODES_NUM;nodi++){
        switch (pid_nod=fork())
        {
        case -1:
            exit(EXIT_FAILURE);
        case 0:

		    free(array_nodi);
            alarm(SO_SIM_SEC);

            /*Semaforo per inizializzazione array_utenti*/
            sops.sem_num=1;	
            sops.sem_op=1;
            semop(sem_id,&sops,1);

            /*Blocco per sincronizzare con l'avvio del timer*/
            sops.sem_num=2;
            sops.sem_op=1;
            semop(sem_id,&sops,1);

            /*reciver coda di messaggi*/  
            if (msgrcv(msg_id, &message,MSG_SIZE, 0, 0) == -1) {
            perror("msgrcv");
            return EXIT_FAILURE;
            }
            printf("La transazione ricevuta e' %s\n", message.msg_text);

            /*passo la transazione alla transaction-pool*/



            sleep(30);

            exit(0);
        default:
			array_nodi[nodi]=pid_nod;
            break;
        }
    }

    /*
    Creazione processi utente con fork()
    */

    for(utenti=0; utenti<SO_USERS_NUM;utenti++){
        switch(pid_user=fork())
          {
          case -1:
             exit(EXIT_FAILURE);
          case 0:
            /*La funzione free() dealloca il bloccco di memoria preallocato dalla malloc*/
            alarm(SO_SIM_SEC);

            /*Semaforo per inizializzazione array_utenti*/
            sops.sem_num=0;	
            sops.sem_op=1;
            semop(sem_id,&sops,1);

            /*Blocco per sincronizzare con l'avvio del timer*/
            sops.sem_num=2;
            sops.sem_op=1;
            semop(sem_id,&sops,1);
            /*printf("Utente #%d\n",utenti);*/

            numnodi=SO_NODES_NUM;
            myretry=SO_RETRY;
            while(myretry){
            /*bilancio = get_balance(SO_BUDGET_INIT);*/

#ifdef BIL
                bilancio = 2;
#else
                bilancio = 1;
#endif
                if(bilancio >= 2){
                    myretry=SO_RETRY;

                    /*Estrazione casuale pid nodo e utente*/
                    sender = getpid();
                    casual_nod = get_casual_pid(array_nodi,numnodi);
                    reciver = get_casual_pid(array_utenti,utenti);

                    /*creo la transazione*/
                    transaction = creazione_transazione(SO_REWARD,bilancio,reciver,sender);
                    /*printf("la transazione e' %s \n",transaction);*/

                    /*coda di messaggi*/
                    message.pid_nod_type = casual_nod;
                    memset(&(message.msg_text), 0, MSG_SIZE * sizeof(char));
                    (void)strcpy(message.msg_text,transaction);
                    if (msgsnd(msg_id, &message, sizeof(long) + (strlen(message.msg_text) * sizeof(char)) + 1, 0) == -1) {
                    perror("msgsnd");
                    return EXIT_FAILURE;
                    }
                    printf("il messaggio e' %s\n",message.msg_text);

                    /*attesa in nanosecondi*/
                    /*ts.tv_sec = 0;
                    ts.tv_nsec = get_attesa(SO_MAX_TRANS_GEN_NSEC,SO_MIN_TRANS_GEN_NSEC);*/
                    t_attesa = get_attesa(SO_MAX_TRANS_GEN_NSEC,SO_MIN_TRANS_GEN_NSEC);
                    nanosleep((const struct timespec[]){{0,t_attesa}}, NULL);
                    printf("Attesa in nanosec di %ld\n",t_attesa);
                    /*nanosleep(ts.tv_sec,ts.tv_nsec);*/
                
                sleep(1);
                }else if(bilancio < 2){
                    myretry--;
                    sleep(1);
                }
            }
			exit(0);
            break;

            default:
                /*inserisce nell'array utenti il pid del processo figlio*/
                array_utenti[utenti]=pid_user;

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

    sigaction(SIGALRM,&sa,NULL);
    printf("utenti %d\n", utenti);
    vivi=utenti;

    for (count=0; count < nodi; count++){
        printf("nodo: %d\n", array_nodi[count]);
    }

    for (count=0; count < utenti; count++){
        printf("utente: %d\n", array_utenti[count]);
    }

    while((u=wait(&status)) != -1){
    
        for(count=0; count < utenti; count++){
            if(u==array_utenti[count]){
                vivi--;
            }
            
        }
        if(vivi == 0){
            /*Termino i nodi se gli utenti sono a zero*/
            for (count=0; count < nodi; count++){
                kill(array_nodi[count], SIGTERM);
            }
            
        }
        printf("processo terminato: %d , %d, %d , %d\n", WIFEXITED(status), WIFSIGNALED(status), u, vivi);
    }
    
    sigaction(SIGTERM,&sa,NULL);
    printf("nodi %d\n", nodi);
    printf("miopid %d\n", getpid());

    printf("ciao ciao \n");
	/*Rimuovo il semaforo*/
	semctl(sem_id,0,IPC_RMID);

    msgctl(msg_id, IPC_RMID, NULL); 
    free(array_utenti);
    free(array_nodi);

    return 0;
}