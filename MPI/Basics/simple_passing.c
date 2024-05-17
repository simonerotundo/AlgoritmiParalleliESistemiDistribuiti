#include <stdio.h>
#include <mpi.h>

void main(int argc, char *argv[]) {

    int err, nproc, myid;
    MPI_Status status;
    float a[2];

    err = MPI_Init(&argc, &argv);
    err = MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    err = MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    
    if (myid == 0) {
        a[0] = 3.0;
        a[1] = 5.0;

        /* MPI_Send(a, 2, MPI_FLOAT, 1, 10, MPI_COMM_WORLD):
         * buffer
         * quantità di dati da inviare
         * tipo di dato da inviare
         * id destinatario
         * tag
         * MPI_COMM_WORLD
        */
        MPI_Send(a, 2, MPI_FLOAT, 1, 10, MPI_COMM_WORLD);
    }
    else if (myid == 1) {
        /* MPI_Send(a, 2, MPI_FLOAT, 1, 10, MPI_COMM_WORLD):
         * buffer
         * quantità di dati da ricevere
         * tipo di dato da ricevere
         * id sorgente
         * tag
         * MPI_COMM_WORLD
         * stato
        */
        MPI_Recv(a, 2, MPI_FLOAT, 0, 10, MPI_COMM_WORLD, &status);
        printf("###  ID: %d  ###\n", myid);
        printf(" a[0]:   %f     \n", a[0]);
        printf(" a[1]:   %f     \n", a[1]);

    }
    else {
        /* pass */
    }

    err = MPI_Finalize();

}