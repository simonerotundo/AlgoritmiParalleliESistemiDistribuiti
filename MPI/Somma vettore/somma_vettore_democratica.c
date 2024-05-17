#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MAXSIZE 5


void main(int argc, char *argv[]) {

    /* debug - sequenziale
    int rs = 0;
    for(int s=0; s<MAXSIZE; s++) 
        rs += s;
    printf("[ %d ]", rs);
    */

    double* data = NULL;

    int i, x, low, high;
    int myid, numprocs;
    int dest, source;
    double myresult, result, result_temp;
    double starttime, endtime;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);


    data = (double *)malloc(MAXSIZE * sizeof(double));

    result = 0;
    myresult = 0;


    /* inizializzo l'array */
    for (i=0; i<MAXSIZE; i++) {
        data[i] = i;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    starttime = MPI_Wtime();

    /* individuo la mia porzione di array (tenendo conto anche delle celle "spurie") */
    x = MAXSIZE / numprocs;
    int spurie = MAXSIZE % numprocs; // Calcolo del resto

    if (myid < spurie) {
        x++;
        low = myid * x;
        high = low + x;
    } else {
        low = myid * x + spurie;
        high = low + x;
    }

    /* calcolo il risultato */
    for (i=low; i<high; i++) {
        myresult += data[i];
    }
    printf("la cpu-%d calcola da [%d] a [%d] -> %f\n", myid, low, (high-1), myresult);

    /**  MASTER  **/
    if (myid == 0) {
        result = myresult;
        
        /* aspetto di ricevere numprocs numeri double */
        for(source=1; source<numprocs; source++) {
            MPI_Recv(&myresult, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            result += myresult;
        }
    }
    /**  SLAVE  **/
    else {
        MPI_Send(&myresult, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    endtime = MPI_Wtime();

    if (myid == 0) {
        printf("###  [elapsed time: %f] SOMMA COMPLESSIVA: %f  ###\n", 1000*(endtime-starttime), result);
    }

    free(data);
    MPI_Finalize();

}