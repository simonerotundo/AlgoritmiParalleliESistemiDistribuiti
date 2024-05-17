#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NROWS  3
#define NCOLS  3

#define MASTER 0
#define SLAVE 1


int main(int argc, char ** argv)
{
    int i, j, rank, size;
    int A[NROWS][NCOLS], row[NCOLS], col[NROWS];  /* ATTENZIONE A row E col!! */

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


        /* costruisco il Datatype che conterrà una riga della matrice */
        MPI_Datatype row_dt;
        MPI_Type_contiguous(3, MPI_INT, &row_dt); /* uso MPI_Type_contiguous in quanto le celle sono contigue in memoria */
        MPI_Type_commit(&row_dt);

        /* invio la riga [1] alla cpu-1 */
        MPI_Send(&(A[1][0]), 1, row_dt, SLAVE, 0, MPI_COMM_WORLD);

        /* ripulisco la memoria */
        MPI_Type_free(&row_dt);

    }
    else if(rank == SLAVE) {

        /* ricevo la riga */
        MPI_Recv(row, NCOLS, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* stampo la riga ricevuta */
        for(int r=0; r<NROWS; r++)
            printf("%d ", row[r]);
            printf("\n");

    }

    /*****/

    MPI_Finalize();
    return 0;
}