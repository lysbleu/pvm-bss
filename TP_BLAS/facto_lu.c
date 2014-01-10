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

void dtrsm(const enum CBLAS_SIDE side, const enum CBLAS_UPLO uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG diag, const int M, const int N, const blas_t alpha, const blas_t *A, const int lda, blas_t *B, const int ldb)
{
	int i,j;
	
	if(side == CblasLeft )
	{
		if(alpha == 0)
		{
			for (j=0; j<N, j++)
			{
				for (i=0; i<M; i++)
				{
					B[i+j*ldb]=0;
				}
			}
		}
		
		if(uplo == CblasUpper)
		{
			for (j=0; j<N; j++)
			{
				if(alpha != 1)
				{
					for (i=0; i<M; i++)
					{
						B[i+j*ldb]*=alpha;
					}
				}
				for (k=M-1; k>=0; k--)
				{
					if(B[k+j*ldb] !=0)
					{
						if(diag == CblasNonUnit)
						{
							B[k+j*ldb] /=A[k+k*lda];
						}
						for(i=0;i<k-1; i++)
						{
							B[i+j*ldb]-= B[k+j*ldb]*A[i+k*lda];
						}
					}
				}
			}
		}
		else if (uplo == CblasLower)
		{
			for (j=0; j<N; j++)
			{
				if(alpha != 1)
				{
					for (i=0; i<M; i++)
					{
						B[i+j*ldb]*=alpha;
					}
				}
				for (k=0; k<M; k++)
				{
					if(B[k+j*ldb] !=0)
					{
						if(diag == CblasNonUnit)
						{
							B[k+j*ldb] /=A[k+k*lda];
						}
						for(i=k;i<M; i++)
						{
							B[i+j*ldb]-= B[k+j*ldb]*A[i+k*lda];
						}
					}
				}
			}
		}
	}
	else
	{
		printf("dtrsm : Only Cblasleft is allowed\n"); 
	}
}  
