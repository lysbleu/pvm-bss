#include "dgemm.h"
#include <pthread.h>
#include <string.h>

int max_threads  = 1 ;
int num_threads = 1;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
void cblas_dgemm_scalaire1(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B,
                           int ldb, int m)
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

void cblas_dgemm_scalaire2(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B,
                           int ldb, int m)
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

void cblas_dgemm_scalaire3(blas_t *C,int ldc, blas_t *A,int lda,  blas_t *B,
                           int ldb, int m)
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

void *execute(void *arg_)
{
	struct arg *argument = arg_;
	if(num_threads == max_threads)
	{
		cblas_dgemm_scalaire(argument->TransA, argument->TransB, argument->M, argument->N, argument->K, argument->alpha, &((argument->A)[argument->A_i1]), argument->lda, &((argument->B)[argument->B_i1]), argument->ldb, argument->beta, &((argument->C)[argument->C_i]), argument->ldc);
		cblas_dgemm_scalaire(argument->TransA, argument->TransB, argument->M, argument->N, argument->K, argument->alpha, &((argument->A)[argument->A_i2]), argument->lda, &((argument->B)[argument->B_i2]), argument->ldb, 1, &((argument->C)[argument->C_i]), argument->ldc);
		//~ cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[A2]), lda_bis, &(Bbis[B3]), ldb_bis, 1, C, ldc);
	}
	else
	{
		pthread_mutex_lock(&lock);
		num_threads +=4;
		phtread_mutex_unlock(&lock);
	}
	
	return NULL;
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
	
	char var_env[100];
	pthread_t threads[4];

	if(getenv("MYLIB_NUM_THREADS") != NULL)
	{
		strcpy(var_env,getenv("MYLIB_NUM_THREADS"));

		max_threads = atoi(var_env);
	} 
	
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
				
			if(num_threads == 1 || num_threads == max_threads)
			{
				cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, Abis, lda_bis, Bbis, ldb_bis, beta, C, ldc);
				cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[A2]), lda_bis, &(Bbis[B3]), ldb_bis, 1, C, ldc);
				
				cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, Abis, lda_bis, &(Bbis[B2]), ldb_bis, beta, &(C[(N/2)*ldc]), ldc);
				cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[B2]), lda_bis, &(Bbis[B4]), ldb_bis, 1,  &(C[(N/2)*ldc]), ldc);
				
				cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[A3]), lda_bis, Bbis, ldb_bis, beta, &(C[M/2]), ldc);
				cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[A4]), lda_bis, &(Bbis[B3]), ldb_bis, 1,  &(C[M/2]), ldc);
				
				cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[A3]), lda_bis, &(Bbis[B2]), ldb_bis, beta, &(C[M/2+(N/2)*ldc]), ldc);
				cblas_dgemm_scalaire(TransA, TransB, M/2, N/2, K/2, alpha, &(Abis[A4]), lda_bis, &(Bbis[B4]), ldb_bis, 1,  &(C[M/2+(N/2)*ldc]), ldc);
			}
			else
			{
				pthread_mutex_lock(&lock);
				num_threads +=4;
				pthread_mutex_unlock(&lock);
				
				struct arg arg_thread;
				arg_thread.TransA = TransA;
				arg_thread.TransB = TransB;
				arg_thread.M = M/2;
				arg_thread.N = N/2;
				arg_thread.K = K/2;
				arg_thread.alpha = alpha;
				arg_thread.A = Abis;
				arg_thread.lda = lda_bis;
				arg_thread.B = Bbis;
				arg_thread.ldb = ldb_bis;
				arg_thread.beta = beta;
				arg_thread.C = C;

				struct arg *arg_thread_tmp1 = calloc(1, sizeof(struct arg));
				memcpy(arg_thread_tmp1, &arg_thread, sizeof(struct arg));
				arg_thread_tmp1->A_i1 = 0;
				arg_thread_tmp1->A_i2 = A2;
				arg_thread_tmp1->B_i1 = 0;
				arg_thread_tmp1->B_i2 = B3;
				arg_thread_tmp1->C_i = 0;

				pthread_create(&(threads[0]), NULL, execute, arg_thread_tmp1);
				
				struct arg *arg_thread_tmp2 = calloc(1, sizeof(struct arg));
				memcpy(arg_thread_tmp2, &arg_thread, sizeof(struct arg));
				arg_thread_tmp2->A_i1 = 0;
				arg_thread_tmp2->A_i2 = A2;
				arg_thread_tmp2->B_i1 = B2;
				arg_thread_tmp2->B_i2 = B4;
				arg_thread_tmp2->C_i = (N/2)*ldc;

				pthread_create(&(threads[1]), NULL, execute, arg_thread_tmp2);
				
				struct arg *arg_thread_tmp3 = calloc(1, sizeof(struct arg));
				memcpy(arg_thread_tmp3, &arg_thread, sizeof(struct arg));
				arg_thread_tmp3->A_i1 = A3;
				arg_thread_tmp3->A_i2 = A4;
				arg_thread_tmp3->B_i1 = 0;
				arg_thread_tmp3->B_i2 = B3;
				arg_thread_tmp3->C_i = M/2;

				pthread_create(&(threads[2]), NULL, execute, arg_thread_tmp3);
				
				struct arg *arg_thread_tmp4 = calloc(1, sizeof(struct arg));
				memcpy(arg_thread_tmp4, &arg_thread, sizeof(struct arg));
				arg_thread_tmp4->A_i1 = A3;
				arg_thread_tmp4->A_i2 = A4;
				arg_thread_tmp4->B_i1 = B2;
				arg_thread_tmp4->B_i2 = B4;
				arg_thread_tmp4->C_i = M/2+(N/2)*ldc;

				pthread_create(&(threads[3]), NULL, execute, arg_thread_tmp4);
				
				pthread_join(threads[0], NULL);
				pthread_join(threads[1], NULL);
				pthread_join(threads[2], NULL);
				pthread_join(threads[3], NULL);
			}
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
