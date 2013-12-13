#ifndef DGEMM_H_
#define DGEMM_H_
#include "util.h"
void cblas_dgemm_scalaire1(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B, int ldb, int m);
void cblas_dgemm_scalaire2(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B, int ldb, int m);
void cblas_dgemm_scalaire3(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B, int ldb, int m);


#endif /*DGEMM_H_*/
