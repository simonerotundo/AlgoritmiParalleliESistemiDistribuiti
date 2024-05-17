#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0
#define MAXSIZE 5


int main(int argc, char *argv[]) {

    int* data = NULL;

    int low, high;
    int myid, numprocs;
    int result, result_temp;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    data = (int *)malloc(MAXSIZE * sizeof(int));

    result = 0;
    result_temp = 0;

    /* inizializzo l'array */
    for (int i=0; i<MAXSIZE; i++) {
        data[i] = i;
    }

    if (myid != MASTER) {

        /* individuo la mia porzione di array */
        int slaves = numprocs-1;
        int slaveid = myid-1;
        int chunk = MAXSIZE / slaves;
        int spurie = MAXSIZE % slaves;

        if (slaveid < spurie) {
            chunk++;
            low = slaveid * chunk;
            high = low + chunk;
        } else {
            low = slaveid * chunk + spurie;
            high = low + chunk;
        }

        /* calcolo il risultato */
        for (int i=low; i<high; i++) {
            result_temp += data[i];
        }

        printf("la cpu-%d calcola dalla cella %d alla cella %d (inclusa) -> %d\n", myid, low, (high-1), result_temp);
        MPI_Send(&result_temp, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);

    }
    else {
        for (int p=1; p<numprocs; p++) {
            MPI_Recv(&result_temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            result += result_temp;
        }

        printf("result: %d\n", result);
    }

    MPI_Finalize();
    return 0;
}
