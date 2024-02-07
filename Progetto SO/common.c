#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include "common.h"
#define RESPONSE_SIZE (sizeof(char))
#define SIZE_REP (sizeof(int))*3

#define TEST_ERROR    if (errno) {dprintf(2, \
					   "%s:%d: PID=%5d: Error %d (%s)\n",\
					   __FILE__,\
					   __LINE__,\
					   getpid(),\
					   errno,\
					   strerror(errno));}
/*
#define SO_NAVI 10
#define SO_SPEED 5
#define SO_CAPACITY 5
#define SO_LATO 100.0
#define SO_MERCI 2
#define SO_DAYS 30

#define SO_PORTI 8
#define SO_BANCHINE 2
#define SO_FILL 100
#define SO_LOADSPEED 5

#define SO_MIN_VITA 10
#define SO_MAX_VITA 30

#define SO_SIZE 2
*/


void leggi_variabili(){

	FILE *fconf;
	fconf = fopen("valori.txt","r");
	fscanf(fconf,"%d",&SO_NAVI);
	fscanf(fconf,"%d",&SO_PORTI);
	fscanf(fconf,"%d",&SO_MERCI);
	fscanf(fconf,"%d",&SO_SIZE);
	fscanf(fconf,"%d",&SO_MIN_VITA);
	fscanf(fconf,"%d",&SO_MAX_VITA);
	fscanf(fconf,"%le",&SO_LATO);
	fscanf(fconf,"%d",&SO_SPEED);
	fscanf(fconf,"%d",&SO_CAPACITY);
	fscanf(fconf,"%d",&SO_BANCHINE);
	fscanf(fconf,"%d",&SO_FILL);
	fscanf(fconf,"%d",&SO_LOADSPEED);
	fscanf(fconf,"%d",&SO_DAYS);
	fclose(fconf);
	
}


/*TODO separare file header .h in un file .c*/
/*TODO da eliminare i semafori per sincronizzare le code di messaggi*/
/*TODO
Cose da fare:
	-
	*/



/* Set a semaphore to a user defined value */
int sem_set_val(int sem_id, int sem_num, int sem_val) {

	return semctl(sem_id, sem_num, SETVAL, sem_val);
}


/* Try to access the resource */
int sem_reserve(int sem_id, int sem_num) {
	struct sembuf sops;
	
	sops.sem_num = sem_num;
	sops.sem_op = -1;
	sops.sem_flg = 0;
	return semop(sem_id, &sops, 1);
}

/* Release the resource */
int sem_release(int sem_id, int sem_num) {
	struct sembuf sops;
  
	sops.sem_num = sem_num;
	sops.sem_op = 1;
	sops.sem_flg = 0;
	
	return semop(sem_id, &sops, 1);
}



/* Print all semaphore values to a string */
int sem_getall(char * my_string, int sem_id) {
	union semun arg;   /* man semctl per vedere def della union  */ 
	unsigned short * sem_vals, i;
	unsigned long num_sem;
	char cur_str[10];
	struct semid_ds my_ds;
	
	/* Get the number of semaphores */
	arg.buf = &my_ds;
	semctl(sem_id, 0, IPC_STAT, arg);
	TEST_ERROR;
	num_sem = arg.buf->sem_nsems;
	
	/* Get the values of all semaphores */
	sem_vals = malloc(sizeof(*sem_vals)*num_sem);
	arg.array = sem_vals;
	semctl(sem_id, 0, GETALL, arg);
	
	/* Initialize the string. MUST be allocated by the caller */
	my_string[0] = 0;
	for (i=0; i<num_sem; i++) {
		sprintf(cur_str, "%d ", sem_vals[i]);
		strcat(my_string, cur_str);
	}
}


/*Ottiene la capacità delle navi in base alla dimensione del lotto
int get_capacity(){
	int capacity=SO_CAPACITY;
	if(capacity % dim_lotto!=0){
		capacity=capacity-1;
	
	}
	
	return capacity;
}
*/





