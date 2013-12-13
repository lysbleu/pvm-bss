#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "dgemm.h"
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

	free(matriceA);
	free(matriceB);
	free(matriceC);
	free(vecteur);
	return EXIT_SUCCESS;
}
