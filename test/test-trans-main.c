/**
 * File per testare funzionamento di transizioni.c
 **/
#include "../transazioni.h"

int main()
{
    long reward;
    char * id, *ad;
    long rec,send;
    long tr;
    pid_t miopid;
    miopid=getpid();
    printf("miopid: %d\n",miopid);
#ifdef SENDER
    id=genera_Identificatore(miopid,2297299);
#else
    id=genera_Identificatore(2297299,miopid);
#endif
    id=transaction_create(id,234,324342);
    printf("%s\n",id);
    ad=id;
    reward=get_reward(id);
    printf("reward: %ld\n",reward);
    printf("ad:%s\n",ad);
    tr=get_quantity(miopid,ad);
    printf("qnt: %ld\n",tr);
    rec=get_receiver(ad);
    printf("rec:%ld\n",rec);
    send=get_sender(ad);
    printf("send:%ld\n",send);

    return 0;
}
