/**
 * 
 * TRACCIA DEL 13-02-2024
 * 
**/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define N_THREADS 5
#define TIMEOUT 5

pthread_t th[N_THREADS];
pthread_mutex_t mutexBarrier;
pthread_cond_t conditionBarrier;
int threadArrivati;


void initBarrier() {
    pthread_mutex_init(&mutexBarrier, NULL);
    pthread_cond_init(&conditionBarrier, NULL);
    threadArrivati = 0;
}

void barrier() {
    pthread_mutex_lock(&mutexBarrier);
    threadArrivati++;

    while(threadArrivati < N_THREADS)
        pthread_cond_wait(&conditionBarrier, &mutexBarrier);

    pthread_cond_broadcast(&conditionBarrier);
    pthread_mutex_unlock(&mutexBarrier);
}

void waitToTerminate() {

    usleep(TIMEOUT);

    pthread_mutex_lock(&mutexBarrier);
    if(threadArrivati == N_THREADS) {
        /* i thread hanno finito in tempo */
        printf("i thread hanno finito in tempo\n");
        /***** deadlock !!!
        for(int i=0; i<N_THREADS; i++)
            pthread_join(th[i], NULL);
        *****/
    } else {
        /* i thread non hanno finito per tempo */
        printf("i thread non hanno finito in tempo\n");
        pthread_cond_broadcast(&conditionBarrier);
        for(int i=0; i<N_THREADS; i++) {
            pthread_cancel(th[i]);
        }
            
    }
    pthread_mutex_unlock(&mutexBarrier);

}

void doSomething() { /* implementazione */ }

void* threadFunc() {
    doSomething();
    printf("inizio\n");
    barrier();
    printf("fine\n");
    return NULL;
}

int main(int argc, char* argv[]) {

    initBarrier();
    for (int i = 0; i < N_THREADS; i++)
        pthread_create(&th[i], NULL, &threadFunc, NULL);

    waitToTerminate();
    printf("fine del main\n");
    return 0;

}
