#ifndef UTIL_H_
#define UTIL_H_
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
typedef double blas_t;
#define CBLAS_INDEX   size_t

//~ #ifndef __INTEL_MKL__
//~ #endif

void affiche(int m, int n, blas_t *a, int lda, FILE* flux);
void alloc_vecteur(blas_t **vecteur, int taille);
void alloc_matrice(blas_t **matrice , int m, int n);
void destruction(blas_t *matrice);

#endif /*UTIL_H_*/
