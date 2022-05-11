#include "utenti.h"
#include "transazioni.h"
/*#include "master.h"*/

/*Calcola il bilancio corrente*/

/*int val_transazione(long Bill,int reward){
}*/
/*calcola il valore e la reward*/
int val_transazione(long Bill,int reward){
    int n;
    if (Bill>=2){
    srand(time(NULL));
    n = rand()%Bill + 2; /*estrae un valore casuale per la transazione*/
    return n;
    }else{
        return 0;
    }
}



