#include "atom.h"
#include <stdio.h>


int main(void)
{
	Atome A;
	Atome B;
	Atome C;
	
	A.m = 10;
	B.m = 10;
	C.m = 20;
	
	A.pos[0] = 0;
	A.pos[1] = 0;
	
	B.pos[0] = 0;
	B.pos[1] = 10;
	
	C.pos[0] = 10;
	C.pos[1] = 10;
	
	printf("A: m = %lf, pos=(%lf, %lf), vit=(%lf, %lf), acc=(%lf, %lf)\n", A.m, A.pos[0], A.pos[1], A.vit[0], A.vit[1], A.acc[0], A.acc[1]);
	
	printf("B: m = %lf, pos=(%lf, %lf), vit=(%lf, %lf), acc=(%lf, %lf)\n", B.m, B.pos[0], B.pos[1], B.vit[0], B.vit[1], B.acc[0], B.acc[1]);
	
	printf("C: m = %lf, pos=(%lf, %lf), vit=(%lf, %lf), acc=(%lf, %lf)\n", C.m, C.pos[0], C.pos[1], C.vit[0], C.vit[1], C.acc[0], C.acc[1]);
	
	double F1 = force_inter(A, B);
	double F2 = force_inter(A, C);
	double F3 = force_inter(B, C);
	
	printf("Force d'interaction entre A et B: %lf\n", F1);
	printf("Force d'interaction entre A et C: %lf\n", F2);
	printf("Force d'interaction entre B et C: %lf\n", F3);
	
	double FA = F1 + F2;
	double FB = F2 + F3;
	double FC = F1 + F3;
	
	
	printf("Force totale sur A: %lf\n", FA);
	printf("Force totale sur B: %lf\n", FB);
	printf("Force totale sur C: %lf\n", FC);
	
}
	
