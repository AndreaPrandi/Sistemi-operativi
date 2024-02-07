#include <errno.h>
#include <unistd.h>

/* ---------------- Prototipi ----------------- */
void createNavi();
void createPorti();
void generaDurateCasuali();


void handle_signal(int signal);

/* ---------------- Variabili globali ----------------- */


int id_rep_gior;
int id_sem_rep_gior;

int id_rep_gior_porti;
int id_sem_rep_gior_porti;
	
int id_semaforo_porti_banchine;

int id_semaforo_creazione_navi;
int id_semaforo_creazione_porti;

/*Variabili generali*/
int j;
int i;
int giorno;
int k;
int g;
long kk;
int size;
int a;
int b;
int a_p;
int b_p;
int b1;
int b2;
int v;
int h;
int num_banchine;
int dim_lotti;
int p;
int gg;
int h;
int l;
int q;
int h;
int lato;
int max_size;
int x;
int y;
int yy;
int b2;
int b3;



/*Creazione navi*/	
char * str_speed;
char * str_capacity;
char * str_id_report_giornaliero;
char * str_sem_id_report_giornaliero;
char * str_id_report_giornaliero_porti;

/*Creazione porti*/
char * str_coor_x;
char * str_coor_y;
char * str_index;
char * str_id_semaforo_porti_banchine;				
char * str_velocita_scaricamento;
char * str_domanda_quantita_merce;
char * str_offerta_quantita_merce;	
char * str_id_report_giornaliero_porti;
char * str_sem_id_report_giornaliero_porti;	
char * str_id_report_giornaliero_navi;
char * str_num_banchine;				
char * str_domanda_tipo_merce;
char * str_domanda_durata_merce;				
char * str_offerta_tipo_merce;
char * str_offerta_durata_merce;				
char * str_porto_coord_x;
char * str_porto_coord_y;				
char * str_index;				
char * str_id_semaforo_porti_banchine;


/*variabili per la terminazione*/
int quantita_merci_navi;
int var_terminate_merci_domanda;
int var_terminate_merci_offerta;
		

pid_t * all_kids_navi;
pid_t * all_kids_porti;

/*variabili per il report giornaliero*/
int num_navi_mare_c;
int num_navi_mare_v;
int num_navi_porto;

int *coord_porti_x;
int *coord_porti_y;
int *durata_merci;

int *tot_merci_disponibili;/* merci disponibili nei porti */
int *tot_merci_consegnate;
int *merci_disponibili;
int *merci_ricevute;
int *merci_inviate;


int *num_banchine_totale;
int *num_banchine_usate;

int *tot_merci_scadute_porto;
int *tot_merci_scadute_nave;
int *merce_contenuta_nave;

/*variabili per il report totale*/
int *tot_merci_ricevute;
int *tot_merci_inviate;

int *tot_scaduta_porto;
int *tot_scaduta_nave;

int *merce_tot_generata_porti;

int *merci_consegnate;



int porto_max_offerta;
int porto_max_domanda;
int max_domanda;
int max_offerta;

FILE *fptr;

struct sigaction sa;
struct timespec my_time1;
report_navi  report_navi_temp;
report_porti  report_porti_temp;
		

