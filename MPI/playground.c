#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NROWS 5
#define NCOLS 5

#define MASTER 0
#define SLAVE 1

#define TAG 0

int rank;
int mat[NROWS][NCOLS];


void printMatrix() {
    int msg;
    if (rank==MASTER) {
        printf("\n\nRANK-0\n");
        for(int i=0; i<NROWS; i++) {
            for(int j=0; j<NCOLS; j++) {
                printf("%2d ", mat[i][j]);
            }
            printf("\n");
        }
        MPI_Send(&msg, 1, MPI_INT, SLAVE, TAG, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&msg, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("\n\nRANK-1\n");
        for(int i=0; i<NROWS; i++) {
            for(int j=0; j<NCOLS; j++) {
                printf("%2d ", mat[i][j]);
            }
            printf("\n");
        }
    }
}

void exchDiagonal() {

    if(rank == MASTER) {

        /* invio la diagonale primaria a 1 */
        MPI_Datatype dp;
        MPI_Type_vector(NROWS, 1 , NCOLS+1, MPI_INT, &dp);
        MPI_Type_commit(&dp);

        MPI_Send(&(mat[0][0]), 1, dp, SLAVE, TAG, MPI_COMM_WORLD);

        MPI_Type_free(&dp);

    } else {

        /* ricevo la diagonale primaria di 0 e la copio nella mia diagonale secondaria */
        MPI_Datatype ds;
        MPI_Type_vector(NROWS, 1, NCOLS-1, MPI_INT, &ds);
        MPI_Type_commit(&ds);

        MPI_Recv(&(mat[0][NCOLS-1]), 1, ds, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Type_free(&ds);

    }

}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == MASTER)
        for(int i=0; i<NROWS; i++)
            for(int j=0; j<NCOLS; j++)
                mat[i][j]=0;
    else
        for(int i=0; i<NROWS; i++)
            for(int j=0; j<NCOLS; j++)
                mat[i][j]=1;

    exchDiagonal();
    MPI_Barrier(MPI_COMM_WORLD);

    /* stampo la matrice */
    printMatrix();

    MPI_Finalize();
    return 0;
}
