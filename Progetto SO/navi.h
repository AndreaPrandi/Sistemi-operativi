#include <errno.h>
#include <unistd.h>

double distanza(double x1, double y1, double x2, double y2);

int velocita;
int capacita;
double coord_x;
double coord_y;
int id_report_giornaliero;
int i;
int sem_id_report_giornaliero;
int id_report_giornaliero_porti;
	
int id_semaforo_porti_banchine;

/* Main */
int size_navi;
int size_porti;

double x_nave;
double y_nave;
double x_porto;
double y_porto;
double x_porto_min;
double y_porto_min;
double min_dist;
int t;
		
int var_nave_sul_porto;
int id_porto_nave;/*id del porto dove si trova la nave*/

int carico;

int temp;

double durata_spostamento;
				
				
double parte_intera;
double parte_decimale;

int scarico;


/* Distanza */
double ris;
