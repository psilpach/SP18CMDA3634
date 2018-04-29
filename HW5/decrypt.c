#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "functions.h"


int main (int argc, char **argv) {

  //declare storage for an ElGamal cryptosytem
  unsigned int n, p, g, h, x;
  unsigned int Nints;

  //get the secret key from the user
  printf("Enter the secret key (0 if unknown): "); fflush(stdout);
  char stat = scanf("%u",&x);

  printf("Reading file.\n");

  /* Q3 Complete this function. Read in the public key data from public_key.txt
    and the cyphertexts from messages.txt. */
  
  FILE* pk = fopen("public_key.txt","r");
  fscanf(pk, "%u\n%u\n%u\n%u\n", &n, &p, &g, &h);
  fclose(pk);

  // find the secret key
  if (x==0 || modExp(g,x,p)!=h) {
    printf("Finding the secret key...\n");
    double startTime = clock();
    for (unsigned int i=0;i<p-1;i++) {
      if (modExp(g,i+1,p)==h) {
        printf("Secret key found! x = %u \n", i+1);
        x=i+1;
      } 
    }
    double endTime = clock();

    double totalTime = (endTime-startTime)/CLOCKS_PER_SEC;
    double work = (double) p;
    double throughput = work/totalTime;

    printf("Searching all keys took %g seconds, throughput was %g values tested per second.\n", totalTime, throughput);
  }

  /* Q3 After finding the secret key, decrypt the message */
  FILE* msg = fopen("messages.txt", "r");
  fscanf(msg, "%u\n", &Nints);

  unsigned int *mhat = (unsigned int *) malloc(Nints*sizeof(unsigned int));
  unsigned int *a = (unsigned int *) malloc(Nints*sizeof(unsigned int));

  for (int i = 0; i < Nints; i++) {
    fscanf(msg, "%u %u\n", &mhat[i], &a[i]);
  } 
  fclose(msg);

  unsigned char *decrypted = (unsigned char *) malloc(1024*sizeof(unsigned char));
  unsigned int charsPerInt = sizeof(p) / sizeof(n);
  unsigned int Nchars = Nints * charsPerInt;

  ElGamalDecrypt(mhat, a, Nints, p, x);  

  convertZToString(mhat, Nints, decrypted, Nchars);
  printf("Decrypted message: %s\n", decrypted);
    
return 0;
 
}
