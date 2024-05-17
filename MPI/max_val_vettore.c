#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0
#define SIZE 10 /* funziona solo se "SIZE%num_procs==0" */


int main(int argc, char *argv[]) {

    int* array = NULL;
    array = (int *)malloc(SIZE * sizeof(int));
    for (int a=0; a<SIZE; a++) {
        array[a] = a; // MAX=SIZE-1
    }

    int low, high;
    int my_id, n_procs, n_slaves;
    int highest, high_partial;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    n_slaves = n_procs-1; /* NON DEMOCRATICO */
    

    /* ########### */
    highest = array[0];
    if (my_id == MASTER) {

        /* aspetto affinchè mi vengano inviati tutti i risultati parziali */
        int s_id = 1,
            ns = n_slaves;
        for (s_id; s_id<n_procs; s_id++) {
            int ppp;
            MPI_Recv(&high_partial, 1, MPI_INT, s_id, 0, MPI_COMM_WORLD, &status);
            if (high_partial > highest) {
                highest = high_partial;
            }
        }

        printf("\n###  HIGHEST: %d  ###\n", highest);

    }
    else {

        /* individuo il mio chunk */
        int dim_chunk = SIZE/n_slaves;
        int start_index = (my_id-1) * dim_chunk;    /* "my_id-1" in quanto non è democratico */
        int stop_index  = start_index+dim_chunk;    /* stop_index non è incluso nella ricerca (si ferma prima) */        

        /* individuo il valore più alto presente nel mio chunk */
        int i = start_index,
            s = stop_index;
        high_partial = array[i];
        for (i; i<s; i++) {
            if (array[i] > high_partial) {
                high_partial = array[i];
            }
        }
        printf("parziale cpu-%d - [%d-%d] max: %d\n", my_id, start_index, stop_index-1, high_partial);

        /* lo invio al master */
        MPI_Send(&high_partial, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);

    }

    
    MPI_Finalize();
    return 0;
}
