#include "atom.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define NB_CHAR_LINE 50
int parse(char* filename, Atome **initialDatas, int rank, int nbProcs) {
    FILE *file = fopen(filename, "r");
    fseek(file, 0, SEEK_END);

    long nbLines = (long) floor( (ftell(file)) / NB_CHAR_LINE);
    int nbLinesByProc = nbLines / nbProcs;
    int remain  = nbLines % nbProcs;

    long cursor = 0;
    if(rank < remain) {
        nbLinesByProc++;
        cursor = rank * nbLinesByProc * NB_CHAR_LINE;
    }
    else {
        cursor = (rank * nbLinesByProc + remain) * NB_CHAR_LINE;
    }
	char buf[51];
	char *buf2;
    fseek(file, cursor, SEEK_SET);
	*initialDatas = calloc(nbLinesByProc, sizeof(Atome));

    for (int i = 0; i < nbLinesByProc; i++) {
        fscanf(file, "%lf %lf %lf %lf %lf\n", &((*initialDatas)[i].m), 
			&((*initialDatas)[i].pos[0]), &((*initialDatas)[i].pos[1]),
			&((*initialDatas)[i].vit[0]), &((*initialDatas)[i].vit[1]));
    }
    fclose(file);
    return nbLinesByProc;
}

void displayAtoms(Atome *atoms, int size) {
    for(int i = 0 ; i < size ; i++) {
        printf("Masse : %le, position : (%le, %le), vitesse : (%le, %le)\n",
               atoms[i].m, atoms[i].pos[0], atoms[i].pos[1],
               atoms[i].vit[0], atoms[i].vit[1]);
    }

}
