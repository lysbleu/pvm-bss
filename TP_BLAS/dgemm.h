#ifndef DGEMM_H_
#define DGEMM_H_
#include "util.h"
void cblas_dgemm_scalaire1(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B, int ldb, int m);
void cblas_dgemm_scalaire2(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B, int ldb, int m);
void cblas_dgemm_scalaire3(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B, int ldb, int m);
void cblas_dgemm_scalaire(const int M, const int N, const int K, const blas_t alpha, const blas_t *A, const int lda, const blas_t *B, const int ldb, const double beta, blas_t *C, const int ldc);
void cblas_daxpy_scalaire(const int N, const blas_t alpha,const blas_t *X, const int ldx, /*@out@*/ blas_t *Y, const int ldy);
void cblas_dgemv_scalaire(const int M, const int N,	const blas_t alpha, const blas_t *A, const int lda, const blas_t *X, const int incX, const blas_t beta,blas_t *Y, const int incY);
void cblas_dger(const int M, const int N, const double alpha, const double *X, const int incX, const double *Y, const int incY, double *A, const int lda);
#endif /*DGEMM_H_*/
