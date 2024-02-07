#include <errno.h>
#include <unistd.h>

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
int  SO_NAVI;
int SO_SPEED;
int SO_MERCI;
int SO_CAPACITY;
int SO_DAYS;
int SO_SIZE;
int SO_PORTI;
int SO_FILL;
int SO_LOADSPEED;
int SO_MIN_VITA;
int SO_MAX_VITA;
int SO_BANCHINE;
double SO_LATO;

void leggi_variabili();


/*TODO separare file header .h in un file .c*/
/*TODO da eliminare i semafori per sincronizzare le code di messaggi*/
/*TODO
Cose da fare:
	-
	*/



typedef struct  {
	long mytype;
	int pid;
	double x;
	double y;
	
	int tipo_merce_nave;
	int quantita_merce_nave;
	int durata_merce_nave;
	
	int stato;/*0->mare 1->porto*/
	
	int merce_scaduta_nave;
	
	
}report_navi;

typedef struct  {
	long mytype;
	
	int velocita_scaricamento;
	
	int num_banchine;
		
	int domanda_tipo_merce;
	int domanda_quantita_merce;
	int domanda_durata_merce;
		
	int offerta_tipo_merce;
	int offerta_quantita_merce;
	int offerta_durata_merce;
		
	double x;
	double y;
	
	int pid;
	
	int merci_ricevute;
	int merci_inviate;
	
	int num_banchine_usate;
	
	int merce_scaduta_porto;
	
	
		
	
}report_porti;



/* Set a semaphore to a user defined value */
int sem_set_val(int sem_id, int sem_num, int sem_val);

/* Try to access the resource */
int sem_reserve(int sem_id, int sem_num) ;
	

/* Release the resource */
int sem_release(int sem_id, int sem_num);

union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
				    (Linux-specific) */
};

/* Print all semaphore values to a string */
int sem_getall(char * my_string, int sem_id); 


/*Ottiene la capacità delle navi in base alla dimensione del lotto
int get_capacity(){
	int capacity=SO_CAPACITY;
	if(capacity % dim_lotto!=0){
		capacity=capacity-1;
	
	}
	
	return capacity;
}
*/





