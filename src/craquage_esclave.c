#include "pvm3.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

void incr(char* solution, int longueur_max)
{
	int continuer = 1;
	int i = 0;
	while( continuer == 1 && i<longueur_max )
	{
		if(solution[i] == '\0')
		{
			solution[i] = 'a';
			continuer = 0;
		}
		else if (solution[i] == 'o')
		{
			i++;
		}
		else
		{
			solution[i]++;
			continuer = 0;
		}
	}
}

void conversion(int code, char* solution)
{
	int i = 0;
	int reste;
	
	while(code != 0)
	{
		reste = code % 15;
		solution[i] = 'a' + reste;
		code -= reste;
		code /= 15;
		i++;
	}
	solution[i] = '\0';
}

int main (int argc, char* argv[])
{
	if(argc !=3)
	{
		printf("usage Appeler craquage, et non cet executable : ./craquage_esclave r m\n");
		return EXIT_FAILURE;
	}
	fprintf(stderr, "debut\n");

	// Initialisation des variables
	int longueur_mdp = atoi(argv[1]);
	char* mdp = (char*) calloc(longueur_mdp+1, sizeof(char));
	strcpy(mdp, argv[2]);
	
	int parenttid = pvm_parent();
	int max_travail, pas;
	max_travail = -1;
	int travail_courant, bufid;
	travail_courant = 0;
	char * solution = (char*) calloc(longueur_mdp+1, sizeof(char)); ; 
	
	printf("avant while\n");
	while(1)
	{
		//si l esclave a fini le travail qu il avait
		if(travail_courant >= max_travail)
		{
			pvm_initsend(PvmDataDefault);
			pvm_send(parenttid,1);
			
			bufid = pvm_recv(parenttid, -1 );
			pvm_upkint(&travail_courant, 1, 1);
			pvm_upkint(&pas, 1, 1);
			max_travail = travail_courant + pas;
				
			if (bufid < 0)
			{
				printf("Erreur de reception : %d\n", bufid);
				exit(1);
			}
			
			//conversion de l entier en chaine de caracteres
			conversion(travail_courant, solution);
			fprintf(stderr, "solution :%s\ntravail_courant :%d\nmax_travail :%d\n",solution, travail_courant, max_travail);
		}
		
		//incrementation de la solution
		incr(solution, longueur_mdp);
		
		//test si la solution est egale au mot de passe
		// si oui, envoi au maitre
		if (strcmp(solution, mdp) == 0)
		{
			pvm_initsend(PvmDataDefault);
			pvm_pkstr(solution);
			pvm_send(parenttid,0);
		}
		
	}
	
	pvm_exit();
	
	free(solution);

	return EXIT_SUCCESS;
}
