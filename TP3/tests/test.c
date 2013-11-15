#include "atom.h"
#include <stdio.h>
#include <math.h>

#define MIN(a, b)  (a < b ? a : b)

void print_atom(Atome A)
{
	printf("A: m = %lf, pos=(%lf, %lf), vit=(%lf, %lf), acc=(%lf, %lf)\n", A.m, A.pos[0], A.pos[1], A.vit[0], A.vit[1], A.acc[0], A.acc[1]);
	double n = norme(A.pos);
	double n2 = norme_carre(A.pos);
	printf("Norme de A: %lf, norme carree: %lf\n", n, n2);
}

int main(void)
{
	Atome A;
	Atome B;
	Atome C;
	
	A.m = 10000000;
	B.m = 10000000;
	C.m = 20000000;
	
	A.pos[0] = 0;
	A.pos[1] = 0;
	
	B.pos[0] = 0;
	B.pos[1] = 10;
	
	C.pos[0] = 10;
	C.pos[1] = 10;
	
	print_atom(A);
	print_atom(B);
	print_atom(C);

	double F1 = force_inter(A, B);
	double F2 = force_inter(A, C);
	double F3 = force_inter(B, C);
	
	printf("Force d'interaction entre A et B: %lf\n", F1);
	printf("Force d'interaction entre A et C: %lf\n", F2);
	printf("Force d'interaction entre B et C: %lf\n", F3);
	
	double DAB = distance(A, B);
	double DAC = distance(A, C);
	double DBC = distance(B, C);
	
	printf("Distance AB: %lf,\n", DAB);
	printf("Distance AC: %lf,\n", DAC);
	printf("Distance BC: %lf,\n", DBC);
	
	double FA[2];
	double FB[2];
	double FC[2];
	
	FA[0] = F1 * cos(distance(A, B)) + F2 * cos(distance(A, C));
	FA[1] = F1 * sin(distance(A, B)) + F2 * sin(distance(A, C));
	
	FB[0] = -F1 * cos(distance(A, B)) + F3 * cos(distance(B, C));
	FB[1] = -F1 * sin(distance(A, B)) + F3 * sin(distance(B, C));
	
	FC[0] = -F1 * cos(distance(A, C)) + -F2 * cos(distance(B, C));
	FC[1] = -F1 * sin(distance(A, C)) + -F2 * sin(distance(B, C));
	
	printf("Force totale sur A: (%lf, %lf)\n", FA[0], FA[1]);
	printf("Force totale sur B: (%lf, %lf)\n", FB[0], FB[1]);
	printf("Force totale sur C: (%lf, %lf)\n", FC[0], FC[1]);
	
	acceleration(&A, FA);
	acceleration(&B, FB);
	acceleration(&C, FC);
	
	double dtA = calc_dt(A, 10.);
	double dtB = calc_dt(B, 10.);
	double dtC = calc_dt(C, 10.);
	
	printf("dtA = %lf\n", dtA);
	printf("dtB = %lf\n", dtB);
	printf("dtC = %lf\n", dtC);
	
	double dt_min = MIN(dtA, dtB);
	dt_min = MIN(dt_min, dtC);
	
	printf("dt_min: %lf\n", dt_min);
	
	vitesse(&A, dt_min);
	vitesse(&B, dt_min);
	vitesse(&C, dt_min);
	
	new_pos(&A, dt_min);
	new_pos(&B, dt_min);
	new_pos(&C, dt_min);
	
	print_atom(A);
	print_atom(B);
	print_atom(C);
	
	
	return 1;
	
}
	
