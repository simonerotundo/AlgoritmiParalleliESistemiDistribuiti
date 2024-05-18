/* 
 * Un array contiene o celle vuote (es: valore 0) o celle che contengono una formica
 * (es: valore 1). 
 * Associare il movimento di ogni formica ad un thread in modo che il
 * comportamento delle formiche sia il seguente:
 *      Ogni formica inizialmente possiede una direzione (destra o sinistra) scelta in modo
 *      random. Quando una formica si scontra con un'altra, inverte la direzione del suo
 *      moto. Visualizzare il moto complessivo.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MOVE  1
#define PRINT 1

#define N_CELLS 20
#define N_ANTS  (N_CELLS/2)

#define LIBERA 0
#define OCCUPATA 1

#define SINISTRA (-1)
#define DESTRA     1

#define FALSE 0
#define TRUE 1


struct Cell {
    pthread_mutex_t mutex;  /* mutex sulla singola cella */
    int libera;             /* 0: libera o 1: occupata   */
};

struct Ant {
    int position;           /* posizione della formica */
    int direction;          /* direzione della formica */
};

pthread_t threads[N_ANTS];

struct Cell array[N_CELLS];
struct Ant ants[N_ANTS];


void* printAnts(void* arg) {

    while(PRINT) {
        usleep(600000);

        /* acquisisco i lock di tutte le celle */
        for(int i=0; i<N_CELLS; i++) 
            pthread_mutex_lock(&array[i].mutex);
        
        /* stampo le celle e le formiche */
        printf("[ ");
        for(int i=0; i<N_CELLS; i++)
            if(array[i].libera == LIBERA)
                printf(" • ");
            else if(array[i].libera == OCCUPATA)
                printf(" 🐜 ");
            else
                printf(" ? ");
        printf(" ]\n");
        
        /* rilascio il lock di tutte le celle */
        for(int i=0; i<N_CELLS; i++) 
            pthread_mutex_unlock(&array[i].mutex);
    }

    return NULL;
}

void* move(void* me) {
    struct Ant* ant = (struct Ant*) me;
    
    while(MOVE) {
        usleep(1000000);
        /* memorizzo la mia posizione e la cella in cui voglio muovermi */
        int possoMuovermi = TRUE;
        int myPosition  = ant->position;
        int newPosition = ant->position + ant->direction;

        /* se sono arrivato ad un bordo, cambio direzione */
        if(newPosition < 0 || newPosition>=N_CELLS) {
            possoMuovermi = FALSE;
            ant->direction = ant->direction * (-1); /* cambio direzione */
        }

        if(possoMuovermi == TRUE) {
            /* acquisisco i lock in ordine in modo da evitare deadlock */
            if(newPosition < myPosition) {
                pthread_mutex_lock(&array[newPosition].mutex);
                pthread_mutex_lock(&array[myPosition].mutex);
            }
            else {
                pthread_mutex_lock(&array[myPosition].mutex);
                pthread_mutex_lock(&array[newPosition].mutex);
            }
        
            switch (array[newPosition].libera) {
                case LIBERA:
                    /* la occupo */
                    array[myPosition].libera  = LIBERA;
                    array[newPosition].libera = OCCUPATA;
                    ant->position = newPosition;
                    break;

                case OCCUPATA:
                    /* cambio direzione */
                    ant->direction = ant->direction * (-1);
                    break;
                
                default:
                    /* ??? */
                    break;
            }

            /* rilascio i lock */
            pthread_mutex_unlock(&array[myPosition].mutex);
            pthread_mutex_unlock(&array[newPosition].mutex);
        }
        
    }

    return NULL;
}

void init() {

    /* inizializzo le celle dell'array */
    for(int i=0; i<N_CELLS; i++) {
        pthread_mutex_init(&array[i].mutex, NULL);
        array[i].libera = LIBERA;
    }

    /* inizializzo le formiche */
    for(int a=0; a<N_ANTS; a++) {
        /* inizializzo la struct di formiche */
        ants[a].position = a;
        ants[a].direction = SINISTRA;

        /* indico che la cella è occupata */
        array[a].libera = OCCUPATA;
    }
}

int main() {
    
    /* inizializzo le variabili */
    init();

    /* inizializzo la stampante */
    pthread_t printer;
    pthread_create(&printer, NULL, &printAnts, NULL);

    /* creo i thread */
    for(int a=0; a<N_ANTS; a++)
        pthread_create(&threads[a], NULL, &move, &ants[a]);

    /* join */
    pthread_join(printer, NULL);
    for(int a=0; a<N_ANTS; a++)
        pthread_join(threads[a], NULL);
    
    return 0;
}
