#include "util.h"

void affiche(int m, int n, blas_t **a, int lda, FILE* flux)
{
	int i, j;
	for(i=0; i< m; i++)
	{
		for(j=0; j<n; j++)
		{
			fprintf(flux, "%5le ", a[j*lda+i]);
		}
		fprintf(flux,"\n");
	}
}

void alloc_vecteur(int **vecteur, int taille)
{
	int i;
	srand(time(NULL));
	*vecteur = calloc(taille, sizeof(blas_t));
	for (i=0; i<taille; i++)
	{
		
	}
}
void alloc_matrice(int **matrice , int m, int n)
{
	*matrice = calloc(m*n, sizeof(blas_t);
}
