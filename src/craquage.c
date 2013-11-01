#include "pvm3.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <limits.h>

#define TAILLE_MAX  1000
#define NB_TRANCHES 10
#define MIN(n1,n2) n1 < n2 ? n1 : n2

int main (int argc, char* argv[])
{
	if(argc !=4)
	{
		printf("usage : ./craquage p r m\n");
		return EXIT_FAILURE;
	}

	// Initialisation des variables
	int nb_esclaves = atoi(argv[1]);
	int* tids = (int*) calloc(nb_esclaves, sizeof(int));
	int longueur_mdp = atoi(argv[2]);
	char* mdp = (char*) calloc(strlen(argv[3])+1, sizeof(char));
	strcpy(mdp, argv[3]);
 
	//recuperation du chemin de l executable
	char* chemin = getcwd(NULL, 1000);
	strcat(chemin, "/craquage_esclave");
	//printf("%s\n", chemin);
	//creation des arguments pour l esclave
	char *argv_esclave[2] ;
	argv_esclave[0] = (char*) calloc(strlen(argv[2])+1, sizeof(char));
	strcpy(argv_esclave[0],argv[2]);
	 
	argv_esclave[1] = (char*) calloc(strlen(argv[3])+1, sizeof(char));
	strcpy(argv_esclave[1],argv[3]);
	 
	int i;
	int trouve = 0;
	int fini = 0;
	unsigned long int debut_sequence, pas_reel, pas, fin_exec;
	int bufid, info, bytes, type, source;
	char * solution;

	//~ pvm_catchout(stderr);
	
	int longueur_max = 0;
	unsigned long int ulmax = ULONG_MAX;
      	ulmax /=15;
        ulmax *=14;
        ulmax ++;
        while(ulmax>15)
          {
            ulmax/=15;
            longueur_max++;
          }

	if(atoi(argv[2])>longueur_max)
	  {
	    printf("Longueur max du mot de passe trop grande : %d au lieu de %d\n", atoi(argv[2]), longueur_max);
	    return EXIT_FAILURE;
	  }
	
	//initialisation des esclaves
	//printf("chemin:%s argv_esclave[0]:%s argv_esclave[1]:%s nb_esclaves:%d \n", chemin,argv_esclave[0], argv_esclave[1], nb_esclaves); 
	pvm_spawn(chemin, argv_esclave, PvmTaskDefault,"", nb_esclaves, tids);

	//calcul du pas, fin_exec (= fin execution)
	debut_sequence = 0;
	fin_exec = ((pow(15, longueur_mdp)-1)*15)/14;
	pas = fin_exec/(nb_esclaves*longueur_mdp);

	printf("fin_exec: %lu, pas:%lu\n", fin_exec, pas); 
	//boucle principale
	while(!trouve && fini!=nb_esclaves)
	{
		// attente de reception de donnees d un esclave
		bufid = pvm_recv( -1, -1 );
		info = pvm_bufinfo( bufid, &bytes, &type, &source );
		
		if (info < 0)
		{
			printf("Erreur de reception : %d\n", info);
			exit(1);
		}
		
		//selon le tag du message, demande de donnees ou solution trouvee
		switch(type)
		{
			case(0)://mot de passe trouve
			solution = calloc(bytes, sizeof(char));
			pvm_upkstr(solution);
			printf("\nLa solution est : %s\n\n", solution);
			trouve = 1;
			break;	
			
			case(1)://esclave veut plus de donnees
			if(debut_sequence < fin_exec){
				pas_reel = MIN(pas, fin_exec - debut_sequence);
				//prendre en compte la fin des donnees dans le calcul du pas
				//envoi des donnes a l esclave
				pvm_initsend(PvmDataDefault);
				pvm_pkulong(&debut_sequence, 1, 1);
				pvm_pkulong(&pas_reel, 1, 1);
				pvm_send(source,0);
				
				if(pas_reel != pas)
				  {
				    debut_sequence = fin_exec;
				  }
				else
				  {
				    debut_sequence += pas;
				  }
				}
			else{
				fini++ ;
				printf("Pas de solution pour %d esclave(s)\n", fini);
			}		
			
			break;
			
			default:
			break;
		}
	}
	// suppression des esclave
	for(i=0; i<nb_esclaves;i++)
	{
		info = pvm_kill(tids[i]);
		//printf("Suppression de l esclave %d: retour de pvm_kill: %d\n",i ,info);
	}
	
	pvm_exit();
	
	free(tids);
	free(mdp);
	free(argv_esclave[0]);
	free(argv_esclave[1]);

	return EXIT_SUCCESS;
}
