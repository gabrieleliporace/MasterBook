#include "transizioni.h"

#define SIZE_CONV 11

int converti(long input, char *output)
{
    int size;
    size=snprintf(NULL,0,"%ld",input)+1;
    snprintf(output,size,"%ld",input);

    return 0;
}


char * generaIdentificatore(long sender,long receiver)
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
