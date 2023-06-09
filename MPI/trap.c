#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

float f(float x){
    // **THE Parabola**
    return x*x;
}

float Trap(float a, float b, int n, float h){
    float integral;
    float x;
    int i;

    integral = (f(a) + f(b))/2;
    x = a;
    for(i = 1; i <= n - 1; i++){
        x += h;
        integral += f(x);
    }
    return integral*h;
}

int main(int argc, char* argv[]){
    // Estimate the integral from a to b of f(x) using the trapezoidal rule
    // Each process will calculate its part of the integral and send to process 0
    // Process 0 will recieve all parts and print the total estimate of the integral
    if(argc != 3){
        printf("%d", argc);
        printf("Give 2 command-line arguments: [left bound] [right bound]\n");
        exit(0);
    }
    float a = atof(argv[1]); // The lower bound of the integral
    float b = atof(argv[2]); // The upper bound of the integral
    float h = 0.0001; // Hop size
    int n = (b - a)/h; // The number of trapezoids
    // Process variables
    float local_a;
    float local_b;
    float local_n;
    float integral;
    float total = 0;
    int ierr, procid, numprocs;

    MPI_Status status;
    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_size( MPI_COMM_WORLD, &numprocs);
    ierr = MPI_Comm_rank( MPI_COMM_WORLD, &procid);
    // ---- Local integral estimation ----
    local_a = a + procid*(n/numprocs)*h;
    local_b = local_a + (n/numprocs)*h;
    local_n = n/numprocs;
    integral = Trap(local_a, local_b, local_n, h);
    // ------------------------------------
    ierr = MPI_Send(&integral, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    
    if(procid == 0){
        float value;
        for(int i = 0; i < numprocs; i++){
            ierr = MPI_Recv(&value, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status);
            if(ierr == MPI_SUCCESS){
                total += value;
                printf("Process 0 received %1f from process %d.\n", value, i);
            }
            else{
                printf("Process 0 did not receive a value from process %d.\n", i);
                return MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        printf("Trapezoidal estimation of f(x): %1f\n", total);
    }
    ierr = MPI_Finalize();
}