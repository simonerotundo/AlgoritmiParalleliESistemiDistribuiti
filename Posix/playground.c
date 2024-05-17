#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define DIM_BUFFER 10
#define N_PRODUCERS 5
#define N_CONSUMERS 5

#define SLEEP_TIME_MS_PRINTER 10000
#define SLEEP_TIME_MS_THREADS 1000

#define LIBERO   0
#define OCCUPATO 1


pthread_t lockBuffer;
pthread_cond_t condEmpty;
pthread_cond_t condFull;

int buffer[DIM_BUFFER];     /* LIFO */
int headIndex = 0;          /* indice in cui producer inserisce e consumer preleva */
int printerIteration = 0;   /* numero di stampe effettuate */

int devoStampare  = 1;
int devoProdurre  = 1;
int devoConsumare = 1;

void* printBuffer() {
    while(devoStampare) {
      pthread_mutex_lock(&lockBuffer);
        printf("BUFFER   @ %4d-> [ ", ++printerIteration);
        for(int i=0; i<DIM_BUFFER; i++) {
            if(buffer[i] == LIBERO)
                printf("-");
            else if(buffer[i] == OCCUPATO)
                printf("*");
            else
                printf("?");
        }
        printf(" ]\n");
        pthread_mutex_unlock(&lockBuffer);
        usleep(SLEEP_TIME_MS_PRINTER);  
    }

    return NULL;
}

void* threadProducer() {

    while(devoProdurre) {
        /* prendo il lock sulle risorse condivise */
        pthread_mutex_lock(&lockBuffer);

        /* se il buffer è pieno, mi metto in attesa */
        while(headIndex >= DIM_BUFFER)
            pthread_cond_wait(&condFull, &lockBuffer);
        
        /* inserisco un elemento e aggiorno gli indici del prossimo elemento da prelevare e da inserire */
        buffer[headIndex] = OCCUPATO;
        headIndex++;

        /* avviso i consumer che ho inserito un elemento */
        pthread_cond_broadcast(&condEmpty);

        /* rilascio il lock */
        pthread_mutex_unlock(&lockBuffer);

        /* riposo */
        usleep(SLEEP_TIME_MS_THREADS);
    }

    return NULL;
}

void* threadConsumer() {

    while(devoConsumare) {
        /* prendo il lock sulle risorse condivise */
        pthread_mutex_lock(&lockBuffer);

        /* se il buffer è vuoto, mi metto in attesa */
        while(headIndex <= 0)
            pthread_cond_wait(&condEmpty, &lockBuffer);
        
        /* prelevo un elemento e aggiorno gli indici del prossimo elemento da prelevare e da inserire */
        buffer[headIndex-1] = LIBERO;
        headIndex--;

        /* avviso i producer che ho inserito un elemento */
        pthread_cond_broadcast(&condFull);

        /* rilascio il lock */
        pthread_mutex_unlock(&lockBuffer);

        /* riposo */
        usleep(SLEEP_TIME_MS_THREADS);
    }

    return NULL;
}

void finalize() {
    pthread_mutex_destroy(&lockBuffer);
    pthread_cond_destroy(&condEmpty);
    pthread_cond_destroy(&condFull);
}

void init() {
    pthread_mutex_init(&lockBuffer, NULL);
    pthread_cond_init(&condEmpty, NULL);
    pthread_cond_init(&condFull, NULL);

    for(int i=0; i<DIM_BUFFER; i++)
        buffer[i] = 0;
}

int main(int argc, char* argv[]) {

    pthread_t producers[N_PRODUCERS];
    pthread_t consumers[N_CONSUMERS];

    /* inizializzo le variabili */
    init();

    /* inizializzo ed avvio il thread printer */
    pthread_t printer;
    pthread_create(&printer, NULL, &printBuffer, NULL);

    /* creo N_PRODUCERS produttori */
    for(int p=0; p<N_PRODUCERS; p++) {
        int* ptr_id = malloc(sizeof(int*));
	    *ptr_id = p;
        pthread_create(&producers[p], NULL, &threadProducer, ptr_id);
    }

    /* creo N_CONSUMERS consumatori */
    for(int c=0; c<N_CONSUMERS; c++) {
        int* ptr_id = malloc(sizeof(int*));
	    *ptr_id = c;
        pthread_create(&consumers[c], NULL, &threadConsumer, ptr_id);
    }

    /* aspetto che i produttori finiscano */
    for(int p=0; p<N_PRODUCERS; p++)
        pthread_join(producers[p], NULL);

    /* aspetto che i consumatori finiscano */
    for(int c=0; c<N_CONSUMERS; c++)
        pthread_join(consumers[c], NULL);

    /* aspetto che il thread stampatore finisca */
    pthread_join(printer, NULL);

    /* distruggo le variabili allocate */
    finalize();

    return 0;
}
