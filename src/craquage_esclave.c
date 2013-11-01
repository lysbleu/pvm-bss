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
			i++;
			continuer = 0;
		}
		else if (solution[i] == 'o')
		{
		  solution[i]='a';
			i++;
		}
		else
		{
			solution[i]++;
			continuer = 0;
			i++;
		}
	}
	//solution[i] = '\0';
	//printf("incr :%s\n",solution);
}

void conversion(unsigned long code, char* solution)
{
	int i = 0;
	int reste;
	int inf_15 = 0;
	while(code != 0 || inf_15 !=1  )
	{
	  if (code <15)
	    {
	      inf_15 =1;
	    }
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
	//fprintf(stderr, "debut\n");

	// Initialisation des variables
	int longueur_mdp = atoi(argv[1]);
	char* mdp = (char*) calloc(strlen(argv[2]+1), sizeof(char));
	strcpy(mdp, argv[2]);
	
	//int maximum = 0;

	int parenttid = pvm_parent();
	unsigned long int max_travail, pas;
	max_travail = 0;
	unsigned long int travail_courant;
	  int bufid;
	travail_courant = 1;
	char * solution = (char*) calloc(longueur_mdp+1, sizeof(char)); ; 
	
	//fprintf(stderr, "mdp : %s\n", mdp);
	while(1)
	{
	  //fprintf(stderr, "avant\n");
		//si l esclave a fini le travail qu il avait
		if(travail_courant >= max_travail)
		{
		  //fprintf(stderr, "esclave a fini son travail et en demande\n");
			pvm_initsend(PvmDataDefault);
			pvm_send(parenttid,1);
			
			bufid = pvm_recv(parenttid, -1 );
			pvm_upkulong(&travail_courant, 1, 1);
			pvm_upkulong(&pas, 1, 1);
			max_travail = travail_courant + pas;
				
			if (bufid < 0)
			{
			  fprintf(stderr, "Erreur de reception : %d\n", bufid);
				exit(1);
			}
			
			//conversion de l entier en chaine de caracteres
			conversion(travail_courant, solution);
			fprintf(stderr, "solution :%s travail_courant :%lu max_travail :%lu\n",solution, travail_courant, max_travail);
		}
		//fprintf(stderr, "apres");
		
		//test si la solution est egale au mot de passe
		// si oui, envoi au maitre
		if (strcmp(solution, mdp) == 0)
		{
		  fprintf(stderr, "solution trouvee:%s\n", solution);
			pvm_initsend(PvmDataDefault);
			pvm_pkstr(solution);
			pvm_send(parenttid,0);
			break;
		}

                //incrementation de la solution                                                                                                               
                incr(solution, longueur_mdp);
                travail_courant++;
	}
	
	pvm_exit();
	
	free(solution);

	return EXIT_SUCCESS;
}
