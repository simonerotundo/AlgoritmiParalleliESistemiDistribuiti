#include <stdio.h>
#include <mpi.h>

#define MASTER 0
#define SIZE 4

void inizializzaMatriceQuadrata(int (*matrice)[SIZE]) {
    int v = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) 
            matrice[i][j] = v++;
}

int main(int argc, char *argv[]) {

    int matrice[SIZE][SIZE];

    int p;
    int my_rank;
    MPI_Status status;
    
    int i, j;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Datatype diagonale;
    MPI_Type_vector(SIZE, 1, SIZE+1, MPI_INT, &diagonale); // Creazione del tipo di dato per la diagonale primaria
    MPI_Type_commit(&diagonale);

    if (my_rank == MASTER) {
        /* inizializzo la matrice */
        inizializzaMatriceQuadrata(matrice);

        // Invio la diagonale primaria al processo con rank 1
        MPI_Send(&matrice[0][0], 1, diagonale, 1, 0, MPI_COMM_WORLD); 
    }
    else if (my_rank == 1) {
        // Ricevo la diagonale primaria dal processo con rank 0
        MPI_Recv(&matrice[0][0], 1, diagonale, MASTER, 0, MPI_COMM_WORLD, &status);

        // Stampa la diagonale primaria ricevuta
        printf("Diagonale primaria ricevuta:\n");
        for (int i = 0; i < SIZE; i++) {
            printf("  %d\n", matrice[i][i]);
        }
    } 
    else { /* non sono previsti altri processi */ }
    
    MPI_Type_free(&diagonale);
    MPI_Finalize();
    return 0;
}
