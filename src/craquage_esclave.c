#include "pvm3.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "gmp/gmp.h"
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

void conversion(mpz_t code, char* solution)
{
	int i = 0;
	mpz_t reste;
	mpz_init(reste);
	int inf_15 = 0;
	while(mpz_cmp_ui(code,0)!=0 || inf_15 !=1  )
	{
	  if (mpz_cmp_ui(code,15)<0)
	    {
	      inf_15 =1;
	    }
	  mpz_cdiv_r_ui(reste, code, 15);
	  //reste = *code % 15;
	  solution[i] = 'a' + mpz_get_ui(reste);
	  mpz_sub(code, code, reste);
	  mpz_cdiv_q_ui(code, code, 15);
	  //*code -= reste;
	  //*code /= 15;
		i++;
	}
	solution[i] = '\0';
	mpz_clear(reste);
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
	//unsigned long int max_travail, pas;
	//char *pas = calloc(1, sizeof(char)*((4*longueur_mdp+1)/3 +1));
	//char *max_travail = calloc(1, sizeof(char)*((4*longueur_mdp+1)/3 +1));
	//*max_travail = 0;
	mpz_t max_travail, pas, travail_courant;
	//mpz_inits(max_travail, pas, travail_courant, NULL);
	mpz_init(max_travail);
	mpz_init(pas);
	mpz_init(travail_courant);
	mpz_set_ui(max_travail, 0);
	mpz_set_ui(travail_courant, 1);
	//unsigned long int travail_courant;
	//char *travail_courant = calloc(1, sizeof(char)*((4*longueur_mdp+1)/3 +1));
	int bufid, size;
	char * recep_char;
	//*travail_courant = 1;
	char * solution = (char*) calloc(longueur_mdp+1, sizeof(char)); ; 
	
	//fprintf(stderr, "mdp : %s\n", mdp);
	while(1)
	{
	  //fprintf(stderr, "avant\n");
		//si l esclave a fini le travail qu il avait
	  if(mpz_cmp(travail_courant, max_travail)>=0)
		{
		  //fprintf(stderr, "esclave a fini son travail et en demande\n");
			pvm_initsend(PvmDataDefault);
			pvm_send(parenttid,1);
			
			bufid = pvm_recv(parenttid, -1 );
			//pvm_upkulong(travail_courant, 1, 1);
			//pvm_upkulong(pas, 1, 1);
			pvm_upkint(&size, 1, 1);
			recep_char = calloc(size, sizeof(char));
			pvm_upkbyte(recep_char,size , 1);
			gmp_sscanf(recep_char, "%Zd", travail_courant);
			
			pvm_upkint(&size, 1, 1);			
			recep_char = realloc(recep_char, sizeof(char)*size);
			memset(recep_char, 0, sizeof(char)*size);
			pvm_upkbyte(recep_char, size, 1);
			gmp_sscanf(recep_char, "%Zd", pas);
			
			mpz_add(max_travail, travail_courant, pas);
			//*max_travail = *travail_courant + *pas;
				
			if (bufid < 0)
			{
			  fprintf(stderr, "Erreur de reception : %d\n", bufid);
				exit(1);
			}
			
			//conversion de l entier en chaine de caracteres
			conversion(travail_courant, solution);
			gmp_fprintf(stderr, "solution :%s travail_courant :%Zd max_travail :%Zd\n",solution, travail_courant, max_travail);
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
                mpz_add_ui(travail_courant,travail_courant,1);
	}
	
	pvm_exit();
	
	free(solution);

	return EXIT_SUCCESS;
}
