#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NROWS  4
#define NCOLS  7


int main(int argc, char ** argv)
{
    int i, j, rank, size;
    int A[NROWS][NCOLS], row[NCOLS], col[NROWS];
    MPI_Datatype row_type, col_type;            // Declare new datatypes

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
    MPI_Type_contiguous (NCOLS, MPI_INT, &row_type);           // Create row_type
    MPI_Type_vector     (NROWS, 1, NCOLS, MPI_INT, &col_type); // Create col_type
    MPI_Type_commit (&row_type);
    MPI_Type_commit (&col_type);

    /* inizializza della matrice */
    if (rank == 0){
        printf("+++  matrice  +++\n");
        for (i = 0; i < NROWS; i++) {
            for (j = 0; j < NCOLS; j++) {
                A[i][j] = 1 + j + NCOLS*i;
                printf("%2d  ", A[i][j]);
            }
            printf("\n");
        }
        printf("---------------------------\n\n");
    }

    int irow = 2;   // Index of the row we want to send
    int jcol = 1;   // Index of the col we want to send
    if (rank == 0){
        printf ("Sending row = %d, col = %d\n",irow, jcol);
        MPI_Send (&(A[irow][0]), 1, row_type, 1, 10, MPI_COMM_WORLD);
        MPI_Send (&(A[0][jcol]), 1, col_type, 1, 11, MPI_COMM_WORLD);
    } else {
        MPI_Recv (row, NCOLS, MPI_INT, 0, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv (col, NROWS, MPI_INT, 0, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // Do some printing here
        printf ("Row    = ");
        for (j = 0; j < NCOLS; j++) printf ("%2d  ", row[j]);
        printf ("\n");
        printf ("Column = ");
        for (i = 0; i < NROWS; i++) printf ("%2d  ", col[i]);
        printf ("\n");
    }
    MPI_Type_free(&row_type);
    MPI_Type_free(&col_type);

    MPI_Finalize();
    return 0;
}