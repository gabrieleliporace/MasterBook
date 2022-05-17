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
 *genero l'id della transazione
 */
char * id (int utenti[],int sender){
    size_t u = sizeof(utenti)/sizeof(utenti[0]);
    int reciver,r;
    r = rand() % u;
    reciver = utenti[r];
    char * i_d = genera_Identificatore(sender,reciver);
    return i_d;
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
int all_utenti(int reward,int bilancio,char * identificatore){
    int vtran,vreward,quantity;
    /*verifico che il bilancio sia >= a 2*/
    if (bilancio >= 2){
    /*calcolo il valore totale della transazione*/
    vtran = val_transazione(bilancio);
    /*richiamo la funzione per il calcolo del reward*/
    vreward = val_reward(vtran,reward); 
    /*sotraggo il valore di reward per trovare la quantita' da dare alla transazione*/
    quantity = vtran - vreward; 
    /*creo la transazione*/
    char * T = transaction_create(identificatore,quantity,vreward); 
    return 0;
    /*se il bilancio risulta minore di 2*/
    }
    else
    {
        return -1;
    }

    
    
}



