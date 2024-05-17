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
    int A[NROWS][NCOLS], row[NCOLS], col[NROWS]; /* ATTENZIONE A row E col!! */

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
        MPI_Datatype col_dt;
        MPI_Type_vector(3, 1, 3, MPI_INT, &col_dt); /* uso MPI_Type_vector in quanto le righe NON sono contigue in memoria */
        MPI_Type_commit(&col_dt);

        /* invio la colonna 1 alla cpu-1 */
        MPI_Send(&(A[0][1]), 1, col_dt, SLAVE, 0, MPI_COMM_WORLD);

        /* ripulisco la memoria */
        MPI_Type_free(&col_dt);

    }
    else if(rank == SLAVE) {

        MPI_Recv(col, NCOLS, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for(int c=0; c<NCOLS; c++) printf("%2d ", col[c]);

    }

    /*****/

    MPI_Finalize();
    return 0;
}