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
#include "master.h"


int main() {
	
	leggi_variabili();
	
	/*Per inizializzare gli array leggendo la dimensione da file*/
	tot_merci_disponibili=(int*)malloc(sizeof(int)*(SO_MERCI+1));
	tot_merci_consegnate=(int*)malloc(sizeof(int)*(SO_MERCI+1));
	durata_merci=(int*)malloc(sizeof(int)*(SO_MERCI+1));
	merci_disponibili=(int*)malloc(sizeof(int)*(SO_PORTI+1));
	merci_inviate=(int*)malloc(sizeof(int)*(SO_PORTI+1));
	merci_ricevute=(int*)malloc(sizeof(int)*(SO_PORTI+1));
	merci_consegnate=(int*)malloc(sizeof(int)*(SO_MERCI+1));
	merce_tot_generata_porti=(int*)malloc(sizeof(int)*(SO_MERCI+1));
	tot_scaduta_nave=(int*)malloc(sizeof(int)*(SO_MERCI+1));
	tot_scaduta_porto=(int*)malloc(sizeof(int)*(SO_MERCI+1));
	tot_merci_ricevute=(int*)malloc(sizeof(int)*(SO_PORTI+1));
	tot_merci_inviate=(int*)malloc(sizeof(int)*(SO_PORTI+1));
	merce_contenuta_nave=(int*)malloc(sizeof(int)*(SO_MERCI+1));
	tot_merci_scadute_nave=(int*)malloc(sizeof(int)*(SO_MERCI+1));
	tot_merci_scadute_porto=(int*)malloc(sizeof(int)*(SO_MERCI+1));
	num_banchine_usate=(int*)malloc(sizeof(int)*(SO_PORTI+1));
	num_banchine_totale=(int*)malloc(sizeof(int)*(SO_PORTI+1));
	coord_porti_x= (int*)malloc(sizeof(int)*(SO_PORTI+1));
	coord_porti_y=(int*)malloc(sizeof(int)*(SO_PORTI+1));
	durata_merci=(int*)malloc(sizeof(int)*(SO_MERCI+1));
	
	/* Inizialmente elimino il file di report per poterlo successivamente riscrivere */
	if(remove("report.txt")==0){
		printf("File eliminato correttamente\n");
	}else{
		printf("File NON eliminato\n");
	}
	

	fptr=fopen("report.txt","a");
	
	fprintf(fptr,"###############################################\n");
	fprintf(fptr,"PROGETTO DI SIMULAZIONI DI UN TRAFFICO DI NAVII\n");
	fprintf(fptr,"###############################################\n\n");
	
	printf("###############################################\n");
	printf("PROGETTO DI SIMULAZIONI DI UN TRAFFICO DI NAVII\n");
	printf("###############################################\n\n");
	
	
	bzero(&sa,sizeof(sa));
	
	sa.sa_handler = handle_signal;
	sigaction(SIGINT,&sa,NULL);
	
	/*Creo una coda di messaggi*/
	id_rep_gior = msgget(IPC_PRIVATE,IPC_CREAT|IPC_EXCL|0600);
	TEST_ERROR;
	
	id_rep_gior_porti = msgget(IPC_PRIVATE,IPC_CREAT|IPC_EXCL|0600);
	TEST_ERROR;
	
	/*Creo una coda di semafori*/
	id_sem_rep_gior = semget(IPC_PRIVATE,SO_NAVI+1,0600);
	TEST_ERROR;
	
	id_sem_rep_gior_porti = semget(IPC_PRIVATE,SO_PORTI+1,0600);
	TEST_ERROR;
	
	id_semaforo_porti_banchine = semget(IPC_PRIVATE,SO_PORTI+1,0600);
	TEST_ERROR;
	
	/*creo i semafori per la creazione di navi e porti e li setto */
	id_semaforo_creazione_navi=semget(IPC_PRIVATE,1,0600);
	TEST_ERROR;
	sem_set_val(id_semaforo_creazione_navi,0,1);
	
	id_semaforo_creazione_porti=semget(IPC_PRIVATE,1,0600);
	TEST_ERROR;
	sem_set_val(id_semaforo_creazione_porti,0,1);
	
	/*TODO da eliminare*/
	sem_reserve(id_semaforo_creazione_navi,0);
	sem_release(id_semaforo_creazione_navi,0);
	
	
	/*creo tutti i semafori e li setto a 0*/
	for(j=0;j<=SO_NAVI;j++){
		sem_set_val(id_sem_rep_gior,j,0);
	}
	
	for(j=0;j<=SO_PORTI;j++){
		sem_set_val(id_sem_rep_gior_porti,j,0);
		
	}

	srand(time(NULL));
	
	/*------------------------------------*/
	/*Creo le navi e i porti*/
	/*------------------------------------*/
	
	generaDurateCasuali();
    	createPorti();
	createNavi();
	
	/*
	struct timespec my_time00;
			
	my_time00.tv_sec = 5;
	my_time00.tv_nsec = 000000000;
	nanosleep(&my_time00,NULL);
	*/
	
	/*Semafori per aspettare la terminazione della creazione delle navi e dei porti*/
	sem_reserve(id_semaforo_creazione_navi,0);
	sem_reserve(id_semaforo_creazione_porti,0);
	
	/*------------------------------------*/
	/*Creo e setto i semafori per le banchine*/
	/*------------------------------------*/
	
	for(k=1;k<=SO_PORTI;k++){
		kk=(long)k;
		size=sizeof(report_porti_temp)-sizeof(long);
			
		msgrcv(id_rep_gior_porti,&report_porti_temp,size,kk,0);
		msgsnd(id_rep_gior_porti,&report_porti_temp,size,0);
		
		
		num_banchine=report_porti_temp.num_banchine;
		
		sem_set_val(id_semaforo_porti_banchine,k,num_banchine);
		
	}
	
	dim_lotti=1+(rand()%SO_SIZE);
	
	printf("\n\n");
	printf("#######################\n");
	printf("INIZIO SIMULAZIONE\n");
	printf("#######################\n\n");
	
	fprintf(fptr,"\n\n");
	fprintf(fptr,"#######################\n");
	fprintf(fptr,"INIZIO SIMULAZIONE\n");
	fprintf(fptr,"#######################\n\n");
	
	printf("--------------------\n");
	printf("MERCI\n");
	printf("--------------------\n");
	for(p=1;p<=SO_MERCI;p++){
		printf("MERCE %d durata:%d \n",p,durata_merci[p]);
	}
	
	fprintf(fptr,"--------------------\n");
	fprintf(fptr,"MERCI\n");
	fprintf(fptr,"--------------------\n");
	for(p=1;p<=SO_MERCI;p++){
		fprintf(fptr,"MERCE %d durata:%d \n",p,durata_merci[p]);
	}
	
	
	printf("--------------------\n");
	printf("PORTI\n");
	printf("--------------------\n");
	
	fprintf(fptr,"--------------------\n");
	fprintf(fptr,"PORTI\n");
	fprintf(fptr,"--------------------\n");

	printf("Velocità di caricamento/scaricamento: %d \n",SO_LOADSPEED);
	printf("Dimensione lotti: %d \n\n",dim_lotti);

	fprintf(fptr,"Velocità di caricamento/scaricamento: %d \n",SO_LOADSPEED);
	fprintf(fptr,"Dimensione lotti: %d \n\n",dim_lotti);

	for(k=1;k<=SO_PORTI;k++){
		/*kk -> mytype*/
		kk=(long)k;
		/* tolgo la lunghezza del long poichè non deve essere contata */
		size=sizeof(report_porti_temp)-sizeof(long);
		
		/*non uso IPC_NOWAIT|MSG_COPY altrimenti mi 
		da errore poichè non aspetta i messsaggi*/
		
		/*leggo il messaggio e lo consumo*/	
		msgrcv(id_rep_gior_porti,&report_porti_temp,size,kk,0);
		
		/*riscrivo il messaggio che ho appena consumato*/
		msgsnd(id_rep_gior_porti,&report_porti_temp,size,0);
		
		fprintf(fptr,"%ld)PORTO (pid:%d) coord:(%f,%f) num_b:%d domanda(t:%d  q:%d  d:%d) offerta(t:%d  q:%d  d:%d)\n",
			report_porti_temp.mytype,report_porti_temp.pid, 
			report_porti_temp.x, report_porti_temp.y,
			report_porti_temp.num_banchine,
			report_porti_temp.domanda_tipo_merce,
			report_porti_temp.domanda_quantita_merce,
			report_porti_temp.domanda_durata_merce,
			report_porti_temp.offerta_tipo_merce,
			report_porti_temp.offerta_quantita_merce,
			report_porti_temp.offerta_durata_merce);
		
		printf("%ld)PORTO (pid:%d) coord:(%f,%f) num_b:%d domanda(t:%d  q:%d  d:%d) offerta(t:%d  q:%d  d:%d)\n",
			report_porti_temp.mytype,report_porti_temp.pid, 
			report_porti_temp.x, report_porti_temp.y,
			report_porti_temp.num_banchine,
			report_porti_temp.domanda_tipo_merce,
			report_porti_temp.domanda_quantita_merce,
			report_porti_temp.domanda_durata_merce,
			report_porti_temp.offerta_tipo_merce,
			report_porti_temp.offerta_quantita_merce,
			report_porti_temp.offerta_durata_merce);
		
	}
	printf("\n");
	printf("--------------------\n");
	printf("NAVI\n");
	printf("--------------------\n");
	
	fprintf(fptr,"\n");
	fprintf(fptr,"--------------------\n");
	fprintf(fptr,"NAVI\n");
	fprintf(fptr,"--------------------\n");
	
	for(k=1;k<=SO_NAVI;k++){
		/*kk -> mytype*/
		kk=(long)k;
			
		/* tolgo la lunghezza del long poichè non deve essere contata */
		size=sizeof(report_navi_temp)-sizeof(long);
		
		/*leggo la lista delle navi*/	
		msgrcv(id_rep_gior,&report_navi_temp,size,kk,0);
		
		/*riscrivo la lista delle navi*/
		msgsnd(id_rep_gior,&report_navi_temp,size,0);
		
		fprintf(fptr,"%ld)NAVE (pid:%d) coord:(%f,%f) merce(t:%d  q:%d  d:%d) stato:%d\n",
			report_navi_temp.mytype,
			report_navi_temp.pid, 
			report_navi_temp.x, 
			report_navi_temp.y,
			report_navi_temp.tipo_merce_nave,
			report_navi_temp.quantita_merce_nave,
			report_navi_temp.durata_merce_nave,
			report_navi_temp.stato);
		
		printf("%ld)NAVE (pid:%d) coord:(%f,%f) merce(t:%d  q:%d  d:%d) stato:%d\n",
			report_navi_temp.mytype,
			report_navi_temp.pid, 
			report_navi_temp.x, 
			report_navi_temp.y,
			report_navi_temp.tipo_merce_nave,
			report_navi_temp.quantita_merce_nave,
			report_navi_temp.durata_merce_nave,
			report_navi_temp.stato);

	}
	
	printf("\n\n*****************************\n");
	printf("REPORT GIORNALIERO\n");
	printf("*****************************\n");
	
	fprintf(fptr,"\n\n*****************************\n");
	fprintf(fptr,"REPORT GIORNALIERO\n");
	fprintf(fptr,"*****************************\n");
	
	fclose(fptr);
	
	/*Azzero le variabili per il report finale*/
	for(g=1;g<=SO_PORTI;g++){
		tot_merci_ricevute[g]=0;
		tot_merci_inviate[g]=0;
		
	}
	for(g=1;g<=SO_MERCI;g++){
		merce_tot_generata_porti[g]=0;
		merci_consegnate[g]=0;
		tot_scaduta_porto[g]=0;
		tot_scaduta_nave[g]=0;
	}
	
	porto_max_offerta=0;
	porto_max_domanda=0;
	max_domanda=0;
	max_offerta=0;
	
	/*Calcolo il tot di merci generata per ogni porto*/
	for(k=1;k<=SO_PORTI;k++){
		kk=(long)k;
		size=sizeof(report_porti_temp)-sizeof(long);	
		msgrcv(id_rep_gior_porti,&report_porti_temp,size,kk,0);
		msgsnd(id_rep_gior_porti,&report_porti_temp,size,0);
		
		if(max_domanda<report_porti_temp.domanda_quantita_merce){
			max_domanda=report_porti_temp.domanda_quantita_merce;
			porto_max_domanda=k;
		}
		if(max_offerta<report_porti_temp.domanda_quantita_merce){
			max_offerta=report_porti_temp.offerta_quantita_merce;
			porto_max_offerta=k;
		}
		
		for(g=1;g<=SO_MERCI;g++){
			if(report_porti_temp.offerta_tipo_merce==g){
				merce_tot_generata_porti[g]=merce_tot_generata_porti[g]+
				report_porti_temp.offerta_quantita_merce;
			}
			
		}
		
	}
		
	
	
	/*------------------------------------------------------------------------------*/
	/*------------------------------------------------------------------------------*/
	/*INIZIO simulazione 30 giorni*/
	/*------------------------------------------------------------------------------*/
	/*------------------------------------------------------------------------------*/
	giorno=0;
	while(giorno<=SO_DAYS){
		fptr=fopen("report.txt","a");
		
		
		printf("\n\n");
		printf("#######################\n");
		printf("GIORNO %d\n",giorno);
		printf("#######################\n");
		
		
		quantita_merci_navi=0;
		
		/*Scrivo tutte le navi*/
		for(k=1;k<=SO_NAVI;k++){
			
			/*kk -> mytype*/
			kk=(long)k;
				
			/* tolgo la lunghezza del long poichè non deve essere contata */
			size=sizeof(report_navi_temp)-sizeof(long);
			
			
			
			/*leggo la lista delle navi*/	
			msgrcv(id_rep_gior,&report_navi_temp,size,kk,0);
			
			/*merce totale su tutte le navi*/
			quantita_merci_navi=quantita_merci_navi+report_navi_temp.quantita_merce_nave;
			
			/*diminuisco la data di scadenza dei prodotti*/
			if(report_navi_temp.durata_merce_nave>0){
				report_navi_temp.durata_merce_nave = 
				report_navi_temp.durata_merce_nave - 1;
			
			}
			
			/*riscrivo la lista delle navi*/
			msgsnd(id_rep_gior,&report_navi_temp,size,0);
			
		printf("%ld)NAVE (pid:%d) coord:(%f,%f) merce(t:%d-q:%d-d:%d) stato:%d\n",
			report_navi_temp.mytype,
			report_navi_temp.pid, 
			report_navi_temp.x, 
			report_navi_temp.y,
			report_navi_temp.tipo_merce_nave,
			report_navi_temp.quantita_merce_nave,
			report_navi_temp.durata_merce_nave,
			report_navi_temp.stato);
		}
		
		printf("--------------------\n");
		
		var_terminate_merci_domanda=0;
		var_terminate_merci_offerta=0;
		
		/* Scrivo i porti ogni giorno */
		for(g=1;g<=SO_PORTI;g++){
			/*kk -> mytype*/
			gg=(long)g;

			size=sizeof(report_porti_temp)-sizeof(long);
				
			/*leggo il messaggio e lo consumo*/	
			msgrcv(id_rep_gior_porti,&report_porti_temp,size,gg,0);
			
			var_terminate_merci_domanda=var_terminate_merci_domanda +
			report_porti_temp.domanda_quantita_merce;
			
			var_terminate_merci_offerta=var_terminate_merci_offerta +
			report_porti_temp.offerta_quantita_merce;
			
			/*diminuisco la data di scadenza dei prodotti*/
			if(report_porti_temp.domanda_durata_merce>0 && giorno!=0 ){
				report_porti_temp.domanda_durata_merce = 
				report_porti_temp.domanda_durata_merce-1;
			}
			if(report_porti_temp.offerta_durata_merce>0  && giorno!=0){
				report_porti_temp.offerta_durata_merce =
				 report_porti_temp.offerta_durata_merce-1;
			
			}
			
			/*riscrivo il messaggio che ho appena consumato*/
			msgsnd(id_rep_gior_porti,&report_porti_temp,size,0);
			
		printf("%ld)PORTO (pid:%d) coord:(%f,%f) num_b:%d domanda(t:%d q:%d d:%d) offerta(t:%d q:%d d:%d)\n",
			report_porti_temp.mytype,report_porti_temp.pid, 
			report_porti_temp.x, report_porti_temp.y,
			report_porti_temp.num_banchine,
			report_porti_temp.domanda_tipo_merce,
			report_porti_temp.domanda_quantita_merce,
			report_porti_temp.domanda_durata_merce,
			report_porti_temp.offerta_tipo_merce,
			report_porti_temp.offerta_quantita_merce,
			report_porti_temp.offerta_durata_merce);
			
		}
		
		printf("-------------------------------\n");
		
		/*----------------------------------*/
		/*Reset Variabili per il report Giornaliero*/
		/*----------------------------------*/

		
		for(l=0;l<=SO_MERCI;l++){
			tot_merci_disponibili[l]=0;
			tot_merci_consegnate[l]=0;
			merce_contenuta_nave[l]=0;
			
			tot_merci_scadute_porto[l]=0;
			tot_merci_scadute_nave[l]=0;
		}
		
		num_navi_mare_c=0;/*cariche*/
		num_navi_mare_v=0;/*vuote*/
		num_navi_porto=0;
		
		/*azzero le variabili per il report giornaliero*/
		for(p=1;p<=SO_PORTI;p++){
			merci_disponibili[p]=0;
			merci_ricevute[p]=0;
			merci_inviate[p]=0;
		}
		
		
		/*Per ogni merce*/
		for(h=1;h<=SO_MERCI;h++){
		
			for(k=1;k<=SO_NAVI;k++){
				kk=(long)k;
				
				size=sizeof(report_navi_temp)-sizeof(long);

				msgrcv(id_rep_gior,&report_navi_temp,size,kk,0);
				msgsnd(id_rep_gior,&report_navi_temp,size,0);
				

				if(report_navi_temp.tipo_merce_nave==h){
				
					/*report giornaliero*/
					tot_merci_scadute_nave[h] = tot_merci_scadute_nave[h] +
					report_navi_temp.merce_scaduta_nave;
					
					/*report totale*/
					tot_scaduta_nave[h]=tot_scaduta_nave[h] +
					report_navi_temp.merce_scaduta_nave;
				}
				
				
				if(report_navi_temp.tipo_merce_nave==h){
					merce_contenuta_nave[h]=merce_contenuta_nave[h]+
					report_navi_temp.quantita_merce_nave;
				}
				
			}
			
			for(g=1;g<=SO_PORTI;g++){
				gg=(long)g;
				
				size=sizeof(report_porti_temp)-sizeof(long);

				msgrcv(id_rep_gior_porti,&report_porti_temp,size,gg,0);
				
				if(report_porti_temp.offerta_tipo_merce==h){
					/*report giornaliero*/
					tot_merci_scadute_porto[h]=tot_merci_scadute_porto[h]+
					report_porti_temp.merce_scaduta_porto;
					
					/*report totale*/
					tot_scaduta_porto[h]=tot_scaduta_porto[h]+
					report_porti_temp.merce_scaduta_porto;
					
				}
				
				
				
				msgsnd(id_rep_gior_porti,&report_porti_temp,size,0);
				
			}
		}
		
		/*Analizzo le NAVI*/
		for(k=1;k<=SO_NAVI;k++){
			
			kk=(long)k;
			size=sizeof(report_navi_temp)-sizeof(long);

			msgrcv(id_rep_gior,&report_navi_temp,size,kk,0);
			msgsnd(id_rep_gior,&report_navi_temp,size,0);
			
			
			
			if(report_navi_temp.stato==0){/*in mare*/
				
				if(report_navi_temp.quantita_merce_nave>0){/*cariche*/
					num_navi_mare_c= num_navi_mare_c+1;;
				}
				else{
					num_navi_mare_v = num_navi_mare_v +1;;
				}
			
			}
			else if(report_navi_temp.stato==1){/*in porto*/
				num_navi_porto = num_navi_porto+1;
			}
			
			
		}
		
		
		
		
		
		
		/* Analizzo i porti */
		for(g=1;g<=SO_PORTI;g++){
			gg=(long)g;
			size=sizeof(report_porti_temp)-sizeof(long);

			msgrcv(id_rep_gior_porti,&report_porti_temp,size,gg,0);
			msgsnd(id_rep_gior_porti,&report_porti_temp,size,0);
			
			/* per ogni merce sommo la merce totale offerta e richiesta */
			
			for(h=1;h<=SO_MERCI;h++){
				if(report_porti_temp.offerta_tipo_merce==h){
					tot_merci_disponibili[h] = tot_merci_disponibili[h] + 
					report_porti_temp.offerta_quantita_merce;
					}
				if(report_porti_temp.domanda_tipo_merce==h){
					tot_merci_consegnate[h]= tot_merci_consegnate[h] +
					report_porti_temp.domanda_quantita_merce;
				}
				if(report_porti_temp.domanda_tipo_merce==h){
					merci_consegnate[h]=merci_consegnate[h]+
					report_porti_temp.merci_ricevute;
				}
			}
			
			merci_disponibili[g]=report_porti_temp.offerta_quantita_merce;
			merci_ricevute[g]=report_porti_temp.merci_ricevute;
			merci_inviate[g]=report_porti_temp.merci_inviate;

			num_banchine_totale[g]=report_porti_temp.num_banchine;
			num_banchine_usate[g]=report_porti_temp.num_banchine_usate;
			
		}
		
		
		fprintf(fptr,"\n");
		fprintf(fptr,"#######################\n");
		fprintf(fptr,"REPORT giorno %d\n",giorno);
		fprintf(fptr,"#######################\n");
		
		
		for(v=1;v<=SO_MERCI;v++){
			fprintf(fptr," -Merce (%d): Contenuta dai porti: %d  Richiesta dai porti:%d \n",
			v,tot_merci_disponibili[v],tot_merci_consegnate[v]);
			fprintf(fptr,"              Contenuta in nave: %d  Scaduta in porto:%d  Scaduta in nave:%d \n",
			merce_contenuta_nave[v],tot_merci_scadute_porto[v],tot_merci_scadute_nave[v]);
			
			
		}
		fprintf(fptr,"\n");
		

		
		fprintf(fptr," -Numero di navi in mare con carico:%d\n",num_navi_mare_c);
		fprintf(fptr," -Numero di navi in mare senza carico:%d\n",num_navi_mare_v);
		fprintf(fptr," -Numero di navi in porto:%d \n\n",num_navi_porto);
		
		fprintf(fptr,"\n");
		
		
		for(v=1;v<=SO_PORTI;v++){
			
			fprintf(fptr," -Porto (%d) Merce:(presente:%d spedita:%d ricevuta:%d) Num_banchine_occupate(%d/%d)\n"
			,v,merci_disponibili[v],merci_inviate[v],merci_ricevute[v],
			num_banchine_usate[v],num_banchine_totale[v]
			);
			
		}
		
		
		
		fprintf(fptr,"#####################\n\n");
		
		/*Incremento le variabili per il report totale*/
		for(v=1;v<=SO_PORTI;v++){
			tot_merci_ricevute[v]=tot_merci_ricevute[v] + merci_ricevute[v];
			tot_merci_inviate[v]=tot_merci_inviate[v] + merci_inviate[v];
	
		}
			
		
		/*Azzero le variabili per il report giornaliero*/
		for(g=1;g<=SO_PORTI ;g++){
			gg=(long)g;
			size=sizeof(report_porti_temp)-sizeof(long);

			msgrcv(id_rep_gior_porti,&report_porti_temp,size,gg,0);
			
			report_porti_temp.merci_ricevute=0;
			report_porti_temp.merci_inviate=0;
			
			report_porti_temp.merce_scaduta_porto=0;
			
			
			msgsnd(id_rep_gior_porti,&report_porti_temp,size,0);
			
		}
		
		/*Azzero le variabili per il report giornaliero*/
		for(k=1;k<=SO_NAVI ;k++){
			
			kk=(long)k;
			size=sizeof(report_navi_temp)-sizeof(long);

			msgrcv(id_rep_gior,&report_navi_temp,size,kk,0);
			
			report_navi_temp.merce_scaduta_nave=0;
			
			msgsnd(id_rep_gior,&report_navi_temp,size,0);
		}
		
		/*Rilascio i semafori*/
		for(k=1;k<=SO_NAVI;k++){
			sem_release(id_sem_rep_gior,k);
		}
		for(k=1;k<=SO_PORTI;k++){
			sem_release(id_sem_rep_gior_porti,k);
		}
		
			
		fclose(fptr);
		
		
		/* Durata di un giorno */	
		my_time1.tv_sec = 1;
		my_time1.tv_nsec = 000000000;
		nanosleep(&my_time1,NULL);
		
		fptr=fopen("report.txt","a");
		
		if(var_terminate_merci_domanda==0 ){
			printf("\n\n======= DOMANDA MERCI TERMINATA ======  \n\n");
			fprintf(fptr,"\n\n======= DOMANDA MERCI TERMINATA ======  \n\n");
			
			break;
		}
		if(var_terminate_merci_offerta==0 && quantita_merci_navi==0){
			printf("\n\n======= OFFERTA MERCI TERMINATA ======  \n\n");
			fprintf(fptr,"\n\n======= OFFERTA MERCI TERMINATA ======  \n\n");
			
			break;
		}
		fclose(fptr);
		
		giorno++;
		
	
	}
	
	fptr=fopen("report.txt","a");
	
	printf("\n#############################\n");
	printf("Fine simulazione\n");
	printf("#############################\n\n");
	
	/*--------------------------*/
	/*REPORT FINALE*/
	/*--------------------------*/
	fprintf(fptr,"\n#############################\n");
	fprintf(fptr,"REPORT FINALE\n");
	fprintf(fptr,"#############################\n\n");
	
	fprintf(fptr," -NAVI: -ancora in mare con carico:%d\n",num_navi_mare_c);
	fprintf(fptr,"        -ancora in mare senza carico:%d\n",num_navi_mare_v);
	fprintf(fptr,"        -ancora in porto:%d \n\n",num_navi_porto);
	
  	
	for(v=1;v<=SO_MERCI;v++){
		fprintf(fptr," -MERCE (%d): Contenuta nei porti: %d  Richiesta dai porti:%d Contenuta in nave: %d \n",
		v,tot_merci_disponibili[v],tot_merci_consegnate[v],merce_contenuta_nave[v]);
				
	}
	fprintf(fptr,"\n");
	
	for(v=1;v<=SO_PORTI;v++){
			
		fprintf(fptr," -Porto (%d) Merce:(presente:%d)  tot_spedita:%d tot_ricevuta:%d) \n"
			,v,merci_disponibili[v],tot_merci_inviate[v],tot_merci_ricevute[v]);

			
	}
	fprintf(fptr,"\n");
	
	for(v=1;v<=SO_MERCI;v++){
		fprintf(fptr," -MERCE (%d): -Tot generata nei porti:%d  Rimasta ferma in porto:%d  \n"
		,v,merce_tot_generata_porti[v],tot_merci_disponibili[v]);
		
		fprintf(fptr," 		 -Scaduta in porto:%d Scaduta in nave:%d Consegnata: %d  \n"
		,tot_scaduta_porto[v],tot_scaduta_nave[v],
		merci_consegnate[v]);
				
	}
	
	/*
	for(v=1;v<=SO_MERCI;v++){
		fprintf(fptr," -MERCE (%d): -Tot generata nei porti:%d  Rimasta ferma in porto:%d  \n"
		,v,merce_tot_generata_porti[v],tot_merci_disponibili[v]);
		
		fprintf(fptr," 		 -Scaduta in porto:%d Scaduta in nave:%d Consegnata: %d  \n"
		,tot_scaduta_porto[v],tot_scaduta_nave[v],
		merce_tot_generata_porti[v]-tot_merci_disponibili[v]-tot_scaduta_porto[v]-tot_scaduta_nave[v]);
				
	}
	*/
	fprintf(fptr,"\n");
	
	fprintf(fptr,"Il Porto %d ha richiesto la quantità maggiore di merce:%d\n",
	porto_max_offerta,max_offerta);
	fprintf(fptr,"Il Porto %d ha offerto la quantità maggiore di merce:%d\n",
	porto_max_domanda,max_domanda);
	
	
	
	fprintf(fptr,"\n#############################\n");
	fprintf(fptr,"Fine simulazione\n");
	fprintf(fptr,"#############################\n\n");
	
	fclose(fptr);
	
	
	/*Termina tutti i processi navi e porto*/
	for (i=1; i<=SO_NAVI; i++) {
		kill(all_kids_navi[i], SIGINT);
	}
	for (i=1; i<=SO_PORTI; i++) {
		kill(all_kids_porti[i], SIGINT);
	}
	kill(getpid(),SIGINT);
	
	

}

