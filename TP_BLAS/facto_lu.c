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
        dscal(&(A[k*lda+k+1]), M-k, 1, 1/A[k*lda+k]);
        cblas_dger(CblasColMajor, M-k, N-k, 1, &(A[k*lda+k+1]), 1, &(A[(k+1)*lda+k]), lda, &(A[k*lda+k]), lda);
    }
}

void dtrsm(const enum CBLAS_SIDE side, const enum CBLAS_UPLO uplo,
           const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG diag,
           const int M, const int N, const blas_t alpha, const blas_t *A,
           const int lda, blas_t *B, const int ldb)
{
    int i,j,k;
	
    if(side == CblasLeft )
    {
        if(alpha == 0)
        {
            for (j=0; j<N; j++)
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

int dgesv_nopiv( const int N, int nrhs, blas_t* A, const int lda, blas_t* B,
           const int ldb) {
    if(N < 0) {
        return -1;
    }
    else if (nrhs < 0) {
        return -2;
    }
    else if(lda < 1 || lda < N) {
        return -4;
    }
    else if(ldb < 1 || ldb < N) {
        return -7;
    }
    
    int ret = 0;
    
    ret = dgetrf(N, N, A, lda);
    
    if (ret != 0) {
        return ret;
    }
    dgetrs_nopiv(CblasNoTrans, N, NRHS, A, lda, B, ldb);
    
}

int dgetrs_nopiv(const int N, const int NRHS, double* A, const int lda, double* B, const int ldb) {
    if (N < 0) {
        return -2;
    }
    else if(NRHS < 0) {
        return -3;
    }
    else if(lda < 1 || lda < N ) {
        return -5;
    }
    else if (ldb < 1 || ldb < N) {
        return -8;
    }

    if (N == 0 || NRHS == 0 ) {
        return 0;
    }


    dtrsm(CblasLeft, CblasLower, CblasNoTrans, CblasUnit, N, NRHS, 1, A, lda, B, ldb);
    dtrsm(CblasLeft, CblasUpper, CblasNoTrans, CblasNonUnit, N, NRHS, 1, A, lda, B, ldb);
    dtrsm(CblasLeft, CblasLower, CblasTrans, CblasNonUnit, N, NRHS, 1, A, lda, B, ldb);
    dtrsm(CblasLeft, CblasLower, CblasTrans, CblasUnit, N, NRHS, 1, A, lda, B, ldb);
    
    return 0;
}
