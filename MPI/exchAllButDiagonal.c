#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 5

int rank;
int mat[N][N];

void exchAllButDiagonal() {

/**
 *
 *
 *     Full array          What we want
 *                            to send
 * +-----+-----+-----+  +-----+-----+-----+
 * |  1  |  2  |  3  |  |  -  |  2  |  3  |
 * +-----+-----+-----+  +-----+-----+-----+
 * |  4  |  5  |  6  |  |  4  |  -  |  6  |
 * +-----+-----+-----+  +-----+-----+-----+
 * |  7  |  8  |  9  |  |  7  |  8  |  -  |
 * +-----+-----+-----+  +-----+-----+-----+
 *
 * How to extract a column with a vector type:
 *
 *                      distance between the
 *                 start of each block: N+1 elements
 *          <---------------------> <-------------------->
 *         |                       |                      |
 *      start of                start of               start of
 *      block 1                 block 2                block 3
 *         |                       |                       |
 *         V                       V                       V
 *   +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *   |  -  |  2  |  3  |  4  |  -  |  6  |  7  |  8  |  -  |
 *   +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *          <--------------->       <--------------->
 *               block 1                 block 2
 * 
 **/
    MPI_Datatype dt;
    MPI_Type_vector(N-1, N, N+1, MPI_INT, &dt);
    MPI_Type_commit(&dt);

    if(rank == 0) {
        /* invio tutto tranne le celle che compongono la diagonale primaria */
        MPI_Send(&(mat[0][1]), 1, dt, 1, 0, MPI_COMM_WORLD); /* inizio dalla cella [0][1] in quanto [0][0] appartiene alla diagonale primaria */
    } else {
        /* le metto nella mia matrice a partire dalla cella [0][1] in quanto [0][0] appartiene alla diagonale primaria */
        MPI_Recv(&(mat[0][1]), 1, dt, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("++  MATRICE (cpu-1) - dopo aver ricevuto tutto tranne la diagonale primaria ++\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%2d ", mat[i][j]);
            }
            printf("\n");
        }
        printf("\n");

    }

    MPI_Type_free(&dt);

}

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0) {
        /* inizializzo e stampo la matrice 0 con numeri da 1 a N */
        printf("++  MATRICE (cpu-0)  ++\n");
        int v = 1;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                mat[i][j] = v++;
                printf("%2d ", mat[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    } else {
        /* inizializzo e stampo la matrice 1 con soli 0 */
        printf("++  MATRICE (cpu-1)  ++\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                mat[i][j] = 0;
                printf("%2d ", mat[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    exchAllButDiagonal();

    MPI_Finalize();
    return 0;

}