#include <stdio.h>
#include <unistd.h>
#include <mpi.h>



void main(int argc, char *argv[]) {

    int err, nproc, myid;
    MPI_Status status;
    #define DIM 1
    int palla[DIM];
    palla[0] = 0;

    err = MPI_Init(&argc, &argv);
    err = MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    err = MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    
    if (myid == 0) {

        int source = myid;
        int destination = myid+1;        
        for(;;) {
            palla[0] += 1;
            printf("###  [%d] passa la palla a [%d]  ###\n", source, destination);
            printf("rimbalzi: %d\n\n", palla[0]);
            MPI_Send(palla, DIM, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(palla, DIM, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
        }
        
    }
    else if (myid == 1) {

        int source = myid;
        int destination = myid-1;
        for(;;) {
            MPI_Recv(palla, DIM, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            palla[0] -= 1;
            printf("###  [%d] passa la palla a [%d]  ###\n", source, destination);
            printf("rimbalzi: %d\n\n", palla[0]);
            MPI_Send(palla, DIM, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }

    }

    err = MPI_Finalize();

}