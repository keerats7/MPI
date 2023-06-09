#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[]){
    // Write a program to test how fair the message passing implementation is.
    // To do this, have all processes except process 0 send 100 messages to process 0.
    // Have process 0 print out the messages as it receives them
    int ierr, procid, numprocs;
    MPI_Status status;
    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_size( MPI_COMM_WORLD, &numprocs);
    ierr = MPI_Comm_rank( MPI_COMM_WORLD, &procid);
    int buf;
    if(procid != 0){
        for(int i = 0; i < 100; i++){
            ierr = MPI_Send(&buf, 1, MPI_INT, 0, i, MPI_COMM_WORLD);
        }
    }
    else{
        for(int i = 0; i < 100*(numprocs - 1); i++){
            ierr = MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("Message recieved from process %d with tag %d\n", status.MPI_SOURCE, status.MPI_TAG);
        }
    }
    ierr = MPI_Finalize();
}