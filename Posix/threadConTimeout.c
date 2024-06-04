#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define false 0
#define true  1

#define WAIT_TIME 100
#define EXTRA_TIME 1


pthread_t thread;
pthread_t timer;
pthread_mutex_t lock;
int threadTerminato;


void* threadTimer(void* arg) {
    usleep(WAIT_TIME);
    return NULL;
}

void* threadRun(void* arg) {
    
    /* effettuo delle operazioni */
    printf("thread inizio...\n");
    usleep(WAIT_TIME+EXTRA_TIME);

    /* indico che ho terminato l'esecuzione */
    pthread_mutex_lock(&lock);
    threadTerminato = true;
    pthread_mutex_unlock(&lock);
    printf("thread fine...\n");

    /* termino */
    return NULL;
}

void init() {
    threadTerminato = false;
}

int main(int argc, char* argv[]) {

    init();

    /* avvio il thread operaio */
    pthread_create(&thread, NULL, threadRun, NULL);

    /* avvio il thread timer e aspetto che scada il tempo - TODO: fare la join in un thread separato, in modo da ottimizzare meglio */
    pthread_create(&timer, NULL, threadTimer, NULL);
    pthread_join(timer, NULL);

    /*
    ...
    */

    /* stampo lo stato del thread */
    pthread_mutex_lock(&lock);
    printf("Main terminato -> %d\n", threadTerminato);
    pthread_mutex_unlock(&lock);

    return 0;
}
