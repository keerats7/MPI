#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int min(int a, int b){
    return a < b ? a : b;
}
int max(int a, int b){
    return a > b ? a : b;
}

int main(int argc, char* argv[]){
    // Write a program that takes data from process zero and sends it to all of the other processes by sending it in a ring. 
    // That is, process i should receive the data and send it to process i+1, until the last process is reached.
    int ierr, procid, numprocs;
    MPI_Status status;
    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_size( MPI_COMM_WORLD, &numprocs);
    ierr = MPI_Comm_rank( MPI_COMM_WORLD, &procid);
    int val;
    if(procid == 0){
        scanf("%d", &val);
        ierr = MPI_Send(&val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    ierr = MPI_Recv(&val, 1, MPI_INT, max(procid - 1, 0), 0, MPI_COMM_WORLD, &status);
    ierr = MPI_Send(&val, 1, MPI_INT, min(procid + 1, numprocs - 1), 0, MPI_COMM_WORLD);
    if(procid == numprocs - 1) printf("Process %d recieved %d\n", procid, val);
    ierr = MPI_Finalize();
}