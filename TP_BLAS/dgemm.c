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
				C[j*ldc+i] += A[k+i*lda]*B[k+j*ldb];
			}
		}
	}
}
