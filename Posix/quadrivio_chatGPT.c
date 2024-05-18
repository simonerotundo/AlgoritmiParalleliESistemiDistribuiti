/* 
 * Si consideri il traffico di macchine in prossimità di un quadrivio. Implementare le
 * macchine come thread paralleli e gestire il transito delle macchine in base alle
 * seguenti regole:
 * 
 *      - una macchina appare inizialmente, dopo certo tempo random, in una delle strade
 *        che compongono il quadrivio. Se vi sono altre macchine in attesa nella stessa strada,
 *        essa si mette in coda ad esse.
 * 
 *      - solo una macchina alla volta può transitare l'incrocio.
 * 
 *      - una macchina può transitare l’incrocio solo se ha la destra libera. Se nessuna
 *        macchina ha la destra libera, una macchina a caso transiterà l'incrocio.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_BUFFER 4
#define DIM_BUFFER 3

#define NUM_PRODUCERS NUM_BUFFER
#define NUM_CONSUMERS NUM_BUFFER

#define STAMPA  1   /* 0 : non stampare,  1 : stampa  */
#define PRODUCI 1   /* 0 : non produrre,  1 : produci */
#define CONSUMA 1   /* 0 : non consumare, 1 : consuma */

#define LIBERO   0
#define OCCUPATO 1


struct Buffer {
    pthread_mutex_t mutex;
    pthread_cond_t condTuttoPieno;
    pthread_cond_t condTuttoVuoto;
    int id;
    int buffer[DIM_BUFFER];
    int headIndex;
};

struct Buffer buffers[NUM_BUFFER];

pthread_mutex_t incrocio_mutex = PTHREAD_MUTEX_INITIALIZER;
int buffer_in_transito = -1; // Indica quale buffer sta attraversando l'incrocio

void* printBuffers(void* arg) {
    while(STAMPA) {
        usleep(160000);

        for(int b=0; b<NUM_BUFFER; b++)
            pthread_mutex_lock(&buffers[b].mutex);

        for(int b=0; b<NUM_BUFFER; b++) {
            printf("##   BUFFER-%d   ## -> [ ", b);
            for(int i=0; i<DIM_BUFFER; i++)
                if(buffers[b].buffer[i] == LIBERO)
                    printf(" • ");
                else if(buffers[b].buffer[i] == OCCUPATO)
                    printf(" 🚖 ");
                else
                    printf(" ? ");
            printf(" ]\n");
        }
        printf("\n");

        for(int b=0; b<NUM_BUFFER; b++)
            pthread_mutex_unlock(&buffers[b].mutex);
    }
    return NULL;
}

void* producer(void* id_buffer) {
    int id = *(int*) id_buffer;
    struct Buffer* myBuffer = &buffers[id];

    while(PRODUCI) {
        pthread_mutex_lock(&myBuffer->mutex);
        
        while(myBuffer->headIndex >= DIM_BUFFER)
            pthread_cond_wait(&myBuffer->condTuttoPieno, &myBuffer->mutex);

        myBuffer->buffer[myBuffer->headIndex] = OCCUPATO;
        myBuffer->headIndex++;

        pthread_mutex_unlock(&myBuffer->mutex);
        pthread_cond_broadcast(&myBuffer->condTuttoVuoto);
    }
    return NULL;
}

void* consumer(void* id_buffer) {
    int id = *(int*) id_buffer;
    struct Buffer* myBuffer = &buffers[id];

    while(CONSUMA) {
        pthread_mutex_lock(&myBuffer->mutex);
        
        while(myBuffer->headIndex <= 0)
            pthread_cond_wait(&myBuffer->condTuttoVuoto, &myBuffer->mutex);

        int idBufferDestra = (myBuffer->id + NUM_BUFFER - 1) % NUM_BUFFER;

        pthread_mutex_unlock(&myBuffer->mutex);

        pthread_mutex_lock(&incrocio_mutex);

        if (buffer_in_transito == -1 || buffer_in_transito == idBufferDestra) {
            buffer_in_transito = id;
            pthread_mutex_unlock(&incrocio_mutex);

            pthread_mutex_lock(&myBuffer->mutex);

            myBuffer->buffer[0] = LIBERO;
            for (int i = 1; i < DIM_BUFFER; i++) {
                if (myBuffer->buffer[i] == OCCUPATO) {
                    myBuffer->buffer[i] = LIBERO;
                    myBuffer->buffer[i-1] = OCCUPATO;
                    myBuffer->headIndex = i;
                }
            }
            pthread_mutex_unlock(&myBuffer->mutex);

            pthread_mutex_lock(&incrocio_mutex);
            buffer_in_transito = -1;
        }
        
        pthread_mutex_unlock(&incrocio_mutex);
        pthread_cond_broadcast(&myBuffer->condTuttoPieno);
    }
    return NULL;
}

void init() {
    for(int b = 0; b < NUM_BUFFER; b++) {
        pthread_mutex_init(&buffers[b].mutex, NULL);
        pthread_cond_init(&buffers[b].condTuttoPieno, NULL);
        pthread_cond_init(&buffers[b].condTuttoVuoto, NULL);
        buffers[b].id = b;
        for(int i = 0; i < DIM_BUFFER; i++)
            buffers[b].buffer[i] = LIBERO;
        buffers[b].headIndex = 0;
    }
}

int main() {
    init();

    pthread_t printer;
    pthread_create(&printer, NULL, &printBuffers, NULL);

    pthread_t producers[NUM_PRODUCERS];
    for(int p = 0; p < NUM_BUFFER; p++) {
        int* ptr_id = malloc(sizeof(int));
        *ptr_id = p;
        pthread_create(&producers[p], NULL, &producer, ptr_id);
    }

    pthread_t consumers[NUM_CONSUMERS];
    for(int c = 0; c < NUM_BUFFER; c++) {
        int* ptr_id = malloc(sizeof(int));
        *ptr_id = c;
        pthread_create(&consumers[c], NULL, &consumer, ptr_id);
    }

    pthread_join(printer, NULL);

    for(int p = 0; p < NUM_BUFFER; p++)
        pthread_join(producers[p], NULL);

    for(int c = 0; c < NUM_BUFFER; c++)
        pthread_join(consumers[c], NULL);

    return 0;
}