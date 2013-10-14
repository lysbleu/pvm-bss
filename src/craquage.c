#include "pvm3.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define TAILLE_MAX  1000

int main (int argc, char* argv[])
{
	if(argc !=4)
	{
		printf("usage : ./craquage p r m\n");
		return EXIT_FAILURE;
	}
	// Initialisation des variables
	int nb_esclaves = atoi(argv[1]);
	int* tids = (int*) calloc(p, sizeof(int));
	int longueur_mdp = atoi(argv[2]);
	char* mdp = (char*) calloc(longueur_mdp+1);
	strncpy(mdp, argv[3],longueur_mdp+1);
	
	//Partie maitre
	int i;

	pvm_spawn("craquage_esclave", argv, PvmTaskDefault,"", nb_esclaves, tids);

	for(i=0; i<nb_esclaves;i++)
	{
		
	}
		
	pvm_exit();
	return EXIT_SUCCESS;
}
