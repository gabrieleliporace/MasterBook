#include "utenti.h"
#include "transazioni.h"

/*
Calcola il bilancio corrente
*/
int get_balance(int b_i){
    /*ritorno il bilancio iniziale solo per prova*/
    return b_i;
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
    }else {
    r = 1;
    return r;
    }
    
}
/*
 *estraggo randomicamente il tempo d'attesa che il processo utente deve avere
 */
int get_attesa(int tmax,int tmin){
    int tempo_attesa;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME,&spec);
    srand(spec.tv_nsec);
    tempo_attesa = rand() % tmax + tmin;
    return tempo_attesa;
}
/*
 *funzione per creare la transazione
 */
char * creazione_transazione(int reward,int bilancio,int reciver,int sender){
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



