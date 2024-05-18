
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NCOLS 20
#define NROWS 20

#define v(r,c) ((r)*NCOLS+(c))

int* readM = new int[NROWS*NCOLS];
int* writeM = new int[NROWS*NCOLS];
int nsteps=50;


void init(){
	int ii = NROWS/2;
	int jj = NCOLS/2;

	for(int i=0;i<NROWS;i++)
		for(int j=0;j<NCOLS;j++)
			readM[v(i,j)]=0;

	readM[v(ii-1,jj)]=1;
	readM[v(ii,jj+1)]=1;
	readM[v(ii+1,jj-1)]=1;
	readM[v(ii+1,jj)]=1;
	readM[v(ii+1,jj+1)]=1;
}

void print(int step){
	printf("---%d\n",step);
	for(int i=0;i<NROWS;i++){
		for(int j=0;j<NCOLS;j++){
			char ch = '0';
			if (readM[v(i,j)]==1)
				ch='*';
			printf("%c ",ch);

		}
		printf("\n");
	}

}

void swap(){
	int * p=readM;
	readM=writeM;
	writeM=p;
}

void transFuncCell(int i, int j){
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

void transFunc(){

	for(int i=0;i<NROWS;i++){
		for(int j=0;j<NCOLS;j++){
			transFuncCell(i,j);
		}
	}
}

int main(int argc, char *argv[]) {

	init();

	for(int s=0;s<nsteps;s++){
		print(s);
		transFunc();
		swap();
	}

	delete[] readM;
	delete[] writeM;

}
