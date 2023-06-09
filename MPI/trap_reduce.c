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
    int ierr, procid, numprocs;
    MPI_Status status;
    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_size( MPI_COMM_WORLD, &numprocs);
    ierr = MPI_Comm_rank( MPI_COMM_WORLD, &procid);
    float a; // The lower bound of the integral
    float b; // The upper bound of the integral
    if(procid == 0){
        printf("Enter a and b: \n");
        scanf("%f %f", &a, &b);
    }
    ierr = MPI_Bcast(&a, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    ierr = MPI_Bcast(&b, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    float h = 0.0001; // Hop size
    int n = (b - a)/h; // The number of trapezoids
    // Process variables
    float local_a;
    float local_b;
    float local_n;
    float integral;
    float total = 0;

    // ---- Local integral estimation ----
    local_a = a + procid*(n/numprocs)*h;
    local_b = local_a + (n/numprocs)*h;
    local_n = n/numprocs;
    integral = Trap(local_a, local_b, local_n, h);
    // ------------------------------------    
    ierr = MPI_Reduce(&integral, &total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    if(procid == 0){
        printf("Trapezoidal estimation of f(x): %1f\n", total);
    }
    ierr = MPI_Finalize();
}