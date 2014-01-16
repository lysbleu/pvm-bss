#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "facto_lu.h"


int main(int argc, char* argv[]) {
    int M, N, i;
    M= 3;
    N= 3;
    
    blas_t* A;
    blas_t* b;
    alloc_matrice(&A, M,N);
    alloc_vecteur(&b, N);
    
    for (i=0; i<M*N; i++)
    {
		A[i] = i+1;
	}
    //~ memset(A, 0, M * N * sizeof(blas_t));
    //~ for (i = 0 ; i < M ; i++) {
        //~ A[i] = 1;
        //~ if(i != M -1) {
            //~ A[N*(i + 1) + i] = i + 1;
            //~ b[i] = i+2 % N;
        //~ }
    //~ }
    affiche(M, N, A, N, stdout); 
    dgetrf_nopiv(M, N, A, N);
    affiche(M, N, A, N, stdout);    
}
