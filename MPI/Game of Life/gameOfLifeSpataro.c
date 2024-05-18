/**********************************************************************
 * 
 *  				game of life in parallelo
 *
 **********************************************************************/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NCOLS 50
#define NROWS 50

#define v(r,c) ((r)*NCOLS+(c))

int* readM;
int* writeM;
int nsteps = 150;

int rank, rankUp, rankDown;
int nProc;

void init(){
    for(int i = 0; i < NROWS/nProc + 2; i++)
        for(int j = 0; j < NCOLS; j++)
            readM[v(i, j)] = 0;

    if (rank == 0){
        int ii = 3;
        int jj = NCOLS/2;
        readM[v(ii-1, jj)] = 1;
        readM[v(ii, jj+1)] = 1;
        readM[v(ii+1, jj-1)] = 1;
        readM[v(ii+1, jj)] = 1;
        readM[v(ii+1, jj+1)] = 1;
    }
}

void print(int step){
    if (rank == 0){
        printf("---%d\n", step);
        for(int i = 1; i < NROWS/nProc + 1; i++){
            for(int j = 0; j < NCOLS; j++){
                char ch = ' ';
                if (readM[v(i, j)] == 1)
                    ch = '*';
                printf("%c ", ch);
            }
            printf("\n");
        }
        printf("end print\n");
    }
}

void swap(){
    int* p = readM;
    readM = writeM;
    writeM = p;
}

void exchBord(){
    MPI_Request request;
    MPI_Status status;

    MPI_Isend(&readM[v(NROWS/nProc, 0)], NCOLS, MPI_INT, rankDown, 17, MPI_COMM_WORLD, &request);
    MPI_Isend(&readM[v(1, 0)], NCOLS, MPI_INT, rankUp, 20, MPI_COMM_WORLD, &request);
    MPI_Recv(&readM[v(NROWS/nProc + 1, 0)], NCOLS, MPI_INT, rankDown, 20, MPI_COMM_WORLD, &status);
    MPI_Recv(&readM[v(0, 0)], NCOLS, MPI_INT, rankUp, 17, MPI_COMM_WORLD, &status);
}

void transFuncCell(int i, int j){
    int cont = 0;
    for(int di = -1; di < 2; di++)
        for(int dj = -1; dj < 2; dj++)
            if ((di != 0 || dj != 0) && readM[v((i + di + NROWS) % NROWS, (j + dj + NCOLS) % NCOLS)] == 1)
                cont++;

    if (readM[v(i, j)] == 1) {
        if (cont == 2 || cont == 3)
            writeM[v(i, j)] = 1;
        else
            writeM[v(i, j)] = 0;
    } else {
        if (cont == 3)
            writeM[v(i, j)] = 1;
        else
            writeM[v(i, j)] = 0;
    }
}

void transFunc(){
    for(int i = 1; i < NROWS/nProc + 1; i++){
        for(int j = 0; j < NCOLS; j++){
            transFuncCell(i, j);
        }
    }
}

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);                     /* Initialize MPI               */
    MPI_Comm_size(MPI_COMM_WORLD, &nProc);      /* Get the number of processors */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);       /* Get my number                */

    rankUp = (rank - 1 + nProc) % nProc;
    rankDown = (rank + 1) % nProc;

    readM = (int*)malloc((NROWS/nProc + 2) * NCOLS * sizeof(int));
    writeM = (int*)malloc((NROWS/nProc + 2) * NCOLS * sizeof(int));

    init();

    for(int s = 0; s < nsteps; s++){
        system("clear");

        exchBord();
        print(s);
        transFunc();
        swap();

        usleep(10000);
    }

    free(readM);
    free(writeM);

    MPI_Finalize();

    return 0;
}