void handle_signal(int signal){
	
	printf("Ho ricevuto il segnale ctrl+c \n");
	
	/*Libero la memoria dalla la coda di messaggi*/
	a = msgctl(id_rep_gior, IPC_RMID, NULL);
	a_p = msgctl(id_rep_gior_porti, IPC_RMID, NULL);
	
	if(a==0 && a_p==0){
		printf("Coda %d - %d eliminata con SUCCESSO\n",id_rep_gior, id_rep_gior_porti);
	}
	else{
		printf("Coda di messaggi NON eliminata correttamente\n");
	}
	
	/*Libero la memoria dai semafori*/
	b = semctl(id_sem_rep_gior,0,IPC_RMID);
	b_p = semctl(id_sem_rep_gior_porti,0,IPC_RMID);
	b2=  semctl(id_semaforo_creazione_navi,0,IPC_RMID);
	b3=  semctl(id_semaforo_creazione_porti,0,IPC_RMID);
	b1 = semctl(id_semaforo_porti_banchine,0,IPC_RMID);
	
	if(b==0 && b_p==0 && b1==0 && b2==0 && b3==0){
		printf("Semaforo %d - %d - %d - eliminato con SUCCESSO\n",
		id_sem_rep_gior,id_sem_rep_gior_porti,id_semaforo_porti_banchine);
	}
	else{
		printf("Semafori NON eliminati correttamente\n");
	}
	
	exit(0);
	
	
}

