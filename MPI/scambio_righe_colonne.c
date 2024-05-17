/* matrice trasposta (le righe di A corrispondono alle colonne di B) senza effettuare operazioni logiche (tipo annidando i for) */

#include <stdio.h>
#include <mpi.h>

#define DIM 3

int rank;
int mat[DIM][DIM];

void exchTransposed() {

    if(rank == 0) {

        /* MPI_Type_vector in quanto le righe non sono contigue in memoria */
        MPI_Datatype column_dt;
        MPI_Type_vector(DIM, 1, DIM, MPI_INT, &column_dt);
        MPI_Type_commit(&column_dt);

        /* invio DIM colonne a cpu-1 */
        for(int c = 0; c < DIM; c++) {
            MPI_Send(&(mat[0][c]), 1, column_dt, 1, 0, MPI_COMM_WORLD);
        }

        MPI_Type_free(&column_dt);

    } else {

        /* MPI_Type_contiguous in quanto le righe sono contigue in memoria */
        MPI_Datatype row_dt;
        MPI_Type_contiguous(DIM, MPI_INT, &row_dt);
        MPI_Type_commit(&row_dt);

        /* ricezione delle righe trasposte */
        for(int r = 0; r < DIM; r++) {
            MPI_Recv(&(mat[r][0]), 1, row_dt, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        MPI_Type_free(&row_dt);
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Inizializzazione della matrice quadrata di interi
    if(rank==0) {
        printf("Matrice di partenza:\n");
        int v = 1;
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                mat[i][j] = v++;
                printf("%2d ", mat[i][j]);
            }
            printf("\n");
        }
    }

    exchTransposed();

    // Stampa della matrice trasposta
    if (rank == 1) {
        printf("\nMatrice trasposta:\n");
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                printf("%2d ", mat[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}
