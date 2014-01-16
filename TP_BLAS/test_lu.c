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
    blas_t* B;
    blas_t* C;
    alloc_matrice(&A, M,N);
	alloc_matrice(&B, M,N);

	alloc_matrice(&C, M,N);
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
    memcpy(B,A,M*N*sizeof(blas_t));
    printf("Matrice A:\n");
    affiche(M, N, A, N, stdout); 
    dgetrf_nopiv(M, N, A, N);
    printf("Matrice LU:\n");
    affiche(M, N, A, N, stdout);   
    prod_tr(A, M, C, N);
    printf("Produit LU:\n");
    affiche(M, N, C, N, stdout);
    blas_t err = erreur(B, C, M);
    printf("Erreur:%lf\n", err);
    destruction(A);
    destruction(C);
    destruction(b);
    
    M=100;
    N=100;
    alloc_matrice(&A, M,N);
    for (i=0; i<M; i++)
    {
		A[i*N+i]=1;
	}
	alloc_matrice(&B, M,N);
    memcpy(B,A,M*N*sizeof(blas_t));
    alloc_matrice(&C, M,N);

    dgetrf_nopiv(M, N, A, N);   
    prod_tr(A, M, C, N);
    err = erreur(B, C, M);

    printf("Erreur:%lf\n", err);
    //~ affiche(M, N, C, N, stdout);   
   
}
