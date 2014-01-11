#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"


#ifndef __INTEL_MLK__
#include "dgemm.h"
#include "ddot.h"
#endif

#ifdef __INTEL_MLK__
#include <mkl.h>
#endif

#include "perf.h"

int main(int argc, char* argv[])
{
	
// Tests de performances de ddot	
	int size = 50;

	blas_t *matriceD, *matriceE;
	alloc_vecteur(&matriceD, size);
	alloc_vecteur(&matriceE, size);

	printf("Tests de performance de la fonction ddot\n");
	perf_t *t1, *t2,*t3, *t4,*t5, *t6,*t7, *t8;
	t1 = malloc(sizeof(perf_t));
	t2 = malloc(sizeof(perf_t));
        t3 = malloc(sizeof(perf_t));
	t4 = malloc(sizeof(perf_t));
        t5 = malloc(sizeof(perf_t));
	t6 = malloc(sizeof(perf_t));
        t7 = malloc(sizeof(perf_t));
	t8 = malloc(sizeof(perf_t));
        

	double mflops, mflops1,mflops2,mflops3,mflops4;
	char command[200];
        system("rm results/ddot_perf.txt");
	for(size = 50; size < 100000000; size += size/4)
	{
		printf("M: %d ", size);
		if(size != 50)
		{
			free(matriceD);
			free(matriceE);
			alloc_vecteur(&matriceD, size);
			alloc_vecteur(&matriceE, size);
		}
		perf(t1);
		blas_t res = cblas_ddot(size, matriceD, 1, matriceE, 1);
		perf(t2);
		perf_diff(t1, t2);
		mflops = perf_mflops(t2, 2 * size);
                printf("Mflops/s: %le\n", mflops);
		long time = t2->tv_usec + ( t2->tv_sec * 1000000);

                sprintf(command, "echo %d %lf >> results/ddot_perf.txt", size, mflops);	
                system(command);
				
	}


// Test de performance dgemm
//////////////////////////////////////////

	long m = 100;

	
	blas_t *matriceA, *matriceB, *matriceC;
	
	alloc_matrice(&matriceA, m, m);
	alloc_matrice(&matriceB, m, m);
        matriceC = calloc(m*m,sizeof(blas_t));
        system("rm results/dgemm_perf.txt");

	for(; m< 1000; m+=20)
	{
            printf("M: %d ", m);
        
		if(m != 100)
		{
			free(matriceA);
			free(matriceB);
			free(matriceC);
			alloc_matrice(&matriceA, m, m);
			alloc_matrice(&matriceB, m, m);
			alloc_matrice(&matriceC, m, m);
		}
        
                perf(t1);
                cblas_dgemm_scalaire( CblasNoTrans, CblasNoTrans ,m, m, m, 1, matriceA, m, matriceB, m, 1, matriceC, m);
		perf(t2);
		perf_diff(t1, t2);
                mflops1 = perf_mflops(t2, m * m * m * 3 + m * m );
                printf("Mflops/s: %le\n", mflops);
                
        
		perf(t3);
		cblas_dgemm_scalaire1(matriceC, m, matriceA, m, matriceB, m,  m);
		perf(t4);
		perf_diff(t3, t4);
        
        
                
                mflops2 = perf_mflops(t4, m * m * m * 3);
              	perf(t5);
		cblas_dgemm_scalaire2(matriceC, m, matriceA, m, matriceB, m,  m);
		perf(t6);
		perf_diff(t5, t6);
                mflops3 = perf_mflops(t6, m * m * m * 3);
                perf(t7);
		cblas_dgemm_scalaire3(matriceC, m, matriceA, m, matriceB, m,  m);
		perf(t8);
		perf_diff(t7, t8);
                mflops4 = perf_mflops(t8, m * m * m * 3);
					
                sprintf(command, "echo %d %lf %lf %lf %lf  >> results/dgemm_perf.txt", m * m, mflops1, mflops2, mflops3, mflops4);	
                system(command);
	}
	printf("Mflops/s: %le\n", mflops);

	free(matriceA);
	free(matriceB);
	free(matriceC);
	free(matriceD);
	free(matriceE);
	return EXIT_SUCCESS;
}
