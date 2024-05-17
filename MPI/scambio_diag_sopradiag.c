#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define SOPRADIAGONALE 0
#define DIAGONALE 1
#define OTHER 2

#define NODE_0 0
#define NODE_1 1

#define NROWS 4
#define NCOLS 4
int nsteps=1;
int rank;


int mat[NROWS][NCOLS];

void printMat() {
    /* stampa la matrice */
    printf("Stampo matrice cpu-%d\n", rank);
    for(int i=0; i<NROWS; i++) {
        for (int j=0; j<NCOLS; j++) {
            printf("%2d ", mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void init() {
    /* inizializza la matrice */
   for(int i=0; i<NROWS; i++)
        for(int j=0; j<NCOLS; j++) 
            if(rank==0) mat[i][j] = 0; /* matrice 0 -> tutti 0 */
            else        mat[i][j] = 1; /* matrice 1 -> tutti 1 */
}

void swap() {}

void exchBord() {

    MPI_Datatype diagonale;
    MPI_Type_vector(NROWS, 1, NCOLS+1, MPI_INT, &diagonale);
    MPI_Type_commit(&diagonale);

    MPI_Datatype sopradiagonale;
    MPI_Type_vector(NCOLS-1, 1, NCOLS+1, MPI_INT, &sopradiagonale);
    MPI_Type_commit(&sopradiagonale);

    if(rank==0) {
        /* invio la diagonale a cpu-1 */
        MPI_Send(&(mat[0][0]), 1, diagonale, NODE_1, 0, MPI_COMM_WORLD);

        /* ricevo la sopradiagonale da cpu-1 */
        MPI_Recv(&(mat[0][1]), 1, sopradiagonale, NODE_1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        /* ricevo la diagonale da cpu-0 */
        MPI_Recv(&(mat[0][0]), 1, diagonale, NODE_0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* invio la sopradiagonale a cpu-0 */
        MPI_Send(&(mat[0][1]), 1, sopradiagonale, NODE_0, 1, MPI_COMM_WORLD);
    }


    MPI_Barrier(MPI_COMM_WORLD);
    printMat();
    MPI_Type_free(&diagonale);
    MPI_Type_free(&sopradiagonale);

}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    init();
    /* initAutoma(); */

    for(int s=0; s<nsteps; s++) {
        exchBord();
        /* tranFunc(); */
        swap();
    }

    MPI_Finalize();
    return 0;

}
