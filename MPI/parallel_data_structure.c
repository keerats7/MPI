#include <stdio.h>
#include "mpi.h"

/* This example handles a 12 x 12 mesh, on 4 processors only. */
#define maxn 12

int main(int argc,char* argv[]) {
    // This assignment implements a simple parallel data structure.
    int ierr, rank, size;
    int value, errcnt, toterr;
    double xlocal[(12/4)+2][12];
    double x[12][12];
    MPI_Status status;
    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_size( MPI_COMM_WORLD, &size);
    ierr = MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    // Each processor handles x[rank*(maxn/size):(rank + 1)(maxn/size)][maxn]
    // Row 0 and row 4 of xlocal are ghost points
    for(int i = 1; i <= maxn/size; i++){
        for(int j = 0; j < maxn; j++){
            xlocal[i][j] = rank;
        }
    }
    for(int j = 0; j < maxn; j++){
        xlocal[0][j] = -1;
        xlocal[maxn/size + 1][j] = -1;
    }
    // Send my upper column to the process above me
    // Send my lower column to the process below me
    // Recieve my upper ghost column from the process above me
    // Recieve my lower ghost column from the process below me
    if(rank > 0){
        ierr = MPI_Send(xlocal[1], maxn, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
    }
    if(rank < size - 1){
        ierr = MPI_Send(xlocal[maxn/size], maxn, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
        ierr = MPI_Recv(xlocal[maxn/size + 1], maxn, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &status);
    }
    if(rank > 0){
        ierr = MPI_Recv(xlocal[0], maxn, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &status);
        if(ierr != MPI_SUCCESS)
            printf("Process %d", rank);
    }
    // Check correctness of results
    errcnt = 0;
    for(int i = 1; i <= maxn/size; i++){
        for(int j = 0; j < maxn; j++){
            if(xlocal[i][j] != rank) errcnt++;
        }
    }
    for(int j = 0; j < maxn; j++){
        if(rank > 0 && xlocal[0][j] != rank - 1) errcnt++;
        if(rank < size - 1 && xlocal[maxn/size + 1][j] != rank + 1) errcnt++;
    }
    ierr = MPI_Reduce(&errcnt, &toterr, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if(rank == 0){
        printf("Total number of errors: %d\n", toterr);
    }
    MPI_Gather( xlocal[1], maxn * (maxn/size), MPI_DOUBLE,
		x, maxn * (maxn/size), MPI_DOUBLE, 
		0, MPI_COMM_WORLD );
    if (rank == 0) {
	printf( "Total array is\n" );
	for (int i=maxn-1; i>=0; i--) {
	    for (int j=0; j<maxn; j++) 
		printf( "%f ", x[i][j] );
	    printf( "\n" );
	}
    }
    ierr = MPI_Finalize();
}