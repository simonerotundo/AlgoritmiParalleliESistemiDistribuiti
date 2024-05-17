#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>

void barrier_Send(int rank, int size) {

    // se c'è un solo processore non serve sincronizzarlo
    if (size > 1) {
        int sync_msg = 0;

        // invio un messaggio a tutti gli altri
        for (int i = 0; i < size; i++)
            if (i != rank)
                MPI_Send(&sync_msg, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

        // ricevo un messaggio da tutti gli altri
        for (int i = 0; i < size; i++)
            if (i != rank)
                MPI_Recv(&sync_msg, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


        // invio un messaggio per comunicare che sono pronto ad uscire
        for (int i = 0; i < size; i++)
            if (i != rank)
                MPI_Send(&sync_msg, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

        // ricevo il messaggio ed esco
        for (int i = 0; i < size; i++)
            if (i != rank)
                MPI_Recv(&sync_msg, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }

    return;
    
}

void barrier_Isend(int rank, int size) {

    // se c'è un solo processore non serve sincronizzarlo
    if (size > 1) {
        int sync_msg = 0;
        MPI_Request request;

        // invio un messaggio a tutti gli altri in modo asincrono
        for (int i = 0; i < size; i++)
            if (i != rank)
                MPI_Isend(&sync_msg, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);

        // attendo il completamento degli invii
        MPI_Waitall(size-1, &request, MPI_STATUSES_IGNORE);

        // ricevo un messaggio da tutti gli altri
        for (int i = 0; i < size; i++)
            if (i != rank)
                MPI_Recv(&sync_msg, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


        // invio un messaggio per comunicare che sono pronto ad uscire
        for (int i = 0; i < size; i++)
            if (i != rank)
                MPI_Isend(&sync_msg, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);

        // attendo il completamento degli invii
        MPI_Waitall(size-1, &request, MPI_STATUSES_IGNORE);

        // ricevo il messaggio ed esco
        for (int i = 0; i < size; i++)
            if (i != rank)
                MPI_Recv(&sync_msg, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }

    return;
}


int main(int argc, char **argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Esempio di utilizzo della funzione barrier
    printf("Processo %d: prima della barrier\n", rank);
    barrier_Isend(rank, size);
    printf("Processo %d: dopo la barrier\n", rank);

    MPI_Finalize();
    return 0;
}
