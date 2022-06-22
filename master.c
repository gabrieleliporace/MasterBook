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
    struct timespec spec,ts,te;
    pid_t * array_utenti,pid_user;
    int utenti,count,status;
    int sender,receiver,rec;
    int shm_utenti,shm_nodi;
    long t_attesa,bilancio;
    char * transaction,nod_transaction;

    /*dichiarazioni per processi nodo*/   
    pid_t * array_nodi,pid_nod;
    int nodi,casual_nod,numnodi; 
    int tpi,btj,tpj;
    long som_reward,som_rew_tot; 
    int shm_mastro;
    char *mastro;
    char **block_transaction;

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
    msg_id = msgget(MSG_KEY,IPC_CREAT | 0600);
    if (msg_id == -1) {
        perror("msgget");
        return EXIT_FAILURE;
    }

    /* creazione di semaforo per la sincronizzazione dei processi */
	sem_id=semget(SEM_KEY,3,IPC_CREAT|0600);
	semctl(sem_id,0,SETVAL,0);
	semctl(sem_id,1,SETVAL,0);
	semctl(sem_id,2,SETVAL,0);

    /*Creazione memoria condivisa nodi e utenti e mastro*/
    shm_utenti=shmget(SHDM_UTENTI,SO_USERS_NUM*sizeof(*array_utenti),IPC_CREAT|0600);
    shm_nodi=shmget(SHDM_NODI,SO_NODES_NUM*sizeof(*array_nodi),IPC_CREAT|0600);
    shm_mastro=shmget(SHDM_MASTRO,sizeof(char)*SO_BLOCK_SIZE*SO_REGISTRY_SIZE, IPC_CREAT | 0600);

    /*Dichiarazione array per PID utenti e PID nodi*/
    array_utenti=shmat(shm_utenti,NULL,0);
    array_nodi=shmat(shm_nodi,NULL,0);
    mastro=shmat(shm_mastro,NULL,0);

    /*Marchio la shared memory come pronta per la rimozione, 
      viene rimossa quando tutti i processi si sono staccati*/
    shmctl(shm_utenti,IPC_RMID,NULL);
    shmctl(shm_nodi,IPC_RMID,NULL);
    shmctl(shm_mastro,IPC_RMID,NULL);

    miopid=getpid();
    sops.sem_flg=0;

    /*malloc per transaction_block*/
    block_transaction = malloc(SO_BLOCK_SIZE * sizeof(char*));

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

            alarm(SO_SIM_SEC);

            /*Semaforo per inizializzazione array_utenti*/
            sops.sem_num=1;	
            sops.sem_op=1;
            semop(sem_id,&sops,1);

            /*Blocco per sincronizzare con l'avvio del timer*/
            sops.sem_num=2;
            sops.sem_op=1;
            semop(sem_id,&sops,1);

            for(tpi = 0;tpi <= SO_TP_SIZE-1;tpi++){
                
                /*reciver coda di messaggi*/  
                msgrcv(msg_id, &message,MSG_SIZE,getpid(),0); 
                
                /*blocco transazioni*/
                block_transaction[tpi]=message.msg_text;
                
                /*Calcolo la somma dei reward e creo la transazione*/
                som_reward += take_reward(block_transaction[tpi]);
                if (tpi == SO_BLOCK_SIZE-2){
                    block_transaction[SO_BLOCK_SIZE-1] = transazione_reward(0,som_reward,getpid(),SEND);    

                    /*Simulazione elaborazione in nanosecondi*/
                    te.tv_sec = 0;
                    te.tv_nsec = get_attesa(SO_MAX_TRANS_PROC_NSEC,SO_MIN_TRANS_PROC_NSEC);
                    nanosleep(&ts,NULL);

                    /*Mando il blocco transazioni al libro mastro*/

                    /*calcolo bilancio del nodo*/
                    som_rew_tot += som_reward;
                    /*printf("Il bilancio del nodo %d e' %ld\n",getpid(),som_rew_tot);*/
                    som_reward = 1; 
                    tpi = 0;
                }                
            }
                
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
                    do{
                    receiver = get_casual_pid(array_utenti,utenti);
                    }while(receiver==0);
                    

                    /*creo la transazione*/
                    transaction = creazione_transazione(SO_REWARD,bilancio,receiver,sender);

                    /*coda di messaggi*/
                    message.pid_nod_type = casual_nod;
                    memset(&(message.msg_text), 0, MSG_SIZE * sizeof(char));
                    (void)strcpy(message.msg_text,transaction);
                    if (msgsnd(msg_id, &message,sizeof(long)+(strlen(message.msg_text) * sizeof(char)) + 1, 0) == -1) {
                    perror("msgsnd");
                    return EXIT_FAILURE;
                    }

                    /*attesa in nanosecondi*/
                    ts.tv_sec = 0;
                    ts.tv_nsec = get_attesa(SO_MAX_TRANS_GEN_NSEC,SO_MIN_TRANS_GEN_NSEC);
                    nanosleep(&ts,NULL);
                
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
                array_utenti[count]=0;
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
    free(block_transaction);

    return 0;
}