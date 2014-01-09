#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "dgemm.h"
#include "ddot.h"
#include "perf.h"
#include "cblas.h"

int main(int argc, char* argv[])
{
	
// Tests de performances de ddot	
	int size = 5;
	int size2 = size*size;

	blas_t *matriceD, *matriceE;
	alloc_matrice(&matriceD, size, size);
	alloc_matrice(&matriceE, size, size);

	printf("Tests de performance de la fonction ddot\n");
	perf_t *t1, *t2;
	t1 = malloc(sizeof(perf_t));
	t2 = malloc(sizeof(perf_t));
	double mflops;
	char command[200];

	for(size = 5; size2 < 10000000; size += size *25/100)
	{
		printf("size2: %d\n", size2);
		if(size != 5)
		{
			free(matriceD);
			free(matriceE);
			alloc_matrice(&matriceD, size, size);
			alloc_matrice(&matriceE, size, size);
		}
		size2 = size * size;
		perf(t1);
		blas_t res = ddot(size2, matriceD, 1, matriceE, 1);
		perf(t2);
		perf_diff(t1, t2);
		mflops = perf_mflops(t2, size2);
		long time = t2->tv_usec + ( t2->tv_sec * 1000000);
		if(size2==25) //ecrasement si fichier existe deja
		{
			sprintf(command, "echo %ld %d > results/ddot_perf.txt", time, size2);	
			system(command);
		}
		else //concatenation des donnees
		{
			sprintf(command, "echo %ld %d >> results/ddot_perf.txt", time, size2);	
			system(command);	
		}
		
	}
	printf("Mflops/s: %le\n", mflops);


//////////////////////////////////////////

	int m = 50;
	int n = 50;
	int k = 50;
	
	blas_t *matriceA, *matriceB, *matriceC;
	
	for(m = 50; m*n < 10000000; m+= (m/4) + (m/4)%2)
	{
		n = m;
		k = m;
		
		if(m != 50)
		{
			free(matriceA);
			free(matriceB);
			free(matriceC);
			alloc_matrice(&matriceA, m, k);
			alloc_matrice(&matriceB, k, n);
			alloc_matrice(&matriceC, m, n);
		}
		
		perf(t1);
		cblas_dgemm(m, n, k, 1, matriceA, m, matriceB, n, 1, matriceC, m);
		perf(t2);
		perf_diff(t1, t2);
		
		mflops = perf_mflops(t2, size2);
		long time = t2->tv_usec + ( t2->tv_sec * 1000000);
		
		if(size2==25) //ecrasement si fichier existe deja
		{
			sprintf(command, "echo %ld %d > results/dgemm_perf.txt", time, size2);	
			system(command);
		}
		else //concatenation des donnees
		{
			sprintf(command, "echo %ld %d >> results/dgemm_perf.txt", time, size2);	
			system(command);	
		}
		
	}
	printf("Mflops/s: %le\n", mflops);
		
	}	
	
	



	free(matriceA);
	free(matriceB);
	free(matriceC);
	free(matriceD);
	free(matriceE);
	free(vecteur);
	return EXIT_SUCCESS;
}
