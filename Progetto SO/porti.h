#include <errno.h>
#include <unistd.h>

int velocita_scaricamento;
	
int num_banchine;
	
int domanda_tipo_merce;
int domanda_quantita_merce;
int domanda_durata_merce;
	
int offerta_tipo_merce;
int offerta_quantita_merce;
int offerta_durata_merce;
	
double porto_coord_x;
double porto_coord_y;

int id_report_giornaliero_porti;
int sem_id_report_giornaliero_porti;
int j;

int id_report_giornaliero_navi;
	
int id_semaforo_porti_banchine;

/*main*/
int size_porti;
int size_navi;
