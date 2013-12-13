#ifndef UTIL_H_
#define UTIL_H_

typedef blas_t double;

void affiche(int m, int n, blas_t **a, int lda, FILE* flux);
void alloc_vecteur(blas_t **vecteur, int taille);
void alloc_matrice(blas_t **matrice , int m, int n);

#endif /*UTIL_H_*/
