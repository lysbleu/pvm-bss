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
	char* mdp = (char*) calloc(longueur_mdp+1, sizeof(char));
	strcpy(mdp, argv[3]);
	
	char *argv_esclave[2] ;
	argv_esclave[0] = (char*) calloc(strlen(argv[2])+1, sizeof(char));
	strcpy(argv_esclave[0],argv[2]);
	 
	argv_esclave[1] = (char*) calloc(strlen(argv[3])+1, sizeof(char));
	strcpy(argv_esclave[1],argv[3]);
	 
	int i;
	int trouve = 0;
	int fini = 0;
	int debut_sequence, fin_sequence, pas, fin_exec;
	int bufid, info, bytes, type, source;*
	char * solution;
	
	//initialisation des esclaves
	pvm_spawn("craquage_esclave", argv_esclave, PvmTaskDefault,"", nb_esclaves, tids);

	/*
	for(i=0; i<nb_esclaves;i++)
	{
		pvm_initsend(PvmDataDefault);
		//pvm_pkint(&i, 1, 1);
		pvm_send(tids[i],0);
	}*/
	
	//calcul du pas, fin_exec
	debut_sequence = 0;
	//pas ... TODO
	fin_sequence = pas;
	fin_exec = ((pow(longueur_mdp, 15)-1)*15)/14;
	//boucle principale
	while(!trouve && !fini)
	{
		bufid = pvm_recv( -1, -1 );
		info = pvm_bufinfo( bufid, &bytes, &type, &source );
		
		if (info < 0)
		{
			printf("Erreur de reception : %d\n", info);
			exit(1);
		}
		
		switch(type)
		{
			case(0)://mot de passe trouve
			solution = calloc(bytes, sizeof(char));
			pvm_upkstr(solution, bytes + 1, 1);
			printf("La solution est : %s\n", solution);
			trouve = 1;
			break;	
			
			case(1)://esclave veut plus de donnees
			if(debut_sequence < fin_exec){
					
				//TODO calcul du début et fin sequence
				//debut_sequence = ...
				//fin_sequence = ...
				
				pvm_initsend(PvmDataDefault);
				pvm_pkint(&debut_sequence, 1, 1);
				pvm_pkint(&fin_sequence, 1, 1);
				pvm_send(source,0);
				
				debut_sequence = fin_sequence + 1;
				}
			else{
				fini = 1;
				printf("Pas de solution\n");
			}		
			
			break;
			default:
			break;
		}
	}
	
	for(i=0; i<nb_esclaves;i++)
	{
		info = pvm_kill(tids[i]);
		printf("Erreur de fermeture : %d\n", info);
	}
	
	pvm_exit();
	
	free(tids);
	free(mdp);
	free(argv_esclave[0]);
	free(argv_esclave[1]);

	return EXIT_SUCCESS;
}
