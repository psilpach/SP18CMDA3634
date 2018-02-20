#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <mpi.h>

int main(int argc, char **argv) {

 MPI_Init(&argc, &argv);

 int rank, size;
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 MPI_Comm_size(MPI_COMM_WORLD, &size);

 // need running tallies
 long long int Ntotal;
 long long int Ncircle;

 // d. user srand48 to set the random number seed to be the MPI process rank
 double seed = rank;
 srand48(seed);

 for (long long int n=0; n<10000000; n++) {
	// generate two random numbers
	double rand1 = drand48(); //drand48 returns a number between 0 and 1
	double rand2 = drand48();

	double x = -1 + 2*rand1; //shift to [-1,1]
	double y = -1 + 2*rand2;

	//check if its in the circle
	if (sqrt(x*x+y*y)<=1) Ncircle++; 
	Ntotal++; // keep a running tally
 }

 
 double pi = 4.0*Ncircle/ (double) Ntotal;

 printf("Our estimate of pi is %f \n", pi);

 return 0;

 // e. use an all reduce operation

  long long int global;
  //double sum;

  MPI_Allreduce(
    &pi,
    &global,
    1,  
    MPI_DOUBLE,
    MPI_SUM,
    MPI_COMM_WORLD);

 MPI_Finalize();

}

