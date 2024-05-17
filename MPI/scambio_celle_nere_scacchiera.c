#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 5 /* funziona solo con N dispari!!! */

#define MASTER 0
#define SLAVE 1

#define TAG 0

int rank;
int mat[N][N];


void printMatrix() {
    int msg;
    if (rank==MASTER) {
        printf("\n\nRANK-0\n");
        for(int i=0; i<N; i++) {
            for(int j=0; j<N; j++) {
                printf("%2d ", mat[i][j]);
            }
            printf("\n");
        }
        MPI_Send(&msg, 1, MPI_INT, SLAVE, TAG, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&msg, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("\n\nRANK-1\n");
        for(int i=0; i<N; i++) {
            for(int j=0; j<N; j++) {
                printf("%2d ", mat[i][j]);
            }
            printf("\n");
        }
    }
}

void exchChess() {

    /* le celle nere della matrice del nodo 0 quando N è dispari sono N*N/2+1, tuttavia le celle nere nel nodo 1 sono N*N/2, quindi c'è una perdita di informazioni*/
    int numBC = N*N/2; /* numero di celle che cpu-1 si aspetta */

    if(rank == MASTER) {
        /* numero di celle nere nella matrice del rank-0 (la prima cella è nera) */
        MPI_Datatype bc;
        MPI_Type_vector(numBC, 1, 2, MPI_INT, &bc);
        MPI_Type_commit(&bc);

        MPI_Send(&(mat[0][0]), 1, bc, SLAVE, TAG, MPI_COMM_WORLD);

        MPI_Type_free(&bc);

    } else {

        MPI_Datatype bc;
        MPI_Type_vector(numBC, 1, 2, MPI_INT, &bc);
        MPI_Type_commit(&bc);

        MPI_Recv(&(mat[0][1]), 1, bc, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Type_free(&bc);

    }

}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* inzializzo la scacchiera */
    if(rank == MASTER)
        for(int i=0; i<N; i++)
            for(int j=0; j<N; j++)
                mat[i][j]=0;
    else
        for(int i=0; i<N; i++)
            for(int j=0; j<N; j++)
                mat[i][j]=1;

    exchChess();
    MPI_Barrier(MPI_COMM_WORLD);

    /* stampo la matrice */
    printMatrix();

    MPI_Finalize();
    return 0;
}
