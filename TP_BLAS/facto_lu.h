#ifndef FACTO_LU_H_
#define FACTO_LU_H_
#include "dgemm.h"

void dgetf2_nopiv( const int M, const int N, blas_t *A, const int lda);
void dscal(blas_t *X, const int M, const int incX, const blas_t alpha);
void dtrsm(const enum CBLAS_SIDE side, const enum CBLAS_UPLO uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG diag, const int M, const int N, const blas_t alpha, const blas_t *A, const int lda, blas_t *B, const int ldb);  
int dgesv_nopiv(const int N, int nrhs, blas_t* A, const int lda, blas_t* B, const int ldb);
int dgetrs_nopiv(const int N, const int NRHS, double* A, const int lda, double* B, const int ldb);
int dgetrf_nopiv(const int M, const int N, blas_t* A, const int lda);
void prod_tr(blas_t *A, const int M, blas_t *C, const int lda);
blas_t erreur(blas_t *B, blas_t *C, const int M);

#endif /*FACTO_LU_H_*/
