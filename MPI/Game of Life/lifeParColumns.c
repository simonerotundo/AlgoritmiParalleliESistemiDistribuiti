/**********************************************************************
 * life in parallel - column exchange
 *
 **********************************************************************/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NCOLS 30
#define NROWS 20

#define v(r,c) ((r)*(NCOLS/nProc+2)+(c))

int* readM;
int* writeM;
int nsteps=120;

int rank, rankLeft, rankRight;
int nProc;

MPI_Datatype newDT;



void init(){

	for(int i=0;i<NROWS;i++)
		for(int j=0;j<NCOLS/nProc+2;j++)
			readM[v(i,j)]=0;


	if (rank==0){
		int ii = NROWS/2;
		int jj = 3;
		readM[v(ii-1,jj)]=1;
		readM[v(ii,jj+1)]=1;
		readM[v(ii+1,jj-1)]=1;
		readM[v(ii+1,jj)]=1;
		readM[v(ii+1,jj+1)]=1;
	}
}

void print(int step){
	if (rank==0){
		printf("---%d\n",step);
		for(int i=0;i<NROWS;i++){
			for(int j=1;j<NCOLS/nProc+1;j++){
				char ch = '0';
				if (readM[v(i,j)]==1)
					ch='*';
				printf("%c ",ch);

			}
			printf("\n");
		}
	}

}

void swap(){
	int * p=readM;
	readM=writeM;
	writeM=p;
}


inline void exchBord(){
	MPI_Request request;
	MPI_Status status;

	MPI_Isend(&readM[v(0,NCOLS/nProc)], 1, newDT, rankRight, 17, MPI_COMM_WORLD, &request);
	MPI_Isend(&readM[v(0,1)], 1, newDT, rankLeft, 20, MPI_COMM_WORLD, &request);

	MPI_Recv(&readM[v(0,0)], 1, newDT, rankLeft, 17, MPI_COMM_WORLD, &status);
	MPI_Recv(&readM[v(0,NCOLS/nProc+1)], 1, newDT, rankRight, 20, MPI_COMM_WORLD, &status);

}

inline void transFuncCell(int i, int j){
	int cont=0;
	for(int di=-1;di<2;di++)
		for(int dj=-1;dj<2;dj++)
			if ((di!=0 || dj!=0) &&readM[v((i+di+NROWS)%NROWS,(j+dj+NCOLS)%NCOLS)]==1)
				cont++;

	if (readM[v(i,j)]==1)
		if (cont==2 || cont ==3)
			writeM[v(i,j)]=1;
		else
			writeM[v(i,j)]=0;
	else
		if (cont ==3)
			writeM[v(i,j)]=1;
		else
		writeM[v(i,j)]=0;
}


inline void transFunc(){

	for(int i=0;i<NROWS;i++){
		for(int j=1;j<NCOLS/nProc+1;j++){
			transFuncCell(i,j);

		}

	}
}

int main(int argc, char *argv[]) {


	MPI_Init(&argc, &argv);               /* Initialize MPI               */
	MPI_Comm_size(MPI_COMM_WORLD, &nProc); /* Get the number of processors */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* Get my number                */

	readM = new int[NROWS*(NCOLS/nProc+2)];
	writeM = new int[NROWS*(NCOLS/nProc+2)];

	rankLeft = (rank-1+nProc)%nProc;
	rankRight = (rank+1)%nProc;


	MPI_Type_vector(NROWS, 1, NCOLS/nProc+2, MPI_INT, &newDT);
	MPI_Type_commit(&newDT);

	init();

	double starttime = MPI_Wtime();

	for(int s=0;s<nsteps;s++){
		exchBord();
		print(s);
		transFunc();
		swap();
	}

	double endtime = MPI_Wtime();

	if (rank == 0){
		printf("Elapsed time: %f\n", 1000*(endtime - starttime));
	}

	delete[] writeM;
	delete[] readM;

	MPI_Finalize();

	return 0;
}
