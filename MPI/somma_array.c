#include <stdio.h>
#include <mpi.h>

void main(int argc, char *argv[]) {

    int err, nproc, myid;
    MPI_Status status;
    int a[10];
    int s1[5]; int s2[5];
    int sum1, sum2;

    err = MPI_Init(&argc, &argv);
    err = MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    err = MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    /** MASTER **/
    if (myid == 0) {
        
        /* assegno i valori delle celle dell'array */
        for (int i=0; i<10; i++) {
            int v = i;
            a[i] = v;
        }

        /**  INVIO  **/
        /* invio il chunk destinato al processore 1 */
        for (int i=0; i<5; i++) {
            s1[i] = a[i];
        }
        MPI_Send(s1, 5, MPI_INT, 1, 1, MPI_COMM_WORLD);
        
        /* invio il chunk destinato al processore 2 */
        for (int i=0; i<5; i++) {
            s2[i] = a[i];
        }
        MPI_Send(s2, 5, MPI_INT, 2, 2, MPI_COMM_WORLD);


        /**  RICEZIONE  **/
        /* ricevo la somma calcolata dal processore 1 */
        MPI_Recv(&sum1, 1, MPI_INT, 1, 3, MPI_COMM_WORLD, &status);

        /* ricevo la somma calcolata dal processore 2 */
        MPI_Recv(&sum2, 1, MPI_INT, 2, 4, MPI_COMM_WORLD, &status);

        int somma_totale = sum1 + sum2;
        printf("\n\n###  SOMMA TOTALE: %d  ###\n\n", somma_totale);

    }
    
    /** SLAVE 1 **/
    else if (myid == 1) {

        /* ricevo l'array e lo stampo*/
        MPI_Recv(s1, 5, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

        /* calcolo la somma */
        int sum = 0;
        int len_s1 = sizeof(s1) / sizeof(s1[0]);
        for (int i=0; i<len_s1; i++){
            sum += s1[i];
        }

        printf("\n###  ID:  %d - somma: %d  ###\n",  myid, sum);
        printf(" s1[0]:   %d    \n", s1[0]);
        printf(" s1[1]:   %d    \n", s1[1]);
        printf(" s1[2]:   %d    \n", s1[2]);
        printf(" s1[3]:   %d    \n", s1[3]);
        printf(" s1[4]:   %d    \n", s1[4]);

        /* invio la somma al master */
        MPI_Send(&sum, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);

    }
    
    /** SLAVE 2 **/
    else if (myid == 2) {

        /* ricevo l'array e lo stampo*/
        MPI_Recv(s2, 5, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

        /* calcolo la somma */
        int sum = 0;
        int len_s2 = sizeof(s2) / sizeof(s2[0]);
        for (int i=0; i<len_s2; i++){
            sum += s2[i];
        }

        printf("\n###  ID:  %d - somma: %d  ###\n",  myid, sum);
        printf(" s2[0]:   %d    \n", s2[0]);
        printf(" s2[1]:   %d    \n", s2[1]);
        printf(" s2[2]:   %d    \n", s2[2]);
        printf(" s2[3]:   %d    \n", s2[3]);
        printf(" s2[4]:   %d    \n", s2[4]);

        /* invio la somma al master */
        MPI_Send(&sum, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
        
    }
    
    /** ALTRI SLAVE (non previsti) **/
    else {
        /* pass */
    }

    err = MPI_Finalize();

}