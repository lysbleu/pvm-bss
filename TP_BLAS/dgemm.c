#include "util.h"
void cblas_dgemm_scalaire1(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B, int ldb, int m)
{
	int i,j,k;
	for (k=0; k<m; k++)
	{
		for (i=0; i<m; i++)
		{
			for(j=0; j<m; j++)
			{	
				C[j*ldc+i] += A[k+i*lda]*B[k+j*ldb];
			}
		}
	}
}

void cblas_dgemm_scalaire2(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B, int ldb, int m)
{
	int i,j,k;
	for (i=0; i<m; i++)
	{
		for (j=0; j<m; j++)
		{
			for(k=0; k<m; k++)
			{	
				C[j*ldc+i] += A[k+i*lda]*B[k+j*ldb];
			}
		}
	}
}

void cblas_dgemm_scalaire3(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B, int ldb, int m)
{
	int i,j,k;
	for (j=0; j<m; j++)
	{
		for (i=0; i<m;i++)
		{
			for(k=0; k<m; k++)
			{	
				C[j*ldc+i] += A[i+k*lda]*B[k+j*ldb];
			}
		}
	}
}

void cblas_dgemm_scalaire(const int M, const int N,
                 const int K, const blas_t alpha, const blas_t *A,
                 const int lda, const blas_t *B, const int ldb,
                 const double beta, blas_t *C, const int ldc)
{
	int i,j,k;
	for (j=0; j<N; j++)
	{
		for (i=0; i<M;i++)
		{
			C[j*ldc+i]*=beta;
			for(k=0; k<K; k++)
			{	
				C[j*ldc+i] += alpha*A[k*lda+i]*B[k+j*ldb];
			}
		}
	}
}

void cblas_dgemv_scalaire(const int M, const int N,
						const blas_t alpha, const blas_t *A, const int lda,
						const blas_t *X, const int incX, const blas_t beta,
						blas_t *Y, const int incY)
{
	int i,j,k;
	for (j=0; j<N; j++)
	{
		for (i=0; i<M;i++)
		{	
			Y[j*incY]=0;
			for(k=0; k<N; k++)
			{	
				Y[j*incY] += A[k*lda+i]*X[k*incX];
			}
			Y[j*incY]*=alpha*beta;
		}
	}
}

void cblas_daxpy_scalaire(const int N, const blas_t alpha,const blas_t *X, const int ldx,
                 /*@out@*/ blas_t *Y, const int ldy)
{
	int i;
	for (i=0; i<N; i++)
	{
		Y[i*ldy] += alpha*X[i*ldx];
	}
}

void cblas_dger(const int M, const int N,
                const double alpha, const double *X, const int incX,
                const double *Y, const int incY, double *A, const int lda)
{
	int i,j;
	for(i=0; i< N; i++)
	{
		for(j=0; j<M; j++)
		{
			A[lda*j+i]+=alpha*X[incX*i]*Y[incY*j];
		}
	}
}

