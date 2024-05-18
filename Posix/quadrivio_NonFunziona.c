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

/* attenzione! nel ciclo while del thread producer è impostato che il buffer-0 non viene mai riempito! */


struct Buffer {
    /* todo: fix false sharing */
    pthread_mutex_t mutex;
    pthread_cond_t condTuttoPieno;
    pthread_cond_t condTuttoVuoto;
    int id;
    int buffer[DIM_BUFFER];
    int headIndex;
};

struct Buffer buffers[NUM_BUFFER];

void* printBuffers(void* arg) {

    while(STAMPA) {
        // usleep(160000);

        /* acquisisco tutti i lock */
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

        /* rilascio tutti i lock */
        for(int b=0; b<NUM_BUFFER; b++)
            pthread_mutex_unlock(&buffers[b].mutex);
    }
    return NULL;
}

void* producer(void* id_buffer) {
    /** ogni producer è associato ad un buffer! **/
    int id = *(int*) id_buffer;
    struct Buffer* myBuffer = &buffers[id];

    while(PRODUCI && id!=0) {   /* DEBUG: TOGLIERE QUEL "&& ID!=0" */

        /* acquisisco il lock sul buffer */
        pthread_mutex_lock(&myBuffer->mutex);
        
        /* se il buffer è pieno mi metto in attesa */
        while(myBuffer->headIndex >= DIM_BUFFER)
            pthread_cond_wait(&myBuffer->condTuttoPieno, &myBuffer->mutex);
            

        /* se c'è almeno una cella libera, aggiungo una macchina alla coda */
        myBuffer->buffer[myBuffer->headIndex] = OCCUPATO;
        myBuffer->headIndex++;

        
        /* rilascio il lock sul buffer */
        pthread_mutex_unlock(&myBuffer->mutex);

        /* sveglio i consumer in attesa */
        pthread_cond_broadcast(&myBuffer->condTuttoVuoto);
    }
    return NULL;
}

void* consumer(void* id_buffer) {
    /** ogni consumer è associato ad un buffer! **/
    int id = *(int*) id_buffer;
    struct Buffer* myBuffer = &buffers[id];

    while(CONSUMA) {

        int idBufferMio = myBuffer->id;
        int idBufferDestra = (myBuffer->id + NUM_BUFFER - 1) % NUM_BUFFER;

        /* acquisisco i lock sul mio buffer e su quello di destra */
        if(idBufferMio < idBufferDestra) {
            pthread_mutex_lock(&buffers[idBufferMio].mutex);
            pthread_mutex_lock(&buffers[idBufferDestra].mutex);
        } else {
            pthread_mutex_lock(&buffers[idBufferDestra].mutex);
            pthread_mutex_lock(&buffers[idBufferMio].mutex);
        }
        
        /* se il buffer è vuoto mi metto in attesa */
        while(myBuffer->headIndex <= 0)
            pthread_cond_wait(&myBuffer->condTuttoVuoto, &myBuffer->mutex);

        /* se ho la destra libera, passo */
        if(buffers[idBufferDestra].headIndex == 0) {
            printf("ho la destra libera, passo io (%d), PISTAAA.. dx:%d\n", id, buffers[idBufferDestra].buffer[0]);
            myBuffer->buffer[0] = LIBERO;
            
            /* faccio scorrere il traffico */
            for(int i=1; i<DIM_BUFFER; i++) {
                if(myBuffer->buffer[i] == OCCUPATO) {
                    myBuffer->buffer[i]   = LIBERO;
                    myBuffer->buffer[i-1] = OCCUPATO;
                    myBuffer->headIndex = i;
                }
            }
        }
        
        /* rilascio i lock sui buffer */
        pthread_mutex_unlock(&buffers[idBufferMio].mutex);
        pthread_mutex_unlock(&buffers[idBufferDestra].mutex);

        /* sveglio i producer in attesa */
        pthread_cond_broadcast(&myBuffer->condTuttoPieno);
    }
    return NULL;
}

void init() {

    /* inizializzo tutti i buffer */
    for(int b=0; b<NUM_BUFFER; b++) {

        /* inizializzo i lock */
        pthread_mutex_init(&buffers[b].mutex, NULL);

        /* inizializzo le condition */
        pthread_cond_init(&buffers[b].condTuttoPieno, NULL);
        pthread_cond_init(&buffers[b].condTuttoVuoto, NULL);

        /* inizializzo l'id del buffer */
        buffers[b].id = b;
        
        /* inizializzo i posti del buffer a LIBERO */
        for(int i=0; i<DIM_BUFFER; i++)
            buffers[b].buffer[i] = LIBERO;

        /* inizializzo la testa del buffer */
        buffers[b].headIndex = 0;
    }

}

int main() {
    
    /* inizializzo le variabili */
    init();

    /* avvio il thread stampatore */
    pthread_t printer;
    pthread_create(&printer, NULL, &printBuffers, NULL);

    /* avvio i producer - uno per buffer */
    pthread_t producers[NUM_PRODUCERS];
    for(int p=0; p<NUM_BUFFER; p++) {
        int* ptr_id = malloc(sizeof(int));
	    *ptr_id = p;
        pthread_create(&producers[p], NULL, &producer, ptr_id);
    }

    /* avvio i consumer - uno per buffer */
    pthread_t consumers[NUM_CONSUMERS];
    for(int c=0; c<NUM_BUFFER; c++) {
        int* ptr_id = malloc(sizeof(int));
	    *ptr_id = c;
        pthread_create(&consumers[c], NULL, &consumer, ptr_id);
    }

    /* attendo che il thread stampatore termini */
    pthread_join(printer, NULL);

    /* attendo che i producer terminino */
    for(int p=0; p<NUM_BUFFER; p++)
        pthread_join(producers[p], NULL);

    /* attendo che i consumer terminino */
    for(int c=0; c<NUM_BUFFER; c++)
        pthread_join(consumers[c], NULL);


    return 0;
}
