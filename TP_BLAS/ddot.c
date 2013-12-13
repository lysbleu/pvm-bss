#include "ddot.h"


blas_t ddot(const int N, const blas_t *X, const int incX,
                  const blas_t *Y, const int incY)
{
	
	blas_t res;
	
	for(int i = 0; i<N; i++)
	{
		res += X[i * incX] * Y[i * incY];
	}
	
	return res;
}
