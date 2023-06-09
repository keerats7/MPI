#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]){
    // Have each process print 'Hello World!' and their process number.
    int ierr, procid, numprocs;

    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_size( MPI_COMM_WORLD, &numprocs); // numprocs will store how many process are running
    ierr = MPI_Comm_rank( MPI_COMM_WORLD, &procid); // Tell me my name...I am a process running, that number gets stored in procid

    printf("Hello world! I am process %d out of %d!\n", 
            procid, numprocs);
    
    ierr = MPI_Finalize();
}