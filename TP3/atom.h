#indef ATOM_H
#define ATOM_H

#define G 6.67384E-11  //m3 kg-1 s-2

typdef struct atome
{
	double m;
	double pos[2];
	double vit[2];
	double acc[2];
}Atome;


double force_inter(Atome A, Atome B);  // calcule la force d'interraction entre 2 masses

void vitesse(Atome A, double dt); // calcule la vitesse de A

void acceleration(Atome A, double F[2]);

void new_pos(Atome A, double dt); // calcule la nouvelle position de A


#endif //ATOM_H
