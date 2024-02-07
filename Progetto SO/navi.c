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
#include "navi.h"

/*Creo un array di struct di tipo report_navi*/
report_navi  report_navi_giornaliero;
report_porti  report_porti_giornaliero;	


int main(int argc, char * argv[]) {

	leggi_variabili();
	
	velocita = atoi(argv[1]);
	capacita = atoi(argv[2]);
	coord_x = atof(argv[3]);
	coord_y = atof(argv[4]);
	id_report_giornaliero = atoi(argv[5]);
	i = atoi(argv[6]);
	sem_id_report_giornaliero = atoi(argv[7]);
	id_report_giornaliero_porti = atoi(argv[8]);
	
	id_semaforo_porti_banchine = atoi(argv[9]);
	
	report_navi_giornaliero.mytype = i;
	report_navi_giornaliero.pid = getpid();
	report_navi_giornaliero.x = coord_x;
	report_navi_giornaliero.y = coord_y;
	
	report_navi_giornaliero.tipo_merce_nave = 0;
	report_navi_giornaliero.quantita_merce_nave = 0;
	report_navi_giornaliero.durata_merce_nave = 0;
	
	report_navi_giornaliero.merce_scaduta_nave = 0;
	
	
	
	report_navi_giornaliero.stato = 0;/*->in mare*/
	
	size_navi=sizeof(report_navi_giornaliero)-sizeof(long);
	
	/*Invio il messaggio contenente lo stato della nave*/
	msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
	

	while(1){
		
		/*Acquisisco il semaforo*/
		sem_reserve(sem_id_report_giornaliero,i);
		
		size_porti=sizeof(report_porti_giornaliero)-sizeof(long);
		
		min_dist=SO_LATO;

		msgrcv(id_report_giornaliero, &report_navi_giornaliero,size_navi,i,0);
		
		
		/*se la durata della merce è uguale a 0 la elimino*/
		if(report_navi_giornaliero.durata_merce_nave==0 &&
		 report_navi_giornaliero.quantita_merce_nave>0){
		 
		 	report_navi_giornaliero.merce_scaduta_nave =
			report_navi_giornaliero.merce_scaduta_nave +
			report_navi_giornaliero.quantita_merce_nave;
			
			report_navi_giornaliero.tipo_merce_nave=0;
			report_navi_giornaliero.quantita_merce_nave=0;
			
			
			
			printf("===== Merce scaduta nella nave %d ==== \n",i);
			
		}
		
		
		msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
		
		
		/*---------------------------*/
		/*Se la nave è vuota*/
		/*---------------------------*/
		if(report_navi_giornaliero.quantita_merce_nave==0){
		
			var_nave_sul_porto=0;
			min_dist=SO_LATO*2;
			
			/* trovo le coordinate del porto più vicino alla nave */
			for(t=1;t<=SO_PORTI && var_nave_sul_porto==0;t++){
				
				msgrcv(id_report_giornaliero_porti, &report_porti_giornaliero,size_porti,t,0);
				msgsnd(id_report_giornaliero_porti,&report_porti_giornaliero,size_porti,0);
				
				x_porto = report_porti_giornaliero.x;
				y_porto = report_porti_giornaliero.y;
				x_nave = report_navi_giornaliero.x;
				y_nave = report_navi_giornaliero.y;
				
				/*la nave si trova già sul porto che ha un'offerta>0 */
				if(x_porto==x_nave &&
				y_porto==y_nave &&
				report_porti_giornaliero.offerta_quantita_merce > 0){
					
					var_nave_sul_porto=1;
					id_porto_nave=t;
					
				}
				/*se il porto un'offerta di merce>0 e è ha la distanza minore di min_dist*/
				if(report_porti_giornaliero.offerta_quantita_merce > 0 && 
				min_dist > distanza(x_porto,y_porto,x_nave,y_nave)){
				
					min_dist = distanza(x_porto,y_porto,x_nave,y_nave);
					x_porto_min = x_porto;
					y_porto_min = y_porto;
				}
			
			
			}
			
			/*---------------------------*/
			/*Nave vuota è già sul porto*/
			/*---------------------------*/
			if(var_nave_sul_porto==1){
			struct timespec my_time2;
				
				printf("===== La nave %d tenta l'accesso ad una banchina del porto %d==== \n"
				,i,id_porto_nave);
				
				/*calcolo il tempo di caricamento della nave*/
				sem_reserve(id_semaforo_porti_banchine,id_porto_nave);
				
				printf("===== La nave %d accede ad una banchina del porto %d per caricare==== \n"
				,i,id_porto_nave);
					
			
				msgrcv(id_report_giornaliero_porti, 
				&report_porti_giornaliero,size_porti,id_porto_nave,0);
				
				report_porti_giornaliero.num_banchine_usate=
				report_porti_giornaliero.num_banchine_usate+1;
				
				/*se il porto ha ancora della merce*/
				if(report_porti_giornaliero.offerta_quantita_merce>=SO_CAPACITY){
					carico=SO_CAPACITY;
					
					printf("===== La nave %d inizia ora a caricare per %d dal porto %d==== \n"
				,i,carico/SO_LOADSPEED,id_porto_nave);
				
				}
				/*il carico è uguale a zero*/
				else{
					carico=report_porti_giornaliero.offerta_quantita_merce;
					
					printf("===== La nave %d NON carica dal porto %d poichè è terminata l'offerta==== \n"
				,i,id_porto_nave);
					
				}
				
				msgsnd(id_report_giornaliero_porti,&report_porti_giornaliero,size_porti,0);
				

				/*tempo di caricamento della nave*/
				
			
				my_time2.tv_sec = carico/SO_LOADSPEED;
				my_time2.tv_nsec = 000000000;
				nanosleep(&my_time2,NULL);
				
				/* inserisco la merce sulla nave */
			
				msgrcv(id_report_giornaliero, &report_navi_giornaliero,size_navi,i,0);	
				msgrcv(id_report_giornaliero_porti, 
				&report_porti_giornaliero,size_porti,id_porto_nave,0);
				
				if(report_porti_giornaliero.offerta_quantita_merce==0){
					printf("===== La nave %d NON ha caricato nulla poichè il porto %d non ha offerta \n",i,id_porto_nave);
				}
				/*se la merce nel porto è maggiore della capacita della nave*/	
				if(report_porti_giornaliero.offerta_quantita_merce>=SO_CAPACITY){
					temp=report_porti_giornaliero.offerta_quantita_merce - SO_CAPACITY;
					report_porti_giornaliero.offerta_quantita_merce = temp;
							
					report_navi_giornaliero.quantita_merce_nave = SO_CAPACITY;
					report_navi_giornaliero.tipo_merce_nave = 
					report_porti_giornaliero.offerta_tipo_merce;
						
					report_navi_giornaliero.durata_merce_nave= 
					report_porti_giornaliero.offerta_durata_merce;
					
					report_porti_giornaliero.merci_inviate=
					report_porti_giornaliero.merci_inviate + SO_CAPACITY;
					
					report_porti_giornaliero.num_banchine_usate=
					report_porti_giornaliero.num_banchine_usate-1;
					
					msgsnd(id_report_giornaliero_porti,
					 &report_porti_giornaliero,size_porti,0);
					 
					msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
					
					
				}
				
				/*se la merce nel porto è minore della capacita della nave*/
				else{
					report_navi_giornaliero.quantita_merce_nave = 
					report_porti_giornaliero.offerta_quantita_merce;
					
					report_navi_giornaliero.tipo_merce_nave = 
					report_porti_giornaliero.offerta_tipo_merce;
						
					report_navi_giornaliero.durata_merce_nave= 
					report_porti_giornaliero.offerta_durata_merce;
					
					report_porti_giornaliero.merci_inviate=
					report_porti_giornaliero.merci_inviate +
					report_navi_giornaliero.quantita_merce_nave;
						
					report_porti_giornaliero.offerta_quantita_merce = 0;
					
					report_porti_giornaliero.num_banchine_usate=
					report_porti_giornaliero.num_banchine_usate-1;
					
					msgsnd(id_report_giornaliero_porti, &report_porti_giornaliero,size_porti,0);
					msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
					
					
				}
				printf("===== La nave %d ha finito di caricare e lascia la banchina==== \n",i);
				
				
				sem_release(id_semaforo_porti_banchine,id_porto_nave);
				
				
				

			}
			/*---------------------------*/
			/*la nave si trova lontano dal porto*/
			/*---------------------------*/
			else{
				struct timespec my_time;
				/*TODO da controllare*/
				/*non ho trovato nessun porto con offerta di merce >0*/
				if(min_dist==SO_LATO*2){
					printf("======= Nave %d IN PAUSA =======\n",i);
					pause();
				}
				/* calcolo il tempo che ci impiegherà per raggiungere il porto */
				durata_spostamento=(min_dist/SO_SPEED);
				
				parte_intera = floor(durata_spostamento);
				parte_decimale = (durata_spostamento - parte_intera)*1000000000;
				
				
					
				msgrcv(id_report_giornaliero, &report_navi_giornaliero,size_navi,i,0);
				
				/*imposto lo stato delle navi in mare*/
				report_navi_giornaliero.x = -1;
				report_navi_giornaliero.y = -1;
				report_navi_giornaliero.stato = 0;/*-> mare */
				
				msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
				
				
			
				
				printf("======= Nave %d vuota in viaggio per %f verso il porto (%f,%f) =======\n"
				,i,durata_spostamento,x_porto_min,y_porto_min);
				
				
				
				/*Tempo impegato per raggiungere il porto*/	
				
					
				my_time.tv_sec = parte_intera;
				my_time.tv_nsec = parte_decimale;
				
				nanosleep(&my_time,NULL);
				
				
			
				
				/*leggo il messaggio e lo consumo*/	
				msgrcv(id_report_giornaliero, &report_navi_giornaliero,size_navi,i,0);
				
				report_navi_giornaliero.x = x_porto_min;
				report_navi_giornaliero.y = y_porto_min;
				report_navi_giornaliero.stato = 1;/*-> porto */
				
				/*riscrivo il messaggio che ho appena consumato*/
				msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
				
				
			}
			
			
		
		}
		/*---------------------------*/
		/*Se la nave è piena*/
		/*---------------------------*/
		else{
			var_nave_sul_porto=0;
			
			min_dist=(SO_LATO*2);
			/*vado al porto più vicino che richiede la merce che trasporto*/
			for(t=1;t<=SO_PORTI && var_nave_sul_porto==0 ;t++){
				
				msgrcv(id_report_giornaliero_porti, &report_porti_giornaliero,size_porti,t,0);
				msgsnd(id_report_giornaliero_porti,&report_porti_giornaliero,size_porti,0);
							
				x_porto = report_porti_giornaliero.x;
				y_porto = report_porti_giornaliero.y;
				x_nave = report_navi_giornaliero.x;
				y_nave = report_navi_giornaliero.y;
				
				/*la nave si trova già sul porto che ha un'domanda>0 
				e che richiede la merce trasportata dalla nave*/
				if(x_porto==x_nave &&
				y_porto==y_nave &&
				report_porti_giornaliero.domanda_quantita_merce > 0 &&
				report_porti_giornaliero.domanda_tipo_merce == 
				report_navi_giornaliero.tipo_merce_nave ){
					
					var_nave_sul_porto=1;
					id_porto_nave=t;
					
				}
				
				/*se il porto ha la stessa domanda della merce che trasporta la nave
				ed è più vicino del porto che ho trovato in precedenza
				e richiede ancora merce */
				if(
				report_porti_giornaliero.domanda_quantita_merce>0 &&
				report_porti_giornaliero.domanda_tipo_merce == 
				report_navi_giornaliero.tipo_merce_nave && 
				min_dist > distanza(x_porto,y_porto,x_nave,y_nave) &&
				var_nave_sul_porto==0){
				
					min_dist = distanza(x_porto,y_porto,x_nave,y_nave);
					x_porto_min = x_porto;
					y_porto_min = y_porto;
				}
			
			
			}
			/*---------------------------*/
			/*Nave piena è già sul porto che richiede tale merce*/
			/*---------------------------*/
			if(var_nave_sul_porto==1){
				
				printf("===== La nave %d tenta di accedere ad una banchina del porto %d==== \n"
				,i,id_porto_nave);

				sem_reserve(id_semaforo_porti_banchine,id_porto_nave);

				printf("===== La nave %d accede ad una banchina del porto %d per scaricare==== \n"
				,i,id_porto_nave);
				
				/*calcolo il tempo di scaricamento della nave*/
				
				
				msgrcv(id_report_giornaliero_porti, 
				&report_porti_giornaliero,size_porti,id_porto_nave,0);
				
				/*add banchine usate per il report giornaliero*/
				report_porti_giornaliero.num_banchine_usate=
				report_porti_giornaliero.num_banchine_usate+1;
				
				if(report_porti_giornaliero.domanda_quantita_merce>=SO_CAPACITY){
					scarico=SO_CAPACITY;
				}
				else{
					scarico=report_porti_giornaliero.offerta_quantita_merce;
				}
				
				
				
				
				msgsnd(id_report_giornaliero_porti,&report_porti_giornaliero,size_porti,0);
				
				
				printf("===== La nave %d inizia ora a scaricare per %d dal porto %d==== \n"
				,i,(scarico/SO_LOADSPEED),id_porto_nave);	
			
				
				msgrcv(id_report_giornaliero, &report_navi_giornaliero,size_navi,i,0);

				/*Se la durata della merce è minore del tempo di caricamento nel porto*/
				if((scarico/SO_LOADSPEED)>report_navi_giornaliero.durata_merce_nave){
				
					report_navi_giornaliero.merce_scaduta_nave=
					report_navi_giornaliero.merce_scaduta_nave +
					report_navi_giornaliero.quantita_merce_nave;
					
					report_navi_giornaliero.tipo_merce_nave=0;
					report_navi_giornaliero.quantita_merce_nave=0;
					report_navi_giornaliero.durata_merce_nave=0;
					
					
					printf("===== Merce scaduta nella nave %d durante il caricamento ==== \n"
					,i);
					
					
				
					msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
				
					
				}
				/*se la merce non scade durante lo scaricamento*/
				else{
					struct timespec my_time2;
					msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
					
					/*tempo di scaricamento della nave*/
					
				
					my_time2.tv_sec = scarico/SO_LOADSPEED;
					my_time2.tv_nsec = 000000000;
					nanosleep(&my_time2,NULL);
					
					/*-----------------------------------*/
					/* scarico la merce dalla nave */
					/*-----------------------------------*/

					msgrcv(id_report_giornaliero, &report_navi_giornaliero,size_navi,i,0);	
					msgrcv(id_report_giornaliero_porti, 
					&report_porti_giornaliero,size_porti,id_porto_nave,0);
					
					
					/*se la merce richiesta dal porto è maggiore della capacita della nave*/	
					if(report_porti_giornaliero.domanda_quantita_merce>=SO_CAPACITY){
					
						report_porti_giornaliero.domanda_quantita_merce = 
						report_porti_giornaliero.domanda_quantita_merce - SO_CAPACITY;
								
						report_navi_giornaliero.quantita_merce_nave = 0;
						report_navi_giornaliero.tipo_merce_nave = 0;	
						report_navi_giornaliero.durata_merce_nave = 0;
						
						report_porti_giornaliero.merci_ricevute =
						report_porti_giornaliero.merci_ricevute +
						SO_CAPACITY;
						
						report_porti_giornaliero.num_banchine_usate=
						report_porti_giornaliero.num_banchine_usate-1;
						
						msgsnd(id_report_giornaliero_porti,
						&report_porti_giornaliero,size_porti,0);
						 
						msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
						
						
					}
					
					/*se la merce richiesta dal porto è minore della capacita della nave*/
					else{
						report_porti_giornaliero.merci_ricevute =
						report_porti_giornaliero.merci_ricevute +
						report_porti_giornaliero.domanda_quantita_merce;
						
						report_navi_giornaliero.quantita_merce_nave = 
						report_navi_giornaliero.quantita_merce_nave - 
						report_porti_giornaliero.domanda_quantita_merce;
							
						report_porti_giornaliero.domanda_quantita_merce = 0;
						
						report_porti_giornaliero.num_banchine_usate=
						report_porti_giornaliero.num_banchine_usate-1;
						
						
						msgsnd(id_report_giornaliero_porti, 
						&report_porti_giornaliero,size_porti,0);
						
						msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
						
						
					}
					printf("===== La nave %d ha finito di scaricare e lascia la banchina==== \n",i);
					sem_release(id_semaforo_porti_banchine,id_porto_nave);
					
				}
	
			}
			/*---------------------------*/
			/*La nave non si trova nel porto che richiede tale merce*/
			/*---------------------------*/
			else{
				
				/*Se non ho trovato nessn porto che richiede tale merce*/
				if(min_dist==(SO_LATO*2) ){
					
					
					printf("===== NAVE %d Svuotata poichè nessun porto richiede la merce ======\n",i);
					
					/*leggo il messaggio e lo consumo*/	
					msgrcv(id_report_giornaliero, &report_navi_giornaliero,size_navi,i,0);
					
					/*la nave quindi elimina la merce e quindi diventa una nave vuota 
					in un porto
					e verrà considerata come tale dal porto*/
					
					/*TODO elimino la merce della nave e la conto come merce 
					sprecata è giusto???? */
					
					report_navi_giornaliero.merce_scaduta_nave=
					report_navi_giornaliero.merce_scaduta_nave+
					report_navi_giornaliero.quantita_merce_nave;
					
					/*report_navi_giornaliero.tipo_merce_nave=0;*/
					report_navi_giornaliero.quantita_merce_nave=0;
					report_navi_giornaliero.durata_merce_nave=0;
					
					/*riscrivo il messaggio che ho appena consumato*/
					msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
					
					
				
				}
				/*se trovo un porto che richiede tale merce*/
				else{
					struct timespec my_time;
					
					/* calcolo il tempo che ci impiegherà per raggiungere il porto */
					durata_spostamento=(min_dist/SO_SPEED);
					
					printf("======= Nave %d carica in viaggio per %f verso il porto (%f,%f) =======\n"
					,i,durata_spostamento,x_porto_min,y_porto_min);
					
					parte_intera = floor(durata_spostamento);
					parte_decimale = (durata_spostamento - parte_intera)*1000000000;
					
					
					
					/*imposto lo stato della nave con mare*/	
					msgrcv(id_report_giornaliero, &report_navi_giornaliero,size_navi,i,0);
					
					report_navi_giornaliero.x = -1;
					report_navi_giornaliero.y = -1;
					report_navi_giornaliero.stato = 0;/*-> mare */
					
					
					/*se la durata dello spostamento è maggiore della durata della merce*/
					if(durata_spostamento>report_navi_giornaliero.durata_merce_nave){
						
						report_navi_giornaliero.merce_scaduta_nave=
						report_navi_giornaliero.merce_scaduta_nave +
						report_navi_giornaliero.quantita_merce_nave;
						
						/*report_navi_giornaliero.tipo_merce_nave=0;*/
						report_navi_giornaliero.quantita_merce_nave=0;
						report_navi_giornaliero.durata_merce_nave=0;
						
						
						printf("===== Merce scaduta nella nave %d durante lo spostamento ==== \n",i);
						
					
					}
					
					msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
					
					/*tempo per raggiungere il porto dove scaricare le merci*/	
					
							
					my_time.tv_sec = parte_intera;
					my_time.tv_nsec = parte_decimale;
						
					nanosleep(&my_time,NULL);
						
					/*leggo il messaggio e lo consumo*/	
					msgrcv(id_report_giornaliero,
					&report_navi_giornaliero,size_navi,i,0);
						
					/*imposto le coordinate del porto che ho raggiunto 
					e lo stato della nave con porto*/
					report_navi_giornaliero.x = x_porto_min;
					report_navi_giornaliero.y = y_porto_min;
					report_navi_giornaliero.stato = 1;/*-> porto */
						
					/*riscrivo il messaggio che ho appena consumato*/
					msgsnd(id_report_giornaliero,&report_navi_giornaliero,size_navi,0);
					
	
				}
			}
			
		}
		
	}
	
	exit(0);
	
	return 0;

}

double distanza(double x1, double y1, double x2, double y2){
	ris = sqrt(((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1)));	
	return ris;
}
