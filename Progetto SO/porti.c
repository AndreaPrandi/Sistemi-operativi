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
#include "common.h"
#include "porti.h"

/*Creo un array di struct di tipo report_porti*/
report_porti  report_porti_giornaliero;
report_navi  report_navi_giornaliero;

int main(int argc, char * argv[]) {
	leggi_variabili();
	
	velocita_scaricamento = atoi(argv[1]);
	
	domanda_quantita_merce = atoi(argv[2]);
	offerta_quantita_merce = atoi(argv[3]);
	
	num_banchine = atoi(argv[4]);
	
	domanda_tipo_merce = atoi(argv[5]);
	domanda_durata_merce = atoi(argv[6]);
	
	offerta_tipo_merce = atoi(argv[7]);
	offerta_durata_merce = atoi(argv[8]);
	
	porto_coord_x = atof(argv[9]);
	porto_coord_y = atof(argv[10]);
	
	id_report_giornaliero_porti = atoi(argv[11]);
	sem_id_report_giornaliero_porti = atoi(argv[12]);
	
	j = atoi(argv[13]);
	
	id_report_giornaliero_navi = atoi(argv[14]);
		
	id_semaforo_porti_banchine = atoi(argv[15]);
	
	report_porti_giornaliero.mytype = j;
	report_porti_giornaliero.x = porto_coord_x;
	report_porti_giornaliero.y = porto_coord_y;
	
	report_porti_giornaliero.velocita_scaricamento=velocita_scaricamento;
	
	/*aggoingo il resto all'ultimo porto*/
	if(j==SO_PORTI){
		report_porti_giornaliero.domanda_quantita_merce=(SO_FILL/SO_PORTI)+(SO_FILL%SO_PORTI);
	
		report_porti_giornaliero.offerta_quantita_merce=(SO_FILL/SO_PORTI)+(SO_FILL%SO_PORTI);
	}
	else{
		report_porti_giornaliero.domanda_quantita_merce=(SO_FILL/SO_PORTI);
	
		report_porti_giornaliero.offerta_quantita_merce=(SO_FILL/SO_PORTI);
	}
	
	
	report_porti_giornaliero.num_banchine=num_banchine;
	
	report_porti_giornaliero.domanda_tipo_merce=domanda_tipo_merce;
	report_porti_giornaliero.domanda_durata_merce=domanda_durata_merce;
	
	report_porti_giornaliero.offerta_tipo_merce=offerta_tipo_merce;
	report_porti_giornaliero.offerta_durata_merce=offerta_durata_merce;
	
	report_porti_giornaliero.merci_ricevute=0;
	report_porti_giornaliero.merci_inviate=0;
	report_porti_giornaliero.num_banchine_usate=0;
	
	report_porti_giornaliero.merce_scaduta_porto=0;
	
	report_porti_giornaliero.pid = getpid();
	
	size_porti=sizeof(report_porti_giornaliero)-sizeof(long);
		
	/*Invio il messaggio contenente lo stato dei porti*/
	msgsnd(id_report_giornaliero_porti,&report_porti_giornaliero,size_porti,0);
	
	size_navi=sizeof(report_navi_giornaliero)-sizeof(long);
	
	while(1){
		
		
		sem_reserve(sem_id_report_giornaliero_porti,j);
		
		msgrcv(id_report_giornaliero_porti, &report_porti_giornaliero,size_porti,j,0);
		
		/*variabili per il report giornaliero*/
	
		/*se la durata dell'offerta è = 0*/
		if(report_porti_giornaliero.offerta_durata_merce == 0 &&
		 report_porti_giornaliero.offerta_quantita_merce>0) {
			printf("========= Merce Scaduta (offerta) nel Porto %d, rimuovo tutto =========\n",j);
			report_porti_giornaliero.merce_scaduta_porto=
			report_porti_giornaliero.merce_scaduta_porto +
			report_porti_giornaliero.offerta_quantita_merce;
			
			/*report_porti_giornaliero.offerta_tipo_merce = 0;*/
			report_porti_giornaliero.offerta_quantita_merce = 0;
				
		}
		/*se la durata della domanda è = 0*/
		if(report_porti_giornaliero.domanda_durata_merce == 0 &&
		report_porti_giornaliero.domanda_quantita_merce>0) {
			printf("========= Merce Scaduta (domanda) nel Porto %d, rimuovo tutto ========\n",j);
			
			/*report_porti_giornaliero.domanda_tipo_merce = 0;*/
			report_porti_giornaliero.domanda_quantita_merce = 0;
				
		}

		msgsnd(id_report_giornaliero_porti,&report_porti_giornaliero,size_porti,0);
		
	}
	
	exit(1);
	
	return 0;

}
