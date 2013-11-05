#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <signal.h>
#include "gmp/gmp.h"
#include "pvm3.h"

mpz_t max_travail, pas, travail_courant;
char *solution;
char *mdp;
//fonction de terminaison, qui nettoie la memoire avant de quitter 
void terminaison()
{
  fprintf(stderr, "Cleaning in progress...\n");
  mpz_clear(max_travail);
  mpz_clear(pas);
  mpz_clear(travail_courant);
  free(solution);
  free(mdp);
  fprintf(stderr, "Now exiting\n");
  pvm_exit();//non necessaire, car cree par spawn
  exit(1);
}

//fonction appelee lors du captage de SIGTERM (pvm_kill)
void term_signal(int signum)
{
  fprintf(stderr, "Received SIGTERM, cleaning...\n");
  terminaison();
}

//fonction qui incremente la solution d un rang
void incr(char* solution_arg, int longueur_max)
{
	int continuer = 1;
	int i = 0;
	while( continuer == 1 && i<longueur_max )
	{
		if(solution_arg[i] == '\0')
		{
			solution_arg[i] = 'a';
			i++;
			continuer = 0;
		}
		else if (solution_arg[i] == 'o')
		{
		  solution_arg[i]='a';
			i++;
		}
		else
		{
			solution_arg[i]++;
			continuer = 0;
			i++;
		}
	}
}

//fonction de conversion du tres grand entier en base 15 en chaine de caracteres
void conversion(mpz_t code_cons, char* solution_arg)
{
	int i = 0;
	mpz_t reste, code;
	mpz_init(reste);
	mpz_init(code);
	mpz_set(code, code_cons);
	int inf_15 = 0;
	while(mpz_cmp_ui(code,0)!=0 || inf_15 !=1  )
	{
	  if (mpz_cmp_ui(code,15)<0)
	    {
	      inf_15 =1;
	    }
	  mpz_mod_ui(reste, code, 15);
	  solution_arg[i] = 'a' + mpz_get_ui(reste);
	  mpz_sub(code, code, reste);
	  mpz_divexact_ui(code, code, 15);
	  i++;
	}
	solution_arg[i] = '\0';
	mpz_clear(reste);
}

int main (int argc, char* argv[])
{
	if(argc !=3)
	{
	  printf("usage Appeler craquage, et non cet executable : ./craquage_esclave r m\n");
	  return EXIT_FAILURE;
	}
	//gestion de la terminaison propre lors de l appel de pvm_kill
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term_signal;
	sigaction(SIGTERM, &action, NULL);

	// Initialisation des variables
	int longueur_mdp = atoi(argv[1]);
	mdp = (char*) calloc(strlen(argv[2]+1), sizeof(char));
	strcpy(mdp, argv[2]);
	
	int parenttid = pvm_parent();

	mpz_init(max_travail);
	mpz_init(pas);
	mpz_init(travail_courant);
	mpz_set_ui(max_travail, 0);
	mpz_set_ui(travail_courant, 1);

	int bufid, size;
	char * recep_char;
	char * recep_char2;
	solution = (char*) calloc(longueur_mdp+1, sizeof(char)); ; 
	
	while(1)
	{
	  //si l esclave a fini le travail qu il avait
	  // il en demande au maitre
	  if(mpz_cmp(travail_courant, max_travail)>=0)
	    {
	      pvm_initsend(PvmDataDefault);
	      pvm_send(parenttid,1);
	      
	      bufid = pvm_recv(parenttid, -1 );
	      pvm_upkint(&size, 1, 1);
	      recep_char = calloc(size+1, sizeof(char));
	      pvm_upkbyte(recep_char,size+1 , 1);
	      gmp_sscanf(recep_char, "%Zd", travail_courant);
	      free(recep_char);
	
	      pvm_upkint(&size, 1, 1);			
	      recep_char2 = calloc(size+1, sizeof(char));
	      pvm_upkbyte(recep_char, size+1, 1);
	      gmp_sscanf(recep_char, "%Zd", pas);
	      free(recep_char2);
	      mpz_add(max_travail, travail_courant, pas);

	      if (bufid < 0)
		{
		  fprintf(stderr, "Erreur de reception : %d\n", bufid);
		  terminaison();
		}

	      //conversion de l entier en chaine de caracteres
	      conversion(travail_courant, solution);
	      gmp_fprintf(stderr, "solution :%s travail_courant :%Zd pas :%Zd max_travail :%Zd\n",solution, travail_courant, pas,max_travail);
	    }
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
	
	terminaison();
	
	return EXIT_SUCCESS;
}
