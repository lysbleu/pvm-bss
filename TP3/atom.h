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
	//~ printf("A.m :%lf, A.pos[0]:%lf,  A.pos[1]:%lf, B.m :%lf, B.pos[0]:%lf,  B.pos[1]:%lf\n", A.m, A.pos[0],A.pos[1], B.m,  B.pos[0], B.pos[1]); 
	//~ printf("F:%lf\n", F);
	return F;
}

static inline void vitesse(Atome * A, double dt) // calcule la vitesse de A
{
	A->vit[0] += A->acc[0] * dt;
	A->vit[1] += A->acc[1] * dt;
}

static inline void acceleration(Atome * A, double F[2])  //calcule l'acceleration de A
{
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
	printf("acc0: %lf, acc1: %lf, vot0: %lf, vit1: %lf pos0: %lf, pos1: %lf, masse: %lf\n", A.acc[0],A.acc[1], A.vit[0],A.vit[1],A.pos[0],A.pos[1], A.m);
	printf("norme_carre(A.vit): %le\nnorme(A.acc): %le\ndist: %le\nnorme(A.vit):%le\nnorme(A.acc): %le\n\n", norme_carre(A.vit), norme(A.acc), dist, norme(A.vit), norme(A.acc));
	return ((sqrt(norme_carre(A.vit) + 0.2 * norme(A.acc) * dist) - norme(A.vit))/norme(A.acc));
	
}

#endif //ATOM_H
