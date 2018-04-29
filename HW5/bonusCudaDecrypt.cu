#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "cuda.h"
#include "functions.c"

//compute a*b mod p safely
__device__ unsigned int modprodC(unsigned int a, unsigned int b, unsigned int p) {
  unsigned int za = a;
  unsigned int ab = 0;

  while (b > 0) {
    if (b%2 == 1) ab = (ab +  za) % p;
    za = (2 * za) % p;
    b /= 2;
  } // end while
  return ab;
} //end modprodC function

//compute a^b mod p safely
__device__ unsigned int modExpC(unsigned int a, unsigned int b, unsigned int p) {
  unsigned int z = a;
  unsigned int aExpb = 1;

  while (b > 0) {
    if (b%2 == 1) aExpb = modprodC(aExpb, z, p);
    z = modprodC(z, z, p);
    b /= 2;
  } // end while
  return aExpb;
} // end modprodC function

  __global__ void findSecretKey(unsigned int p, unsigned int g, unsigned int h, unsigned int *d_a) {
  
  int threadId = threadIdx.x;
  int blockId = blockIdx.x;
  int Nblock = blockDim.x;
 
  int id = threadId+blockId*Nblock;

  if (id < (p-1)) {
	if(modExpC(g, id, p) == h) {
		*d_a = id;
	} // end inner if
  } // end outer if

 } // end findKey


int main (int argc, char **argv) {

  /* Part 2. Start this program by first copying the contents of the main function from your completed decrypt.c main function. */
  //declare storage for an ElGamal cryptosytem
  unsigned int n, p, g, h, x;
  unsigned int Nints;

  //get the secret key from the user
  printf("Enter the secret key (0 if unknown): "); fflush(stdout);
  char stat = scanf("%u",&x);

  printf("Reading file.\n");

  /* Q3 Complete this function. Read in the public key data from public_key.txt
    and the cyphertexts from messages.txt. */
  
  FILE* pk = fopen("bonus_public_key.txt","r");
  FILE* msg = fopen("bonus_message.txt", "r");
  fscanf(pk, "%u\n%u\n%u\n%u\n", &n, &p, &g, &h);
  fclose(pk);
  fscanf(msg, "%u\n", &Nints);

  unsigned int *mhat = (unsigned int *) malloc(Nints*sizeof(unsigned int));
  unsigned int *a = (unsigned int *) malloc(Nints*sizeof(unsigned int));

   for (int i = 0; i < Nints; i++) {
    fscanf(msg, "%u %u\n", &mhat[i], &a[i]);
  } // end for

  fclose(msg);
 // } // end main

  /* Q3 After finding the secret key, decrypt the message */

  unsigned int found;
  unsigned int *d_a;

  cudaMalloc(&d_a, sizeof(unsigned int));

    if (x == 0 || modExp(g, x, p) != h) {
	printf("Finding the secret key ... \n");
  } // end if

  double startTime = clock();

  unsigned int Nthreads = 32;
  unsigned int Nblocks = (p-1)/Nthreads;

  findSecretKey <<<Nblocks, Nthreads>>>(g, h, p, d_a);
  cudaDeviceSynchronize();

  cudaMemcpy(&found, d_a, sizeof(unsigned int), cudaMemcpyDeviceToHost);

  double endTime = clock();
  double totalTime = (endTime-startTime)/CLOCKS_PER_SEC;
  double work = (double) p;
  double throughput = work/totalTime;

  printf("Searching all keys took %g seconds, throughput was %g values tested per second.\n", totalTime, throughput);

  printf("Secret key found! x = %u \n", found);

  int Nchars = ((n-1)/8)*Nints;
  unsigned char *decrypted = (unsigned char *) malloc(1024*sizeof(unsigned char));

  ElGamalDecrypt(mhat, a, Nints, p, found);
  convertZToString(mhat, Nints, decrypted, Nchars);
  printf("Decrypted message: %s\n", decrypted);
    
  cudaFree(d_a);
  /* Q4 Make the search for the secret key parallel on the GPU using CUDA. */
  
  return 0;
}
