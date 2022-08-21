#define _GNU_SOURCE
#include "master.h"
#include "utenti.h"
#include "nodi.h"

void my_handler(int signum)
{
    switch(signum){
    
        case SIGALRM:
            timer=0;
            break;

        case SIGUSR1:
            invio=1;
            break;

        case SIGINT:
            raise(SIGALRM);
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
    long t_attesa,bilancio,bilancio_mastro;
    char * transaction,nod_transaction;
    char * transaction_bi;

    /*dichiarazioni per processi nodo*/  
    struct master * master;
    pid_t * array_nodi,pid_nod;
    int nodi,casual_nod,numnodi,numutenti; 
    int tpi,tpj,shared_mastro,shared_registro;
    long som_reward,som_rew_tot; 
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

    bzero(&sa, sizeof(sa));
    sa.sa_handler=&my_handler;
    sigaction(SIGUSR1,&sa,NULL);
    sigaction(SIGINT,&sa,NULL);

    inizializzazione_valori(); /*Richiamo la funzione per le variabili globali*/
    
    /*creazioe coda di messaggi per mandare la transazione*/
    msg_id = msgget(MSG_KEY,IPC_CREAT | 0600);
    if (msg_id == -1) {
        perror("msgget");
        return EXIT_FAILURE;
    }

    /* creazione di semaforo per la sincronizzazione dei processi */
	sem_id=semget(SEM_KEY,4,IPC_CREAT|0600);
	semctl(sem_id,0,SETVAL,0);
	semctl(sem_id,1,SETVAL,0);
	semctl(sem_id,2,SETVAL,0);
    semctl(sem_id,3,SETVAL,1);

    /*Creazione memoria condivisa nodi e utenti e mastro*/
    shm_utenti=shmget(SHDM_UTENTI,SO_USERS_NUM*sizeof(*array_utenti),IPC_CREAT|0600);
    shm_nodi=shmget(SHDM_NODI,SO_NODES_NUM*sizeof(*array_nodi),IPC_CREAT|0600);
    shared_mastro=shmget(SHDM_MASTRO,sizeof(master->mastro),IPC_CREAT|0600);

    /*Dichiarazione array per PID utenti e PID nodi*/
    array_utenti=shmat(shm_utenti,NULL,0);
    array_nodi=shmat(shm_nodi,NULL,0);
    master=shmat(shared_mastro,NULL,0);

    /*Marchio la shared memory come pronta per la rimozione, 
      viene rimossa quando tutti i processi si sono staccati*/
    shmctl(shm_utenti,IPC_RMID,NULL);
    shmctl(shm_nodi,IPC_RMID,NULL);
    shmctl(shared_mastro,IPC_RMID,NULL);
    
    miopid=getpid();
    sops.sem_flg=0;

    /*malloc per transaction_block*/
    block_transaction = malloc(SO_BLOCK_SIZE*sizeof(char*));

    bilancio = SO_BUDGET_INIT;

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

            /*Semaforo per inizializzazione array_nodi*/
            sops.sem_num=1;	
            sops.sem_op=-1;
            semop(sem_id,&sops,1);

            /*Blocco per sincronizzare con l'avvio del timer*/
            sops.sem_num=2;
            sops.sem_op=-1;
            semop(sem_id,&sops,1);

            while(1){

            sops.sem_num = 3;
            sops.sem_op = -1;
            sops.sem_flg = 0;
            semop(sem_id,&sops,1);

            for(tpi = 0;tpi <= SO_BLOCK_SIZE-2;tpi++){

                if(master->registro==1){
                    printf("Processo terminato causa: Master pieno.");
                    exit(-1);
                    }

                /*reciver coda di messaggi*/  
                msgrcv(msg_id, &message,MSG_SIZE,getpid(),0); 
                
                /*blocco transazioni*/
                block_transaction[tpi]=message.msg_text;
                
                /*Mando transazioni al libro Mastro*/
                master->mastro[master->registro]=block_transaction[tpi];
                som_reward += take_reward(block_transaction[tpi]);
                block_transaction[tpi]="/0";
                printf("pid:%d mastro[%ld]: %s\n",getpid(),master->registro,master->mastro[master->registro]);

                if (tpi == SO_BLOCK_SIZE-2){
                    block_transaction[SO_BLOCK_SIZE-1] = transazione_reward(0,som_reward,getpid(),SEND);    
                    master->mastro[master->registro]=block_transaction[SO_BLOCK_SIZE-1];
                    printf("pid:%d mastro[%ld]: %s\n",getpid(),master->registro,master->mastro[master->registro]);

                    /*Simulazione elaborazione in nanosecondi*/
                    te.tv_sec = 0;
                    te.tv_nsec = get_attesa(SO_MAX_TRANS_PROC_NSEC,SO_MIN_TRANS_PROC_NSEC);
                    nanosleep(&ts,NULL);
                        
                    /*calcolo bilancio del nodo*/
                    som_rew_tot += som_reward;
                    /*printf("Il bilancio del nodo %d e' %ld\n",getpid(),som_rew_tot);*/

                    som_reward = 1; 
                    master->registro++;

                    sops.sem_num = 3;
                    sops.sem_op = 1;
                    sops.sem_flg = 0;
                    semop(sem_id,&sops,1);
                    
                } 
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
            sops.sem_op=-1;
            semop(sem_id,&sops,1);

            /*Blocco per sincronizzare con l'avvio del timer*/
            sops.sem_num=2;
            sops.sem_op=-1;
            semop(sem_id,&sops,1);

            numutenti=SO_USERS_NUM;
            numnodi=SO_NODES_NUM;
            myretry=SO_RETRY;

            while(myretry){

                /*while(contatore <= master->registro){
                bilancio += get_quantity(getpid(),master->mastro[contatore]);
                printf("Il bilancio dell'utente %d e': %ld\n",getpid(),bilancio);
                contatore++;
                }*/
                
                if(invio){
                    if(bilancio>=2){
                    
                        sender = getpid();
                        casual_nod = get_casual_pid(array_nodi,numnodi);
                        do{
                        receiver = get_casual_pid(array_utenti,utenti);
                        }while(receiver==0);
                        

                        /*creo la transazione*/
                        transaction = creazione_transazione(SO_REWARD,bilancio,receiver,sender);

                        printf("Invio transazione causa segnale \
USR1, %s\n", transaction);

                        /*coda di messaggi*/
                        message.pid_nod_type = casual_nod;
                        memset(&(message.msg_text), 0, MSG_SIZE * sizeof(char));
                        (void)strcpy(message.msg_text,transaction);
                        if (msgsnd(msg_id, &message,sizeof(long)+(strlen(message.msg_text) * sizeof(char)) + 1, 0) == -1) {
                        perror("msgsnd");
                        return EXIT_FAILURE;
                        }
                    }else{
                        printf("Impossibile inviare transazione, \
bilancio insufficiente\n");
                    }
                    invio=0;
                    }

                if(bilancio >= 2){

                    myretry=SO_RETRY;

                    /*Estrazione casuale pid nodo e utente*/
                    sender = getpid();
                    casual_nod = get_casual_pid(array_nodi,numnodi);
                    do{
                    receiver = get_casual_pid(array_utenti,numutenti);
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

                
                ts.tv_sec = 1;
                ts.tv_nsec = 0;
                nanosleep(&ts,NULL);

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
	sops.sem_op=utenti;
	semop(sem_id,&sops,1);
    
    /*Lascio partire i nodi*/
	sops.sem_num=1;	
	sops.sem_op=nodi;
	semop(sem_id,&sops,1);

    /*Avvio la simulazione e il timer*/
	sops.sem_num=2;	
	sops.sem_op=(utenti+nodi);
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
        printf("processo terminato: %d , %d, %d , %d\n", WTERMSIG(status), WIFSIGNALED(status), u, vivi);
    }
    
    sigaction(SIGTERM,&sa,NULL);
    printf("nodi %d\n", nodi);
    printf("miopid %d\n", getpid());

	/*Rimuovo il semaforo*/
	semctl(sem_id,0,IPC_RMID);

    msgctl(msg_id, IPC_RMID, NULL); 
    free(block_transaction);

    return 0;
}