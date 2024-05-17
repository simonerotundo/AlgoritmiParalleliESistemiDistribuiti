#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define N_POSTI 5
#define N_GIOCATORI (N_POSTI+1)

#define LIBERO 0
#define OCCUPATO 1

pthread_mutex_t lock;
int posti[N_POSTI];


int testa(int indice_posto) {
    return posti[indice_posto];
}

void occupa(int indice_posto) {
    posti[indice_posto] = OCCUPATO;
}

void* iniziaGioco(void* arg) {

    /* acquisisco il lock */
    pthread_mutex_lock(&lock);
    int id = *(int*) arg;

    /* cerco un posto libero */
    for(int p=0; p<N_POSTI; p++) {

        /* se ne trovo uno libero lo occupo ed esco */
        if(testa(p)==LIBERO) {
            /* occupo il posto */
            occupa(p);

            /* stampo un messaggio */
            printf("[cpu-%d] - Ho occupato il posto %d, ho vintooo\n", id, p);

            /* rilascio il lock ed esco */
            pthread_mutex_unlock(&lock);
            return NULL;
        }
    }

    /*** se arrivo qui, vuol dire che non ho trovato un posto libero ***/

    /* stampo un messaggio */
    printf("[cpu-%d] - Sono rimasto in piedi :(\n", id);

    /* rilascio il lock ed esco */
    pthread_mutex_unlock(&lock);
    return NULL;

}

void inizializzaGioco() {
    /* imposto tutte le sedie su LIBERO */
    for(int p=0; p<N_POSTI; p++)
        posti[p] = LIBERO;

    /* inizializzo il mutex */
    pthread_mutex_init(&lock, NULL);
}

int main(int argc, char* argv[]) {

    /* inizializzo il gioco */
    inizializzaGioco();

    /* inizializzo i thread */
    pthread_t giocatori[N_GIOCATORI];
    for(int p=0; p<N_GIOCATORI; p++) {
        int* ptr_id = (int*)malloc(sizeof(int));    /* id thread */
        *ptr_id = p;                                /* ^^^^^^^^^ */

        /* creo il thread */
        pthread_create(&giocatori[p], NULL, &iniziaGioco, ptr_id);
    }

    /* aspetto che tutti i thread finiscano */
    for(int p=0; p<N_GIOCATORI; p++)
        pthread_join(giocatori[p], NULL);

    /* termino il gioco */
    return 0;
}
