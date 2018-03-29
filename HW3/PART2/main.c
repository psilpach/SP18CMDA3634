
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "mpi.h"
#include "functions.h"

int main (int argc, char **argv) {

  MPI_Init(&argc,&argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  //seed value for the randomizer 
  double seed = clock()+rank; //this will make your program run differently everytime
  //double seed = rank; //uncomment this and your program will behave the same everytime it's run

  srand(seed);

  //begin with rank 0 getting user's input
  unsigned int n;

  /* Q3.1 Make rank 0 setup the ELGamal system and
    broadcast the public key information */

  //storage for an ElGamal cryptosytem
  unsigned int p, g, h, x;
 
  if(rank == 0) {
  // commented out to get rid of user input for section 4.2
  printf("Enter a number of bits: "); fflush(stdout);
  char status = scanf("%u",&n);

  /* Q4.2 alter number of bits instead of allowing user to input */
  
  //n = 20; 
  // for 1: 4.880337
  // for 2: 3.895412, 3.223508
  // for 4: 4.186997, 5.032037, 5.184166, 5.239378
  // for 8: 4.83, 5.01, 5.2, 5.16, 5.23, 5.29, 5.35
  // for 12: 5.6, 5.7
  // for 16: 4.3, 4.28, 4.16
  // for 20: 3.98, 4.01, 4.038
  
  //make sure the input makes sense
  if ((n<3)||(n>31)) {	//Updated bounds. 2 is no good, 31 is actually ok
    printf("Unsupported bit size.\n");
    return 0;   
  }
  printf("\n");

  //setup an ElGamal cryptosystem
  setupElGamal(n,&p,&g,&h,&x);
 
}

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast(&g, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast(&h, 1, MPI_INT, 0, MPI_COMM_WORLD); 

  //Suppose we don't know the secret key. Use all the ranks to try and find it in parallel
  if (rank==0)
    printf("Using %d processes to find the secret key...\n", size);

  /*Q3.2 We want to loop through values i=0 .. p-2
     determine start and end values so this loop is 
     distributed amounst the MPI ranks  */

  unsigned int N = p; //-1; total loop size
  unsigned int start, end;

  unsigned int cntr = N/size;
  unsigned int rmndr = N%size;

  if (rank < rmndr) {

	start = rank *(cntr +1);
	end = start +cntr;
  } else {
  
	start = rank*cntr+rmndr;
	end = start+(cntr -1);
  }

  double startTime = MPI_Wtime();
  
  
  //loop through the values from 'start' to 'end'
  for (unsigned int i=start;i<end;i++) {
    if (modExp(g,i+1,p)==h)
      printf("Secret key found! x = %u \n", i+1);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  if(rank == 0) { 
  double endTime = MPI_Wtime();
  double totalTime = endTime - startTime;
  double thru = N/totalTime;
  printf("total time: %f. \n", totalTime);
  printf("Throughput quantities: %f. \n", thru);
  }

  MPI_Finalize();

  return 0;
}
