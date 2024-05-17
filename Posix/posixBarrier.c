#include <stdio.h>
#include <pthread.h>


int nThread = 5;


/* variabili barrier */
pthread_mutex_t lock;
pthread_cond_t condition;
int threadArrivati;

void initBarrier() {
    threadArrivati = 0;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&condition, NULL);
}

void barrier() {

    /* acquisisco il lock */
    pthread_mutex_lock(&lock);

    /* incremento il numero di thread arrivati */
    threadArrivati++;

    /* se non sono l'ultimo thread, mi metto in attesa */
    while (threadArrivati < nThread)
        pthread_cond_wait(&condition, &lock);

    /* se sono l'ultimo thread, sveglio tutti gli altri */
    pthread_cond_broadcast(&condition);

    /* rilascio il lock */
    pthread_mutex_unlock(&lock);

    /* restituisco il controllo */
    return;

}

void destroyBarrier() {
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&condition);
}

void* threadFunc(void* arg) {
    printf("inizio\n");
    barrier();
    printf("fine\n");
    return NULL;
}

int main(int argc, char* argv[]) {

    pthread_t th[nThread];
    initBarrier();

    for(int i=0; i<nThread; i++)
        pthread_create(&th[i], NULL, &threadFunc, NULL);

    for(int i=0; i<nThread; i++)
        pthread_join(th[i], NULL);

    destroyBarrier();
    return 0;

}
