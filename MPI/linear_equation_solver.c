#include <stdio.h>
#include <math.h>
#include "mpi.h"

/* This example handles a 12 x 12 mesh, on 4 processors only. */
#define maxn 12

int main(int argc,char* argv[]) {
    // This program reads a data file for a 12x12 matrix;
    // The linera equations are solved using Jacobi iteration implemented in parallel;
    // Honestly, I don't understand how the output relates to the solution of the linear equation;
    // I would expect a 1x12 array for x1..x12, but I'm getting a 12x12 array...
    // Although, this matches what I find online for performing Jacobi iteration on this input
    // I'm following the formulas but need to better understand their context.
    FILE *input;
    input = fopen("in.dat", "r");
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
    // Process 0 reads and distributes input
    if(rank == 0){
        for(int i = 0; i < maxn; i++){
            for(int j = 0; j < maxn; j++){
                fscanf( input, "%lf", &x[i][j] );
            }
            fscanf(input, "\n");
        }
    }
    ierr = MPI_Scatter(&x, maxn * (maxn/size), MPI_DOUBLE, &xlocal[1], maxn * (maxn/size), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    /*for(int i = 1; i <= maxn/size; i++){
        for(int j = 0; j < maxn; j++){
            xlocal[i][j] = rank;
        }
    }*/
    for(int j = 0; j < maxn; j++){
        xlocal[0][j] = -1;
        xlocal[maxn/size + 1][j] = -1;
    }
    MPI_Gather( xlocal[1], maxn * (maxn/size), MPI_DOUBLE,
		x, maxn * (maxn/size), MPI_DOUBLE, 
		0, MPI_COMM_WORLD );
    if (rank == 0) {
        printf( "Initial array is\n" );
        for (int i=0; i<=maxn -1; i++) {
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
        for (int i=0; i<=maxn -1; i++) {
            for (int j=0; j<maxn; j++) 
            printf( "%f ", x[i][j] );
            printf( "\n" );
        }
    }
    ierr = MPI_Finalize();
}