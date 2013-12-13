#include "ddot.h"


blas_t ddot(blas_t * x, blas_t * y, int dim)
{
	
	blas_t res;
	
	for(int i = 0; i<dim; i++)
	{
		res[i] += x[i] * y[i];
	}
}
