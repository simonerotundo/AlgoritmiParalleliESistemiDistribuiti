/* il prof ha dato un codice di partenza diverso (generale), ma non funzionava quindi ho fatto a modo mio */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define CPU_0 0
#define CPU_1 1
#define CPU_2 2
#define CPU_3 3
#define CPU_4 4
#define CPU_5 5

int myrank;

void doSomething() {
    printf("cpu-%d: sto facendo qualcosa..\n", myrank);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    /*
    * 1 -> si libera quando finisce 4
    * 5 -> si libera quando finiscono 3 e 2
    * 0 -> si libera quando finiscono 5 e 1
    */

    int msg=0;
    switch (myrank) {
    case CPU_0:
        /* aspetto che cpu-1 e cpu-5 finiscano */
        MPI_Recv(&msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&msg, 1, MPI_INT, 5, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        /* faccio quello che devo fare */
        doSomething();
        break;

    case CPU_1:
        /* aspetto che cpu-4 finisca */
        MPI_Recv(&msg, 1, MPI_INT, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* faccio quello che devo fare */
        doSomething();

        /* comunico a cpu-0 di aver finito */
        MPI_Send(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        break;

    case CPU_2:
        /* faccio quello che devo fare */
        doSomething();

        /* comunico a cpu-5 di aver finito */
        MPI_Send(&msg, 1, MPI_INT, 5, 0, MPI_COMM_WORLD);
        break;

    case CPU_3:
        /* faccio quello che devo fare */
        doSomething();

        /* comunico a cpu-5 di aver finito */
        MPI_Send(&msg, 1, MPI_INT, 5, 0, MPI_COMM_WORLD);
        break;

    case CPU_4:
        /* faccio quello che devo fare */
        doSomething();

        /* comunico a cpu-1 di aver finito */
        MPI_Send(&msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        break;

    case CPU_5:
        /* aspetto che cpu-2 e cpu-3 finiscano */
        MPI_Recv(&msg, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&msg, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* faccio quello che devo fare */
        doSomething();

        /* comunico a cpu-0 di aver finito */
        MPI_Send(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        break;

    default:
        break;
    }
    
    MPI_Finalize();
    return 0;
}