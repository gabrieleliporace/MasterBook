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
int get_balance(int b_i);
char * id (int utenti[],int sender);
int all_utenti(int reward,int bilancio,char * identificatore);
int get_attesa (int tmax,int tmin);


#endif 
    