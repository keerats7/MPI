#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[]){
    // Write a program that reads an integer value from the terminal and distributes the value to all of the MPI processes. 
    // Each process should print out its rank and the value it received. 
    // Values should be read until a negative integer is given as input.
    int ierr, procid, numprocs;
    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_size( MPI_COMM_WORLD, &numprocs);
    ierr = MPI_Comm_rank( MPI_COMM_WORLD, &procid);
    int val;
    while(1){
        if(procid == 0){
                scanf("%d", &val);
                if (val < 0) MPI_Abort(MPI_COMM_WORLD, 1);
            }
        ierr = MPI_Bcast(&val, 1, MPI_INT, 0, MPI_COMM_WORLD);
        printf("Process %d recieved %d!\n", procid, val);
        fflush( stdout );
    }
    ierr = MPI_Finalize();
}