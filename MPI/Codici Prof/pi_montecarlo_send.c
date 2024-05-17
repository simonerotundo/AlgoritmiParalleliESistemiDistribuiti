/**********************************************************************
  pi calculation - Monte Carlo
**********************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define DARTS 10000000     /* number of throws at dartboard */
#define MASTER 0        /* task ID of master task */

int main (int argc, char *argv[])
{
double	homepi,         /* value of pi calculated by current task */
	      pi,             /* average of pi after "darts" is thrown */
	      avepi,          /* average pi value for all iterations */
	      pirecv,         /* pi received from worker */
	      pisum;          /* sum of workers pi values */
int	taskid,         /* task ID - also used as seed number */
	   numtasks,       /* number of tasks */
	   source,         /* source of incoming message */ 
	   i, n;
int score=0;
int darts_slave;
double x_coord, y_coord, r; 
double starttime, endtime; 
MPI_Status status;

/* Obtain number of tasks and task ID */
MPI_Init(&argc,&argv);
MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

MPI_Barrier(MPI_COMM_WORLD);
starttime = MPI_Wtime();

/* Master receives pi values from slaves */
if (taskid == 0) {
    pisum = 0;
    for (n = 1; n < numtasks; n++) {
       MPI_Recv(&pirecv, 1, MPI_DOUBLE, MPI_ANY_SOURCE,  n, MPI_COMM_WORLD, &status);
       pisum = pisum + pirecv; /* keep summing ... */
      }
      
      pi = pisum ;  // lazy master ! :) 
   } 
else
    {
     darts_slave = DARTS/(numtasks-1);
     for (n = 1; n <= darts_slave; n++)  {
      /* generate random numbers for x and y coordinates */
      r = (double)rand()/RAND_MAX;
      x_coord =  (2.0 * r) - 1.0;
      r = (double)rand()/RAND_MAX;
      y_coord =  (2.0 * r) - 1.0;

      /* if dart lands in circle, increment score */
      if (x_coord*x_coord + y_coord*y_coord <= 1.0)
           score++;
   }
   /* calculate local pi */
   homepi = 4.0 * (double)score/(double)DARTS;
   MPI_Send(&homepi, 1, MPI_DOUBLE, MASTER, taskid, MPI_COMM_WORLD);
} // end slave   

MPI_Barrier(MPI_COMM_WORLD);
endtime = MPI_Wtime();
if (taskid == MASTER){
   printf ("\nReal value of PI: 3.1415926535897 \n");
   printf ("\nComputed value of PI: %f\n", pi );
   printf("Elapsed time : %f ms\n", 1000*(endtime - starttime));

}  
MPI_Finalize();
return 0;
}

