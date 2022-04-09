#include "transazioni.h"

#define SIZE_CONV 11

/*
 * converte il pid in una stringa corrispondente
 */
int converti(long input, char *output)
{
    int size;
    size=snprintf(NULL,0,"%ld",input)+1;
    snprintf(output,size,"%ld",input);

    return 0;
}

/*
 * utilizza i pid di sender e receiver per creare l'identificatore della 
 * transazione in modo da ottenere "<timestamp,pid_sender,pid_receiver>"
 */
char * genera_Identificatore(long sender,long receiver)
{
    int timestamp;
    long id;
    char *identificatore, *send, *receive, *identifier;
    struct timespec spec;
    
    identifier=malloc(SIZE_CONV);
    strcpy(identifier,"<");
    identificatore=malloc(SIZE_CONV);
    send=malloc(SIZE_CONV);
    receive=malloc(SIZE_CONV);
    clock_gettime(CLOCK_REALTIME,&spec);
    converti(spec.tv_nsec,identificatore);
    strcat(identificatore,",");
    converti(sender,send);
    strcat(send,",");
    strcat(identificatore,send);
    converti(receiver,receive);
    strcat(receive,">");
    strcat(identificatore,receive);
    strcat(identifier,identificatore);

    return identifier;
    
}

/*
 * utilizza l'identificatore della transazione per creare la stringa completa
 * che caratterizza la transazione
 */
char * transaction_create(char * id,long quantity,long reward)
{
    char * qnty, *rew;

    qnty=malloc(SIZE_CONV);
    rew=malloc(SIZE_CONV);
    strcat(id,",");
    converti(quantity,qnty);
    strcat(id,qnty);
    strcat(id,",");
    converti(reward,rew);
    strcat(id,rew);

    return id;
}


/*
 * permette di recuperare dalla stringa della transazione il valore di reward
 */
long get_reward(char * transaction)
{
    const char sep=',';
    char * last;
    char * const sep_at=strrchr(transaction,sep);
    if(sep_at!=NULL){
        /**sep_at='\0';*/
        last=sep_at+1;
        return atol(last);
    }
}

/*long get_receiver(char * transaction)
{
    const char sup='>';
    const char sep=',';
    char * prev;
    char * last;
    char * sep_at;
    last=transaction;
    sep_at=strchr(last,sup);
    if(sep_at != NULL){
        *sep_at='\0';
        prev=last;
    }
    sep_at=strrchr(prev,sep);
    if(sep_at != NULL){
        *sep_at='\0';
        prev=sep_at+1;
    }
    return atol(prev);
}*/

/*
 * recupera dalla stringa della transazione il processo sender
 */
long get_sender(char * transaction)
{
    const char sup=',';
    char * trans;
    char * prev, * sep_ut;
    trans=transaction;
    sep_ut=strchr(trans,sup);
    if(sep_ut != NULL){
        *sep_ut='\0';
        prev=sep_ut+1;
    }
    sep_ut=strchr(prev,sup);
    if(sep_ut != NULL){
        *sep_ut='\0';
    }

    return atol(prev);
}

/*
 * recupera l'identificatore dalla stringa della transazione
 */
char * shrink(char * transaction)
{
    const char sep=',';
    char * shrinked;
    int i;
    char * sep_at;
    for(i=0;i<2;i++){
        sep_at=strrchr(transaction,sep);
        if(sep_at!=NULL){
            *sep_at='\0';
            shrinked=transaction;
        }
    }
    return shrinked;
}

/*
 * recupera il valore quantity dalla stringa della transazione e
 * lo restituisce positivo se il processo miopid corrisponde al 
 * receiver, mentre lo restituisce negativo se miopid corrisponde al sender
 */
long get_quantity(pid_t miopid,char * transaction)
{
    int i;
    char * trans;
    char * shortened;
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

    printf("last: %ld\n",atol(last));
    i=get_sender(trans);

    if(i==miopid){
        return -atol(last);
    }else{
        return atol(last);
    }

} 
