#include "utenti.h"
#include "transazioni.h"

/*
 * recupera il valore quantity dalla stringa della transazione e
 * lo restituisce positivo se il processo miopid corrisponde al 
 * receiver, mentre lo restituisce negativo se miopid corrisponde al sender
 */

long get_quantity(pid_t miopid,char * transaction){
    int i,j;
    char * trans;
    int shortened;
    const char sep='>';
    const char sip=',';
    char * last,* sep_it,* sep_at;
    trans=malloc(sizeof(transaction));
    strcpy(trans,transaction);
    shortened=shrink(trans);
    sep_at=strrchr(transaction,sep);
    if(sep_at!=NULL){
        last=sep_at;
    }

    sep_it=strchr(last,sip);
    if(sep_it!=NULL){
        *sep_it='\0';
        last=sep_it+1;
    }

    /*printf("last: %ld\n",atol(last));*/
    i=get_sender(trans);
    j=get_receiver(trans);

    if(i==miopid){
        return -atol(last);
    }else if(j==miopid){
        return atol(last);
    }else if(j!=miopid && i!=miopid){
        return 0;
    }

} 

/* 
 *Calcolo valore transazione
 */

int val_transazione(int bilancio){
    int n;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME,&spec); /*prendo il timestemp attuale*/
    srand(spec.tv_nsec);
    n = rand() % bilancio + 2; /*estrae un valore casuale per la transazione*/
    return n;
}

/*
 *Calcolo valore reward
 */

int val_reward(int vtran,int reward){
    int r = (vtran * reward ) / 100;
    if (r >= 1){
    return r;
    }else if (r<1){
    r = 1;
    return r;
    }
    
}

/*
 *estraggo randomicamente il tempo d'attesa che il processo utente deve avere
 */

long get_attesa(int tmax,int tmin){
    long tempo_attesa;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME,&spec);
    srand(spec.tv_nsec);
    tempo_attesa = rand() % tmax + tmin;
    return tempo_attesa;
}

/*
 *funzione per creare la transazione
 */

char * creazione_transazione(int reward,long bilancio,int reciver,int sender){
    char * T;
    char * i_d;
    int vtran,vreward,quantity;
    /*genero il pid della transazione*/
    i_d = genera_Identificatore(sender,reciver);
    /*calcolo il valore totale della transazione*/
    vtran = val_transazione(bilancio);
    /*richiamo la funzione per il calcolo del reward*/
    vreward = val_reward(vtran,reward); 
    /*sotraggo il valore di reward per trovare la quantita' da dare alla transazione*/
    quantity = vtran - vreward; 
    /*creo la transazione*/
    T = transaction_create(i_d,quantity,vreward); 
    return T;
    
}
