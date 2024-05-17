#include <stdio.h>
#include <mpi.h>

#define MASTER 0

void barrier(int rank, int size) {

    int REQ_posso_andare;
    int MSG_puoi_andare;

    if (rank == MASTER) {

        int p;
        int processi_arrivati = 1; 
        
        /* aspetto che tutti gli slave comunichino la loro presenza */
        for (p=1; p<size; p++)
            MPI_Recv(&REQ_posso_andare, 1, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* do il via libera agli slave */
        for (p=1; p<size; p++)
            MPI_Send(&MSG_puoi_andare, 1, MPI_INT, p, 0, MPI_COMM_WORLD);

    }
    else {

        /* segnalo la mia presenza al master */
        MPI_Send(&REQ_posso_andare, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);

        /* attendo il via libera del master */
        MPI_Recv(&MSG_puoi_andare, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }

    /* esco dalla barrier */
    return;

}

void main(int argc, char *argv[]) {

    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;
    MPI_Request request;

    barrier(rank, size);

    MPI_Finalize();
    return;

}