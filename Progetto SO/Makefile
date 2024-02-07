# dal carattere '#' fino a fine riga, il testo dentro il Makefile e`
# un commento
#
# flags per la compilazione
CFLAGS = -std=c89 -Wpedantic 

# crea tutto
all:  master navi porti 

master: master.o common.o
	gcc master.o common.o -o master 
	
navi: navi.o common.o
	gcc navi.o common.o -o navi -lm
	
porti: porti.o common.o
	gcc porti.o common.o -o porti

master.o: master.c common.o
	gcc -c $(CFLAGS) master.c

navi.o: navi.c 
	gcc -c $(CFLAGS) navi.c 
	
porti.o: porti.c 
	gcc -c $(CFLAGS) porti.c
common.o: common.c common.h
	gcc -c $(CFLAGS) common.c
# elimina i file .o
clean:
	rm -f *.o master navi porti common

# esegue l'applicazione
run: master.o
	./master
