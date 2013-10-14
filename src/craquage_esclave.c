#include "pvm3.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int main (int argc, char* argv[])
{
	if(argc !=4)
	{
		printf("usage Appeler craquage, et non cet executable : ./craquage_esclave r m\n");
		return EXIT_FAILURE;
	}
	// Initialisation des variables
	int longueur_mdp = atoi(argv[1]);
	char* mdp = (char*) calloc(longueur_mdp+1, sizeof(char));
	strcpy(mdp, argv[2]);
	
	int parenttid = pvm_parent();
	int max_travail, pas;
	int travail_courant;
	char * solution = (char*) calloc(longueur_mdp+1, sizeof(char)); ; 
	
	while(1)
	{
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
				printf("Erreur de reception : %d\n", info);
				exit(1);
			}
		}
		
		//faire les tests et incrementer

	}
	
	pvm_exit();
	
	free(solution);

	return EXIT_SUCCESS;
}
