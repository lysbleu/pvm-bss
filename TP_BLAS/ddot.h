#ifndef DDOT_H
#define DDOT_H

#include "util.h"

blas_t cblas_ddot(const int N, const double *X, const int incX,
                  const double *Y, const int incY);
#endif //DDOT_H