void createNavi(){
	char * args_navi[] = {"./navi",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	
	sem_reserve(id_semaforo_creazione_navi,0);

	all_kids_navi = malloc((SO_NAVI+1)*sizeof(* all_kids_navi));

	/* Calcolo massimo numero di caratteri per rappresentare un unsigned long in una stringa */
	max_size = ceil(log10(ULONG_MAX)) + 1;/*TODO da controllare*/
	
	
	str_speed = malloc (max_size);
	str_capacity = malloc (max_size);
	str_id_report_giornaliero = malloc (max_size);
	str_sem_id_report_giornaliero = malloc (max_size);
	str_id_report_giornaliero_porti = malloc (max_size);

	args_navi[1] = str_speed; 
	args_navi[2] = str_capacity; 
	args_navi[5] = str_id_report_giornaliero;
	args_navi[7] = str_sem_id_report_giornaliero;
	args_navi[8] = str_id_report_giornaliero_porti;
	
	sprintf(str_speed,"%d",SO_SPEED);
	sprintf(str_capacity,"%d",SO_CAPACITY);
	sprintf(str_id_report_giornaliero,"%d",id_rep_gior);
	sprintf(str_sem_id_report_giornaliero,"%d",id_sem_rep_gior);
	sprintf(str_id_report_giornaliero_porti,"%d",id_rep_gior_porti);
	

	for(i=1;i<=SO_NAVI;i++){
	
		switch(all_kids_navi[i]=fork()){
			case -1:
			
				break;
			/*figlio*/
			case 0:
				srand(getpid());
				
				lato = (int)SO_LATO;
				x=rand()%lato;
				y=rand()%lato;
				
				str_coor_x = malloc (max_size);
				str_coor_y = malloc (max_size);
				str_index = malloc (max_size);
				
				str_id_semaforo_porti_banchine = malloc (max_size);
				
				args_navi[3] = str_coor_x ;
				args_navi[4] = str_coor_y ;
				args_navi[6] = str_index;
				args_navi[9] = str_id_semaforo_porti_banchine;
				
				sprintf(str_coor_x,"%d",x);
				sprintf(str_coor_y,"%d",y);
				sprintf(str_index,"%d",i);
				
				sprintf(str_id_semaforo_porti_banchine,"%d",
				id_semaforo_porti_banchine);
				
				
				execve("./navi", args_navi,NULL);
				
				exit(1);
				break;
			
		}
	}
	sem_release(id_semaforo_creazione_navi,0);
	

}

void createPorti(){
	
	
	char * args_porti[] = {"./porti",NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	
	int velocita_scaricamento;
	
	int num_banchine;
	
	int domanda_tipo_merce;
	int domanda_quantita_merce;
	int domanda_durata_merce;
	
	int offerta_tipo_merce;
	int offerta_quantita_merce;
	int offerta_durata_merce;
	
	int porto_coord_x;
	int porto_coord_y;
	
	sem_reserve(id_semaforo_creazione_porti,0);

	all_kids_porti = malloc((SO_PORTI+1)*sizeof(* all_kids_porti));
	
	max_size = ceil(log10(ULONG_MAX)) + 1;
	

	str_velocita_scaricamento = malloc (max_size);
	str_domanda_quantita_merce = malloc (max_size);
	str_offerta_quantita_merce = malloc (max_size);
	
	str_id_report_giornaliero_porti = malloc (max_size);
	str_sem_id_report_giornaliero_porti = malloc (max_size);
	
	str_id_report_giornaliero_navi = malloc (max_size);
	
	args_porti[1] = str_velocita_scaricamento;
	args_porti[2] = str_domanda_quantita_merce; 
	args_porti[3] = str_offerta_quantita_merce;  
	
	args_porti[11] = str_id_report_giornaliero_porti;
	args_porti[12] = str_sem_id_report_giornaliero_porti;
	args_porti[14] = str_id_report_giornaliero_navi;
	
	
	sprintf(str_velocita_scaricamento,"%d",SO_LOADSPEED);
	sprintf(str_domanda_quantita_merce,"%d",SO_FILL);
	sprintf(str_offerta_quantita_merce,"%d",SO_FILL);
	
	sprintf(str_id_report_giornaliero_porti,"%d",id_rep_gior_porti);
	sprintf(str_sem_id_report_giornaliero_porti,"%d",id_sem_rep_gior_porti);
	
	sprintf(str_id_report_giornaliero_navi,"%d",id_rep_gior);
	
	
	for(j=1;j<=SO_PORTI;j++){
	
		switch(all_kids_porti[j]=fork()){
			case -1:
				break;
			/*figlio*/
			case 0:
				/*printf("Creazione di un figlio (pid:%d) (%d)",getpid(),j);*/
				
				srand(getpid());
				
				str_num_banchine = malloc (max_size);
				
				str_domanda_tipo_merce = malloc (max_size);
				str_domanda_durata_merce = malloc (max_size);
				
				str_offerta_tipo_merce = malloc (max_size);
				str_offerta_durata_merce = malloc (max_size);
				
				str_porto_coord_x = malloc (max_size);
				str_porto_coord_y = malloc (max_size);
				
				str_index = malloc (max_size);
				
				str_id_semaforo_porti_banchine = malloc (max_size);
				
				args_porti[15] = str_id_semaforo_porti_banchine;
				
				sprintf(str_id_semaforo_porti_banchine
				,"%d",id_semaforo_porti_banchine);  
				
				args_porti[13] = str_index ;
				sprintf(str_index,"%d",j);
				
				if(SO_BANCHINE==1){
					num_banchine = 1;
				}
				else if(SO_BANCHINE>1){
					num_banchine = 1+(rand()%(SO_BANCHINE));
				}
				
				args_porti[4] = str_num_banchine ;
				sprintf(str_num_banchine,"%d",num_banchine);
				
				/*Domanda*/
				domanda_tipo_merce=1+(rand()%(SO_MERCI));
				domanda_durata_merce= durata_merci[domanda_tipo_merce];
				
				args_porti[5] = str_domanda_tipo_merce; 
				args_porti[6] = str_domanda_durata_merce;  
				
				sprintf(str_domanda_tipo_merce,"%d",domanda_tipo_merce);
				sprintf(str_domanda_durata_merce,"%d",domanda_durata_merce);
				
				srand(getpid()+domanda_tipo_merce);
				
				/*Offerta*/
				offerta_tipo_merce=1+(rand()%(SO_MERCI));
				
				while(domanda_tipo_merce==offerta_tipo_merce){
					srand(getpid()+rand());
					offerta_tipo_merce=(rand()%(SO_MERCI))+1;
				}
				offerta_durata_merce= durata_merci[offerta_tipo_merce];
				
				args_porti[7] = str_offerta_tipo_merce; 
				args_porti[8] = str_offerta_durata_merce;  
				
				sprintf(str_offerta_tipo_merce,"%d",offerta_tipo_merce);
				sprintf(str_offerta_durata_merce,"%d",offerta_durata_merce);
				

				
				lato = (int)SO_LATO;
				
				
				
				srand(getpid());
				if(j==1){
					porto_coord_x=0;
					porto_coord_y=0;
					coord_porti_x[j]=porto_coord_x;
					coord_porti_y[j]=porto_coord_y;
						
				}
				if(j==2){
					porto_coord_x=SO_LATO;
					porto_coord_y=SO_LATO;
					coord_porti_x[j]=porto_coord_x;
					coord_porti_y[j]=porto_coord_y;
					
				}
				if(j==3){
					porto_coord_x=0;
					porto_coord_y=SO_LATO;
					coord_porti_x[j]=porto_coord_x;
					coord_porti_y[j]=porto_coord_y;
				}
				if(j==4){
					porto_coord_x=SO_LATO;
					porto_coord_y=0;
					coord_porti_x[j]=porto_coord_x;
					coord_porti_y[j]=porto_coord_y;
				}
				if(j>4){
					
					h=0;
					while(h==0){
					
						porto_coord_x=rand()%lato;
						porto_coord_y=rand()%lato;
						
						h=1;
						
						for(yy=1;yy<=SO_PORTI && h==1;yy++){
						
							if(porto_coord_x==coord_porti_x[yy]&&
							porto_coord_y==coord_porti_y[yy]){
								h=0;
							}
							
						}
					}
					coord_porti_x[j]=porto_coord_x;
					coord_porti_y[j]=porto_coord_y;

				}
				args_porti[9] = str_porto_coord_x;
				args_porti[10] = str_porto_coord_y;
					
				sprintf(str_porto_coord_x,"%d",porto_coord_x);
				sprintf(str_porto_coord_y,"%d",porto_coord_y);
				
	
				execve("./porti", args_porti,NULL);
				
				exit(1);
				break;
			
		}
		
	}
	sem_release(id_semaforo_creazione_porti,0);
	
	
	
}
void generaDurateCasuali(){
	srand(time(NULL));
	
	
	for(q=1;q<=SO_MERCI;q++){
		srand(rand()+q);
		durata_merci[q]=(rand()%(SO_MAX_VITA-SO_MIN_VITA+1)) + SO_MIN_VITA;
	}

}









