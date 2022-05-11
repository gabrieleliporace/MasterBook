#include "utenti.h"
#include "Transazioni.h"

/*Calcola il bilancio corrente*/

int val_transazione(long Bill,int reward){
}
/*calcola il valore e la reward*/
int val_transazine(long Bill,int reward){
    if (Bill>=2){
    srand(time(NULL));
    int n = rand()%Bill + 2; /*estrae un valore casuale per la transazione*/
    return n;
    }else{
        return 0;
    }
}



