#include <stdio.h>
#include <math.h>
#include "mpi.h"

/* This example handles a 12 x 12 mesh, on 4 processors only. */
#define maxn 12

int main(int argc,char* argv[]) {
    // This assignment builds on the simple parallel data structure.
    // Now we will implement a simple Jacobi iteration for approximating the solution to a linear system of equations.
    int ierr, rank, size;
    int value, itcount;
    double local_diffnorm, total_diffnorm;
    double x[12][12];
    double xlocal[(12/4)+2][12];
    double xnew[12/4][12];
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
        if(rank == 0) xlocal[1][j] = -1;
        if(rank == size - 1) xlocal[(maxn/size)][j] = -1;
    }
    MPI_Gather( xlocal[1], maxn * (maxn/size), MPI_DOUBLE,
		x, maxn * (maxn/size), MPI_DOUBLE, 
		0, MPI_COMM_WORLD );
    if (rank == 0) {
        printf( "Initial array is\n" );
        for (int i=maxn-1; i>=0; i--) {
            for (int j=0; j<maxn; j++) 
            printf( "%f ", x[i][j] );
            printf( "\n" );
        }
    }
    itcount = 0;
    do{
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
        // Jacobi iteration
        //int i_local = 1;
        local_diffnorm = 0;
        for(int i = 1; i <= maxn/size; i++){
            if((rank == 0 && i == 1) || (rank == size - 1 && i == maxn/size)) continue;
            for(int j = 1; j < maxn - 1; j++){
                xnew[i][j] = (xlocal[i+1][j] + xlocal[i-1][j] + xlocal[i][j+1] + xlocal[i][j-1])/4;
                local_diffnorm += (xnew[i][j] - xlocal[i][j]) * (xnew[i][j] - xlocal[i][j]);
            }
        }
        // Transfer interior points
        for(int i = 1; i <= maxn/size; i++){
            if((rank == 0 && i == 1) || (rank == size - 1 && i == maxn/size)) continue;
            for(int j = 1; j < maxn - 1; j++){
                xlocal[i][j] = xnew[i][j];
            }
        }
        //printf("Process %d: %1f\n", rank, local_diffnorm);
        ierr = MPI_Allreduce(&local_diffnorm, &total_diffnorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        total_diffnorm = sqrt(total_diffnorm);
        itcount++;
        if(rank == 0){
            printf("Iteration %d: %1f\n", itcount, total_diffnorm);
        }
    } while(total_diffnorm > .01 && itcount < 100);
    ierr = MPI_Gather(&xlocal[1], maxn * (maxn/size), MPI_DOUBLE, &x+rank*(maxn/size), maxn * (maxn/size), MPI_DOUBLE, 0, MPI_COMM_WORLD );
    if (rank == 0) {
        printf( "Final solution is\n" );
        for (int i=maxn-1; i>=0; i--) {
            for (int j=0; j<maxn; j++) 
            printf( "%f ", x[i][j] );
            printf( "\n" );
        }
    }
    ierr = MPI_Finalize();
}