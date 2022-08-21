#ifndef _UTENTI_H
#define _UTENTI_H

/*Librerie*/
#define _GNU_SOURCE 
#include <stdio.h> 
#include <string.h> /*Manipolazione stringhe*/
#include <stdlib.h> /*Conversione da numeri a stringhe e viceversa, anche generatori casuali*/
#include <sys/types.h> 
#include <unistd.h> 
#include <time.h> /*Manipolazione date e valori temporali*/
#include <sys/time.h> 
#include <sys/wait.h>
#include <sys/shm.h>
#include <signal.h>/*Gestione dei segnali fra processi*/

/* 
 *Definizione metodi
 */
long get_quantity(pid_t miopid,char *transaction);
char * creazione_transazione(int reward,long bilancio,int reciver,int sender);
long get_attesa (int tmax,int tmin);



#endif 
    