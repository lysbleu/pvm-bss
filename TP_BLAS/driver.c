#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "dgemm.h"
#include "ddot.h"
#include "perf.h"

int main(int argc, char* argv[])
{
	blas_t *vecteur;
	alloc_vecteur(&vecteur, 10);
	affiche(1, 10, vecteur, 1, stdout);
	blas_t *matriceA, *matriceB, *matriceC;
	alloc_matrice(&matriceC, 5,5);
	affiche(5,5,matriceC, 5, stdout);
	memset(matriceC, 0, sizeof(blas_t)*5*5);
	
	alloc_matrice(&matriceA, 5,5);
	alloc_matrice(&matriceB, 5,5);
	fprintf(stdout, "A\n\n");
	affiche(5,5,matriceA, 5, stdout);

	fprintf(stdout, "B\n\n");
	affiche(5,5,matriceB, 5, stdout);

	fprintf(stdout, "C\n\n");
	memset(matriceC, 0, sizeof(blas_t)*5*5);
	cblas_dgemm_scalaire1(matriceC,5, matriceA,5,  matriceB, 5, 5);
	affiche(5,5,matriceC, 5, stdout);
	
	memset(matriceC, 0, sizeof(blas_t)*5*5);
	cblas_dgemm_scalaire2(matriceC,5, matriceA,5,  matriceB, 5, 5);
	affiche(5,5,matriceC, 5, stdout);
	
	memset(matriceC, 0, sizeof(blas_t)*5*5);
	cblas_dgemm_scalaire3(matriceC,5, matriceA,5,  matriceB, 5, 5);
	affiche(5,5,matriceC, 5, stdout);

// Tests de performances de ddot	

	blas_t *matriceD, *matriceE;
	alloc_matrice(&matriceD, 100,100);
	alloc_matrice(&matriceE, 100,100);

	printf("Tests de performance de la fonction ddot\n");
	perf_t *t1, *t2;
	t1 = malloc(sizeof(perf_t));
	t2 = malloc(sizeof(perf_t));
	double mflops;
	perf(t1);
	blas_t res = ddot(10000, matriceD, 1, matriceE, 1);
	printf("Résultat du produit scalaire: %le \n", res);
	perf(t2);
	perf_printh(t1);
	perf_printh(t2);
	perf_diff(t1, t2);
	perf_printh(t2);
	
	mflops = perf_mflops(t2, 10000);
	printf("Mflops/s: %le\n", mflops);

	free(matriceA);
	free(matriceB);
	free(matriceC);
	free(vecteur);
	return EXIT_SUCCESS;
}
