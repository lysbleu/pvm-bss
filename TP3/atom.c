#include "atom.h"
#include <math.h>

double force_inter(Atome A, Atome B);
{
	double F;
	F = A.m * B.m;
	F /= (pow(A.pos[0] - B.pos[0], 2) + pow(A.pos[1] - b.pos[1], 2));
	F *= G;
	return F;
}

void vitesse(Atome * A, double dt)
{
	A->vit[0] += A->acc[0] * dt;
	A->vit[1] += A->acc[1] * dt;
	
}

void acceleration(Atome A, double F[2])
{
	A.acc[0] = F[0] / A.m;
	A.acc[1] = F[1] / A.m;
}

double * new_pos(Atome A, double dt)
{
	A.pos[0] += A.vit[0]*dt + A.acc[0]*dt/2;
	A.pos[1] += A.vit[1]*dt + A.acc[1]*dt/2;
}
