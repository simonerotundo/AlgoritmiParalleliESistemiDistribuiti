/* 
 * Si calcoli il valore approssimato di Pi Greco tramite il metodo di Montecarlo (v. slide
 * lezioni teoria). Si faccia attenzione a problemi di false sharing, serializzazione, etc
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define NUM_POINTS 100000
#define N_THREADS 5

pthread_t threads[N_THREADS];
pthread_mutex_t mutex;
long points_inside_circle = 0;

void* calcolaPiGreco() {

    double x, y, distance;
    for (long i = 0; i < NUM_POINTS; i++) {
        // Generazione di punti casuali tra -1 e 1
        x = (double)rand() / RAND_MAX * 2.0 - 1.0;
        y = (double)rand() / RAND_MAX * 2.0 - 1.0;
        
        // Calcolo della distanza dall'origine
        distance = x*x + y*y;
        
        // Controlla se il punto è dentro il cerchio
        if (distance <= 1.0) {
            pthread_mutex_lock(&mutex);
            points_inside_circle++;
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

int main() {
    
    // Inizializzazione del generatore di numeri casuali
    srand(time(NULL));
    
    pthread_mutex_init(&mutex, NULL);
    for(int i=0; i<N_THREADS; i++)
        pthread_create(&threads[i], NULL, &calcolaPiGreco, NULL);

    for(int i=0; i<N_THREADS; i++)
        pthread_join(threads[i], NULL);

    
    // Calcolo di π -> [ (4 * punti all'interno del cerchio) / (numero totale di punti) ]
    double pi_estimate = (double)points_inside_circle / (NUM_POINTS * N_THREADS) * 4.0;
    
    // Stampa il risultato
    printf("Valore stimato di π: %f\n", pi_estimate);
    
    return 0;
}
