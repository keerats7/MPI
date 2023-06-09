#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]){
    // Have Process 0 send Process 1 some pi...
    int ierr, procid, numprocs;

    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_size( MPI_COMM_WORLD, &numprocs);
    if(numprocs != 2){
        printf("There are %d processes. There can be no more than 2.", numprocs);
        return MPI_Abort(MPI_COMM_WORLD, 1);
    }
    ierr = MPI_Comm_rank( MPI_COMM_WORLD, &procid);
    if(procid == 0){
        // Send
        double pi = 3.14;
        ierr = MPI_Send(&pi, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
        printf("Process %d sent %1f to process 1.\n", procid, pi);
    }
    else {
        // Recieve
        double value;
        MPI_Status status;
        ierr = MPI_Recv(&value, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        if(ierr == MPI_SUCCESS)
            printf("Process %d recieved %1f from process 0.\n", procid, value);
        else
            printf("Process %d did not recieve a value...", procid);
    }
    
    ierr = MPI_Finalize();
}