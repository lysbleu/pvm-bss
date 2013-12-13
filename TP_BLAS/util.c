#include "util.h"

void affiche(int m, int n, blas_t *a, int lda, FILE* flux)
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
	fprintf(flux,"\n");
}

void alloc_vecteur(blas_t **vecteur, int taille)
{
	int i;
	srand(time(NULL));
	*vecteur = calloc(taille, sizeof(blas_t));
	for (i=0; i<taille; i++)
	{
		(*vecteur)[i]= rand()%100;
	}
}
void alloc_matrice(blas_t **matrice , int m, int n)
{
	int i;
	srand(time(NULL));
	*matrice = calloc(m*n, sizeof(blas_t));
	for (i=0; i<m*n; i++)
	{
		(*matrice)[i]= rand()%100;
	}
}

void destruction(blas_t *matrice)
{
	free(matrice);
}
