#include <stdio.h>
#include <mpi.h>

int wrap(int procid, int numprocs){
    if(procid ==  -1){
        return numprocs - 1;
    }
    else if(procid == numprocs){
        return 0;
    }
    return procid;
}

int main(int argc, char *argv[]){
    // Have Process i send Process i+1(last process sends to process 0) some pi+i...
    int ierr, procid, numprocs;

    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_size( MPI_COMM_WORLD, &numprocs);
    ierr = MPI_Comm_rank( MPI_COMM_WORLD, &procid);
    // Send
    double send_val = 3.14 + procid;
    int send_add = wrap(procid + 1, numprocs);
    ierr = MPI_Send(&send_val, 1, MPI_DOUBLE, send_add, 0, MPI_COMM_WORLD);
    printf("Process %d sent %1f to process %d.\n", procid, send_val, send_add);
    // Recieve
    double rec_val;
    int rec_add = wrap(procid - 1, numprocs);
    MPI_Status status;
    ierr = MPI_Recv(&rec_val, 1, MPI_DOUBLE, rec_add, 0, MPI_COMM_WORLD, &status);
    if(ierr == MPI_SUCCESS)
        printf("Process %d recieved %1f from process %d.\n", procid, rec_val, rec_add);
    else
        printf("Process %d did not recieve a value...", procid);
    
    ierr = MPI_Finalize();
}