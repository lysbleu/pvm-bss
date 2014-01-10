#ifndef FACTO_LU_H_
#define FACTO_LU_H_

void dgetf2_nopiv( const int M, const int N, blas_t *A, const int lda);
void dscal(blas_t *X, const int M, const int incX, const blas_t alpha);

#endif /*FACTO_LU_H_*/
