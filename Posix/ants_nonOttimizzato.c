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

#define N_CELLS 9
#define N_ANTS  (N_CELLS/2)

#define LIBERA 0
#define OCCUPATA 1

#define SINISTRA (-1)
#define DESTRA     1


struct Ant {
    int position;   
    int direction;
};

pthread_t threads[N_ANTS];
pthread_mutex_t mutex;

int array[N_CELLS];
struct Ant ants[N_ANTS];


void* printAnts() {

    while(PRINT) {
        usleep(900000);

        pthread_mutex_lock(&mutex);
        printf("[ ");
        for(int i=0; i<N_CELLS; i++)
            if(array[i] == LIBERA)
                printf(" • ");
            else if(array[i] == OCCUPATA)
                printf(" 🐜 ");
            else
                printf(" ? ");
        printf(" ]\n");
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void* move(void* me) {
    struct Ant* ant = (struct Ant*) me;
    
    while(MOVE) {
        /* memorizzo la mia posizione e la cella in cui voglio muovermi */
        int myPosition  = ant->position;
        int newPosition = ant->position + ant->direction;

        /* se sono arrivato ad un bordo, cambio direzione */
        if(newPosition <= 0 || newPosition>=N_CELLS) {
            ant->direction = ant->direction * (-1);
            newPosition = ant->position + ant->direction;
        }

        pthread_mutex_lock(&mutex);
        switch (array[newPosition])
        {
        case LIBERA:
            /* la occupo */
            array[myPosition]  = LIBERA;
            array[newPosition] = OCCUPATA;
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
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void init() {
    /* inizializzo il mutex */
    pthread_mutex_init(&mutex, NULL);

    /* inizializzo l'array */
    for(int i=0; i<N_CELLS; i++)
        array[i] = LIBERA;

    /* inizializzo le formiche */
    for(int a=0; a<N_ANTS; a++) {
        /* inizializzo la struct di formiche */
        ants[a].position = a;
        ants[a].direction = SINISTRA;

        /* indico che la cella è occupata */
        array[a] = OCCUPATA;
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
