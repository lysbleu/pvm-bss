#ifndef _UTIL_H_
#define _UTIL_H_

#ifndef TAILLE
#define TAILLE 8
#endif /*TAILLE*/

#ifndef MIN
#define MIN(x,y) ((x<y)?x:y)
#endif /*MIN*/

int nb_carreaux(const int i, const int j, const int taille);
int choix_N(const int nb_procs, const int nb_tuiles);
int stocke_carreaux(int ** tab_carreaux, const int i, const int j, const int taille, int *myrank);
void reduce_in_place(float *TabColor, int count, int root);
void liberation(int *tab_carreaux);
#endif /*_UTIL_H_*/
