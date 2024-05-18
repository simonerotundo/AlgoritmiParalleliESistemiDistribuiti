/* 
 * Dato un array in memoria (variabile globale) di dimensione N dividere l'operazione
 * di somma degli elementi dell'array equamente tra un numero T di thread.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define N 9     /* numero di celle dell'array */
#define T 2     /* numero di thread (N>T)     */
#define DIM_CHUNK (N/T) /* funziona solo se N%T==0 */


struct Chunk {
    /* thread */
    int id_thread;
    pthread_t thread;
    
    /* chunk */
    int indiceInizio;   
    int indiceFine;     /* la cella all'indice indiceFine NON è inclusa nella somma */
};

pthread_mutex_t mutex;

/* non uso i lock sull'array perchè ogni thread ha la sua porzione, quindi non si verificano race condition. lo uso solamente alla fine per aggiornare il valore della somma */
int array[N];
struct Chunk chunks[T];
int somma;


void aggiornaSomma(int t, int sp) {
    int id_thread = t;
    int sommaParziale = sp;

    pthread_mutex_lock(&mutex);
    somma += sommaParziale;
    pthread_mutex_unlock(&mutex);
}

void* calcolaSommaParziale(void* id) {

    int id_t = *(int*) id;

    int sommaParziale = 0;
    for(int i=chunks[id_t].indiceInizio; i<chunks[id_t].indiceFine; i++)
        sommaParziale += array[i];

    printf("[t-%d] parziale: %3d\n", id_t, sommaParziale);
    aggiornaSomma(id_t, sommaParziale);

    return NULL;
}

void init() {

    /* inizializzo il mutex */
    pthread_mutex_init(&mutex, NULL);

    /* inizializzo l'array */
    for(int i=0; i<N; i++)
        array[i] = i;

    /* inizializzo la somma a 0 */
    somma = 0;
}

int main(int argc, char* argv[]) {

    /* inizializzo l'array */
    init();

    /* effettuo la suddivisione in chunk ed eseguo i thread */
    for(int i=0; i<T; i++) {
        chunks[i].id_thread = i;
        chunks[i].indiceInizio = chunks[i].id_thread    * DIM_CHUNK;
        chunks[i].indiceFine   = chunks[i].indiceInizio + DIM_CHUNK;

        int* ptr_id = (int*)malloc(sizeof(int));
        *ptr_id = i;
        pthread_create(&chunks[i].thread, NULL, &calcolaSommaParziale, ptr_id);
    }

    /* attendo che tutti i thread terminino */
    for(int i=0; i<T; i++) {
        pthread_join(chunks[i].thread, NULL);
    }

    /* stampo la somma complessiva */
    pthread_mutex_lock(&mutex);
    printf("Somma complessiva: %d\n", somma);
    pthread_mutex_unlock(&mutex);

    return 0;
}
