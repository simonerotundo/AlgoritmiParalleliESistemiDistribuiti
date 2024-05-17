#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define N_POSTI 5
#define N_GIOCATORI (N_POSTI+1)

#define LIBERO 0
#define OCCUPATO 1

struct Posto {
    pthread_mutex_t lock;   /* lock per proteggere la risorsa */
    int stato;              /* LIBERO o OCCUPATO */
};

struct Posto posti[N_POSTI];


void* iniziaGioco(void* arg) {
    int cpu = *(int*)arg;
    
    /* scandisco tutti i posti in ordine crescente in modo da evitare deadlock */
    for(int p=0; p<N_POSTI; p++) {

        /* acquisisco il lock sul posto che sto controllando */
        pthread_mutex_lock(&posti[p].lock);

        /* se è libero mi ci siedo, rilascio il lock e termino */
        if(posti[p].stato == LIBERO) {
            posti[p].stato = OCCUPATO;
            printf("[cpu-%d] - Ho occupato il posto %d, ho vintooo\n", cpu, p);
            pthread_mutex_unlock(&posti[p].lock);
            return NULL;
        } else { 
            /* se è occupato rilascio il lock e amen */
            pthread_mutex_unlock(&posti[p].lock); 
        }
    }

    /* se sono arrivato a questo punto, vuol dire che non c'è nessun posto libero, quindi ho perso */
    printf("[cpu-%d] - Sono rimasto in piedi :(\n", cpu);
    return NULL;
}

void terminaGioco() {
    /* distruggo i mutex */
    for(int p=0; p<N_POSTI; p++)
        pthread_mutex_destroy(&posti[p].lock);
}

void inizializzaGioco() {
    /* inizializzo i mutex e inizializzo i posti a LIBERO */
    for(int p=0; p<N_POSTI; p++) {
        pthread_mutex_init(&posti[p].lock, NULL);   /* inizializzo i lock  */
        posti[p].stato = LIBERO;                    /* inizializzo i posti */
    }
}

int main(int argc, char* argv[]) {

    /* inizializzo la struct dei posti e inizializzo i thread giocatori */
    inizializzaGioco();
    pthread_t giocatori[N_GIOCATORI];

    /* avvio i thread giocatori e gli assegno un ID */
    for(int p=0; p<N_GIOCATORI; p++) {
        int* ptr_id = (int*)malloc(sizeof(int));                    /* id thread */
        *ptr_id = p;                                                /* (evito che il puntatore mi cambi sotto al culo) */
        pthread_create(&giocatori[p], NULL, &iniziaGioco, ptr_id);  /* creo il thread */
    }

    /* aspetto che i thread giocatori finiscano */
    for(int p=0; p<N_GIOCATORI; p++)
        pthread_join(giocatori[p], NULL);

    /* termino il gioco */
    terminaGioco();
    return 0;
}
