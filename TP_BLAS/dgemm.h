#ifndef DGEMM_H_
#define DGEMM_H_
#include "util.h"
void cblas_dgemm_scalaire1(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B, int ldb, int m);
void cblas_dgemm_scalaire2(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B, int ldb, int m);
void cblas_dgemm_scalaire3(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B, int ldb, int m);
void cblas_dgemm_scalaire(const int M, const int N, const int K, const blas_t alpha, const blas_t *A, const int lda, const blas_t *B, const int ldb, const blas_t beta, blas_t *C, const int ldc);
void cblas_daxpy(const int N, const blas_t alpha,const blas_t *X, const int ldx, /*@out@*/ blas_t *Y, const int ldy);
void cblas_dgemv(const enum CBLAS_ORDER order,
                 const enum CBLAS_TRANSPOSE TransA,const int M, const int N,	const blas_t alpha, const blas_t *A, const int lda, const blas_t *X, const int incX, const blas_t beta,blas_t *Y, const int incY);
void cblas_dger(const enum CBLAS_ORDER order,const int M, const int N, const blas_t alpha, const blas_t *X, const int incX, const blas_t *Y, const int incY, blas_t *A, const int lda);
void cblas_dgemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_TRANSPOSE TransB,const int M, const int N, const int K, const blas_t alpha, const blas_t *A,const int lda, const blas_t *B, const int ldb, const blas_t beta, blas_t *C, const int ldc);
#endif /*DGEMM_H_*/
