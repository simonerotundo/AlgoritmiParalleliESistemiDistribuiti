#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0
#define SIZE 10


int main(int argc, char *argv[]) {

    int matrice[SIZE][SIZE];

    int p;
    int my_rank;
    MPI_Status status;
    MPI_Datatype column_mpi_t;
    int i, j;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Type_vector(SIZE, 1, SIZE, MPI_FLOAT, &column_mpi_t);
    MPI_Type_commit(&column_mpi_t);

    if (my_rank == MASTER) {

        /* inizializzo la matrice */
        for (i=0; i<10; i++) 
            for (j=0; j<10; j++) 
                matrice[i][j] = j;
        
        /* invio la terza colonna della matrice al processo 1 */
        MPI_Send(&(matrice[0][2]), 1, column_mpi_t, 1, 0, MPI_COMM_WORLD);
            
    }
    else if (my_rank == 1) {

        /* ricevo la terza colonna della matrice dal processo 0 */
        MPI_Recv(&(matrice[0][2]), 1, column_mpi_t, 0, 0, MPI_COMM_WORLD, &status);

        printf("\n[ ");
        for (i=0; i<10; i++)
            printf("%d ", matrice[i][2]);
        printf("]\n");

        MPI_Finalize();
        
    } 
    else { /* non sono previsti altri processi */ }

}
