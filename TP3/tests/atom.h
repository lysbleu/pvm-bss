#ifndef ATOM_H
#define ATOM_H

#define G 6.67384E-11  //m3 kg-1 s-2

typedef struct atome{
	double m;
	double pos[2];
	double vit[2];
	double acc[2];
}Atome;

inline double distance(Atome A, Atome B);

inline double force_inter(Atome A, Atome B);  // calcule la force d'interraction entre 2 masses

inline void vitesse(Atome * A, double dt); // calcule la vitesse de A

inline void acceleration(Atome * A, double F[2]);  //calcule l'acceleration de A

inline void new_pos(Atome * A, double dt); // calcule la nouvelle position de A

inline double norme(double vect[2]);

inline double norme_carre(double vect[2]);

inline double calc_dt(Atome A, double dist);


#endif //ATOM_H
