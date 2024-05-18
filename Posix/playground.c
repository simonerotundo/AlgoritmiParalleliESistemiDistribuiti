#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define NTHREADS 4
#define NSTEPS 20

struct ant {
    int pos, dir; // 0 ovest, 1 est
};


ant* ants, local_sx, local_dx;
ant** V, localReadV, localWriteV;
int nAnts, nCells, nPerThread, myrank;
MPI_Datatype structType;

void init() {
    V = new ant*[nCells];

    for(int i = 0; i < nAnts; i++) {
        V[ants[i].pos] = &ants[i];
    }

    MPI_Type_create_struct(1, 2, 0, MPI_INT, &structType); //count, blockcounts, offset
    MPI_Type_commit(&structType);
}

void printArray(int it) {
    printf("----- ITER: %d -----\n|", it);

    for(int i = 0; i < nCells; i++) {
        printf("%d|", V[i]);
    }
    printf("\n");
}


void exchangeBorders() {
    if(myrank == 0) {

        for(int i = 1; i < NTHREADS; i++) {
            if(i == 1) {
                MPI_Send(V[i*nPerThread], 1, structType, i, 0, MPI_COMM_WORLD); // invia solo cella_dx a thread 1
            }

            else if(i == (NTHREADS-1)) {
                MPI_Send(V[(i-1)*nPerThread-1], 1, structType, i, 0, MPI_COMM_WORLD); // invia cella_sx ad ultimo thread
            }

            else {
                MPI_Send(V[(i-1)*nPerThread-1], 1, structType, i, 0, MPI_COMM_WORLD); // invia cella_sx ad ogni thread
                MPI_Send(V[i*nPerThread], 1, structType, i, 0, MPI_COMM_WORLD); // invia cella_dx ad ogni thread
            }
        }        
    }

    else {

        if(myrank == 1) {
            MPI_Recv(local_dx, 1, structType, 0, 0, MPI_COMM_WORLD);
        }

        else if(myrank == (NTHREADS-1)) {
            MPI_Recv(local_sx, 1, structType, 0, 0, MPI_COMM_WORLD);
        }

        else {
            MPI_Recv(local_sx, 1, structType, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(local_dx, 1, structType, 0, 0, MPI_COMM_WORLD);
        }
    }
}

void move(int it) {

    if(myrank == 0) {
        printArray(it);  // visualizza l'array completo aggiornato
    }  

    exchangeBorders();

    if(myrank != 0) {
        bool movedInLocal_dx = false, movedInLocal_sx = false;

        for(int i = 0; i < nPerThread; i++) {
            if(localReadV[i] != NULL) { 
                if(localReadV[i]->dir == 0) {
                    if(localReadV[i]->pos > 0 && localReadV[i-1] == NULL) { // sposta la formica in pos i-1
                        ant* tmp = localReadV[i];
                        localWriteV[i] = NULL;
                        localWriteV[i-1] = tmp;
                        localWriteV[i-1]->pos--;
                    }
                    else if(localReadV[i]->pos == 0 && myrank > 1 && local_sx == NULL) { // sposta la formica in local_sx
                        ant* tmp = localReadV[i];
                        localWriteV[i] = NULL;
                        local_sx = tmp;
                        localWriteV[i-1]->pos--;
                        movedInLocal_sx = true;
                    }
                    else localReadV[i]->dir = 1;
                }

                else {  // dir == 1
                    if(localReadV[i]->pos < (nPerThread-1) && localReadV[i+1] == NULL) { // sposta la formica in pos i+1
                        ant* tmp = localReadV[i];
                        localWriteV[i] = NULL;
                        localWriteV[i+1] = tmp;
                        localWriteV[i-1]->pos++;
                    }
                    else if(localReadV[i]->pos == (nPerThread-1) && myrank != (NTHREADS-1) && local_dx == NULL) { // sposta la formica in local_dx
                        ant* tmp = localReadV[i];
                        localWriteV[i] = NULL;
                        local_dx = tmp;                        
                        localWriteV[i-1]->pos++;
                        movedInLocal_dx = true;
                    }
                    else localReadV[i]->dir = 0;
                }
            }
        }

        if(myrank > 1 && local_sx != NULL && !movedInLocal_sx && local_sx->dir == 1 && localWriteV[0] == NULL) { // sposta la formica da local_sx a pos 0
            ant* tmp = local_sx;
            local_sx = NULL;
            localWritev[0] = tmp;                        
            localWriteV[0]->pos++;
        }

        if(myrank < (NTHREADS-1) && local_dx != NULL && !movedInLocal_dx && local_dx->dir == 0 && localWriteV[nPerThread-1] == NULL) { // sposta la formica da local_dx a pos nPerThread-1
            ant* tmp = local_dx;
            local_dx = NULL;
            localWritev[nPerThread-1] = tmp;                        
            localWriteV[nPerThread-1]->pos--;
        }

        ant** p = localReadV;
        localReadV = localWriteV;
        localWriteV = p;
    }

    // ogni thread invia il proprio localReadV a thread 0
    MPI_Gather(localReadV, nPerThread, structType, V, nCells, structType, 0, MPI_COMM_WORLD);
}


void setVariables(int numAnts, int numCells) {
    nAnts = numAnts;
    nCells = numCells;
    nPerThread = nCells / (NTHREADS-1);
}

void setPosDir(int index, int p, int d) {
    ants[index].pos = p;
    ants[index].dir = d; 

    printf("Formica %d: pos %d --- dir %d\n", ants[index].thid, ants[index].pos, ants[index].dir);
}

int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    setVariables(5, 12); // nAnts, nCells
    ants = new ant[nAnts];

    for(int i = 0; i < nAnts; i++) {
        setPosDir(i, i*2, i%2);
    }

    init();

    for(int i = 0; i < NSTEPS; i++) {
        printf("e movati\n");
        move(i);
    }

    delete [] V;
    MPI_Finalize();

    return 0;
}