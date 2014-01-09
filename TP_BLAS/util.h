#ifndef UTIL_H_
#define UTIL_H_
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
typedef double blas_t;
#define CBLAS_INDEX   size_t

enum  	CBLAS_ORDER { CblasRowMajor = 101, CblasColMajor = 102 };
enum  	CBLAS_TRANSPOSE { CblasNoTrans = 111, CblasTrans = 112, CblasConjTrans = 113 };
enum  	CBLAS_UPLO { CblasUpper = 121, CblasLower = 122 };
enum  	CBLAS_DIAG { CblasNonUnit = 131, CblasUnit = 132 };
enum  	CBLAS_SIDE { CblasLeft = 141, CblasRight = 142 };

void affiche(int m, int n, blas_t *a, int lda, FILE* flux);
void alloc_vecteur(blas_t **vecteur, int taille);
void alloc_matrice(blas_t **matrice , int m, int n);
void destruction(blas_t *matrice);

#endif /*UTIL_H_*/
