#include <stdio.h>
#include <mpi.h>

void main(int argc, char *argv[]) {

    int err, nproc, myid;

    err = MPI_Init(&argc, &argv);
    err = MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    err = MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    /*
    ## CODE HERE
    */

   err = MPI_Finalize();

}