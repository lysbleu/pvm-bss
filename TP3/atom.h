#ifndef ATOM_H
#define ATOM_H
#include <math.h>
#include <stdio.h>

#define G 6.67384E-11  //m3 kg-1 s-2

typedef struct atome{
	double m;
	double pos[2];
	double vit[2];
	double acc[2];
}Atome;

static inline double distance(Atome A, Atome B)
{
	return sqrt(pow(A.pos[0] - B.pos[0], 2) + pow(A.pos[1] - B.pos[1], 2));
}

static inline double force_inter(Atome A, Atome B)  // calcule la force d'interaction entre 2 masses
{
	double F;
	F = A.m * B.m;
	F /= (pow(A.pos[0] - B.pos[0], 2) + pow(A.pos[1] - B.pos[1], 2));
	F *= G;
	printf("A.m :%le, A.pos[0]:%le,  A.pos[1]:%le, B.m :%le, B.pos[0]:%le,  B.pos[1]:%le\n", A.m, A.pos[0],A.pos[1], B.m,  B.pos[0], B.pos[1]); 
	//~ printf("F:%lf\n", F);
	return -F;
}

static inline void vitesse(Atome * A, double dt) // calcule la vitesse de A
{
	A->vit[0] += A->acc[0] * dt;
	A->vit[1] += A->acc[1] * dt;
}

static inline void acceleration(Atome * A, double F[2])  //calcule l'acceleration de A
{
	printf("Acceleration m:%le  F[0]:%le F[1]:%le\n",A->m, F[0], F[1]);
	A->acc[0] += F[0] / A->m;
	A->acc[1] += F[1] / A->m;
}

static inline void new_pos(Atome * A, double dt) // calcule la nouvelle position de A
{
	A->pos[0] += A->vit[0]*dt + A->acc[0]*dt/2.;
	A->pos[1] += A->vit[1]*dt + A->acc[1]*dt/2.;
}

static inline double norme(double vect[2])
{
	return sqrt(pow(vect[0], 2) + pow(vect[1], 2));
}

static inline double norme_carre(double vect[2])
{
	return pow(vect[0], 2) + pow(vect[1], 2);
}

static inline double calc_dt(Atome A, double dist)
{
	//~ printf("norme_carre(A.vit): %le\nnorme(A.acc): %le\ndist: %le\nnorme(A.vit):%le\nnorme(A.acc): %le\nresult:%lf\n\n", norme_carre(A.vit), norme(A.acc), dist, norme(A.vit), norme(A.acc), (sqrt(norme_carre(A.vit) + 0.2 * norme(A.acc) * dist) - norme(A.vit))/norme(A.acc));
	printf("A.acc: (%le, %le)\nA.vit:(%le, %le)\ndist: %le\n\n", A.acc[0], A.acc[1], A.vit[0], A.vit[1], dist );
	return ((sqrt(norme_carre(A.vit) + 0.2 * norme(A.acc) * dist) - norme(A.vit))/norme(A.acc));
	
}

#endif //ATOM_H
