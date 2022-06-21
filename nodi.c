#include "nodi.h"
#include "transazioni.h"

/*creo la transazione reward*/
char * transazione_reward(int reward,long somma_reward,int receiver,int sender){
    char * reward_transazione;
    char * id;
    id = genera_Identificatore(sender,receiver);
    reward_transazione = transaction_create(id,somma_reward,reward);
    return reward_transazione;
    
}
/*creazione del blocco e della transaction pool*/
long take_reward (char * transazione){
    long r = get_reward(transazione);
    return r;
}