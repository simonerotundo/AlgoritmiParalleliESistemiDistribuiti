#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* attenzione, la diagonale primaria si può inviare solo se NROWS==NCOLS */
#define NROWS  3
#define NCOLS  3

#define MASTER 0
#define SLAVE 1


int main(int argc, char ** argv)
{
    int i, j, rank, size;
    int A[NROWS][NCOLS], diagonalePrimaria[NCOLS]; /* ATTENZIONE A row E col!! */

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    /*****/
    int v = 0;
    if(rank == MASTER) {

        /* inizializzo e stampo la matrice */
        printf("+++  matrice  +++\n");
        for (i = 0; i < NROWS; i++) {
            for (j = 0; j < NCOLS; j++) {
                A[i][j] = v++;
                printf("%3d  ", A[i][j]);
            }
            printf("\n");
        }
        printf("-----------------\n\n");
        /* ------------------------------- */

        /* creo il Datatype che conterrà una colonna della matrice */
        MPI_Datatype dp;
        MPI_Type_vector(NCOLS, 1, NCOLS+1, MPI_INT, &dp); /* uso MPI_Type_vector in quanto le righe NON sono contigue in memoria */
        MPI_Type_commit(&dp);

        MPI_Send(&(A[0][0]), 1, dp, SLAVE, 0, MPI_COMM_WORLD);

        MPI_Type_free(&dp);

    }
    else if(rank == SLAVE) { 

        MPI_Recv(diagonalePrimaria, NCOLS, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for(int d=0; d<NCOLS; d++) printf("%2d ", diagonalePrimaria[d]);

    }

    /*****/

    MPI_Finalize();
    return 0;
}