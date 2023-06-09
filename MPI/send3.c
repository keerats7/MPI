#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]){
    // Each process will send process 0 the value -procid, process 0 will sum it all up and return.
    int ierr, procid, numprocs;

    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_size( MPI_COMM_WORLD, &numprocs);
    ierr = MPI_Comm_rank( MPI_COMM_WORLD, &procid);
    if(procid != 0){
        // Send
        double send_val = -procid;
        ierr = MPI_Send(&send_val, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        printf("Process %d sent %1f to process 0.\n", procid, send_val);
    }
    else{
        // Recieve
        double total = 0;
        for(int i = 1; i < numprocs; i++){
            double rec_val;
            MPI_Status status;
            ierr = MPI_Recv(&rec_val, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
            if(ierr == MPI_SUCCESS){
                total += rec_val;
                printf("Process 0 recieved %1f from process %d.\n", rec_val, i);
            }
            else
                printf("Process 0 did not recieve a value from process %d...\n", procid);
        }
        printf("Total: %1f\n", total);
    }
    
    ierr = MPI_Finalize();
}