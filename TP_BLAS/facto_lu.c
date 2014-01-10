#include "dgemm.h"

#define MIN(x,y) (x<y)?x:y

void dscal(blas_t *X, const int M, const int incX, const blas_t alpha)
{
	int k; 
	for (k=0; k<M; k++)
	{
		X[k*incX]*= alpha;
	}
}

void dgetf2_nopiv( const int M, const int N, blas_t *A, const int lda)
{
	int k;
	for (k=0; k< MIN(M,N); k++)
	{
		dscal(A[k*lda+k+1], M-k, 1, 1/A[k*lda+k]);
		cblas_dger(CblasColMajor, M-k, N-k, alpha, A[k*lda+k+1], 1, A[(k+1)*lda+k], lda, A[k*lda+k], lda);
	}
}
