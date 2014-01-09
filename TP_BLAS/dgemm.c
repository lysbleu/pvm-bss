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

void cblas_dgemm_scalaire(const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const blas_t alpha, const blas_t *A,
                 const int lda, const blas_t *B, const int ldb,
                 const blas_t beta, blas_t *C, const int ldc)
{
	int i,j,k;
	if(TransA == CblasNoTrans)
	{
		
	}
	
	if(TransB == CblasTrans)
	{
		
	}
	
	for (j=0; j<N; j++)
	{
		for (i=0; i<M;i++)
		{
			C[j*ldc+i]*=beta;
			for(k=0; k<K; k++)
			{	
				if((TransA != CblasNoTrans) && (TransB != CblasTrans))
				{
					C[j*ldc+i] += alpha*A[k*lda+i]*B[k+j*ldb];
				}
				else if(TransA != CblasNoTrans)
				{
					C[j*ldc+i] += alpha*A[k+i*lda]*B[k+j*ldb];
				}
				else
				{
					C[j*ldc+i] += alpha*A[k*lda+i]*B[k*ldb+j];
				}
			}
		}
	}
}

void cblas_dgemv(const enum CBLAS_ORDER order,
                 const enum CBLAS_TRANSPOSE TransA, const int M, const int N,
						const blas_t alpha, const blas_t *A, const int lda,
						const blas_t *X, const int incX, const blas_t beta,
						blas_t *Y, const int incY)
{
	int i,j,k;
	if(order == CblasColMajor)
	{
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
	else
	{
		printf("cblas_dgemv : Only CblasColMajor is allowed\n"); 
	}
}

void cblas_daxpy(const int N, const blas_t alpha,const blas_t *X, const int ldx,
                 /*@out@*/ blas_t *Y, const int ldy)
{
	int i;
	for (i=0; i<N; i++)
	{
		Y[i*ldy] += alpha*X[i*ldx];
	}
}

void cblas_dger(const enum CBLAS_ORDER order, const int M, const int N,
                const blas_t alpha, const blas_t *X, const int incX,
                const blas_t *Y, const int incY, blas_t *A, const int lda)
{
	int i,j;
	if(order == CblasColMajor)
	{
		for(i=0; i< N; i++)
		{
			for(j=0; j<M; j++)
			{
				A[lda*j+i]+=alpha*X[incX*i]*Y[incY*j];
			}
		}
	}
	else
	{
		printf("cblas_dger : Only CblasColMajor is allowed\n"); 
	}
}

void cblas_dgemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const blas_t alpha, const blas_t *A,
                 const int lda, const blas_t *B, const int ldb,
                 const blas_t beta, blas_t *C, const int ldc)
{
	blas_t *Abis=A;
	blas_t *Bbis=B;
	int i,j,A2,A3,A4,B2,B3,B4,lda_bis, ldb_bis;
	
	if(Order == CblasColMajor)
	{
		if((TransA == CblasConjTrans) || (TransB == CblasConjTrans))
		{
			printf("cblas_dgemm : Only CblasNoTrans and CblasTrans are allowed\n"); 
		}
		else
		{
			if(TransA == CblasNoTrans)
			{ 
				Abis = calloc(M*K, sizeof(blas_t));
				for (i=0; i<K; i++)
				{
					for(j=0; j<M; j++)
					{
						Abis[i+j*K]=A[i*lda+j];
					}
				}
				lda_bis = K;
				A2=(K/2);
				A3=(M/2)*K;
				A4=(K/2)+(M/2)*K;
			}
			else
			{	
				lda_bis = lda;
				A2=(K/2)*lda;
				A3=(M/2);
				A4=(K/2)*lda+(M/2);
			}
			
			if(TransB == CblasTrans)
			{
				Bbis = calloc(M*K, sizeof(blas_t));
				for (i=0; i<N; i++)
				{
					for(j=0; j<K; j++)
					{
						Bbis[i+j*K]=B[i*ldb+j];
					}
				}
				ldb_bis = N;
				B2=(N/2);
				B3=(K/2)*N;
				B4=(K/2)*N+(N/2);
				
			}
			else
			{
				ldb_bis = ldb;
				B2=(N/2)*ldb;
				B3=(K/2);
				B4=(K/2)+(N/2)*ldb;
			}
			
			cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, Abis, lda_bis, Bbis, ldb_bis, beta, C, ldc);
			cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[A2]), lda_bis, &(Bbis[B3]), ldb_bis, 1, C, ldc);
			
			cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, Abis, lda_bis, &(Bbis[B2]), ldb_bis, beta, &(C[(N/2)*ldc]), ldc);
			cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[B2]), lda_bis, &(Bbis[B4]), ldb_bis, 1,  &(C[(N/2)*ldc]), ldc);
			
			cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[A3]), lda_bis, Bbis, ldb_bis, beta, &(C[M/2]), ldc);
			cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[A4]), lda_bis, &(Bbis[B3]), ldb_bis, 1,  &(C[M/2]), ldc);
			
			cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[A3]), lda_bis, &(Bbis[B2]), ldb_bis, beta, &(C[M/2+(N/2)*ldc]), ldc);
			cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[A4]), lda_bis, &(Bbis[B4]), ldb_bis, 1,  &(C[M/2+(N/2)*ldc]), ldc);
		}
		if(TransA == CblasNoTrans)
		{
			free(Abis);
		}
		if(TransA == CblasTrans)
		{
			free(Bbis);
		}
	}
	else
	{
		printf("cblas_dgemm : Only CblasColMajor is allowed\n"); 
	}
}
