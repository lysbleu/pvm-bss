
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "gmp/gmp.h"
#include "pvm3.h"

mpz_t fin_exec_global, pas_global, travail_courant_global;
mpz_t fin_exec_global_bis, pas_bis, travail_courant_global_bis;
char *solution, *solution_globale;
char *mdp;
pthread_t *threads;
pthread_t communication;
pthread_mutex_t communication_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t donnees_reserve_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t travail_courant_lock = PTHREAD_MUTEX_INITIALIZER;
int nb_threads;
int parenttid;
int donnees_reserve = 0;
int termine = 1;
int longueur_mdp;

//fonction de terminaison, qui nettoie la memoire avant de quitter 
void terminaison()
{
  fprintf(stderr, "Cleaning in progress...\n");
  mpz_clear(fin_exec_global);
  mpz_clear(fin_exec_global_bis);
  mpz_clear(pas_global);
  mpz_clear(pas_bis);
  mpz_clear(travail_courant_global);
  mpz_clear(travail_courant_global_bis);
  free(solution);
  free(mdp);
  fprintf(stderr, "Now exiting\n");
  pvm_exit();
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


//fonction qui demande les donnees
void* demande_donnees(void* arg)
{
  pthread_mutex_lock(&communication_lock);
  pvm_initsend(PvmDataDefault);
  pvm_send(parenttid,1);
  int bufid,size;
  char *recep_char, *recep_char2;

  bufid = pvm_recv(parenttid, -1 );
  pvm_upkint(&size, 1, 1);
  recep_char = calloc(size+1, sizeof(char));
  pvm_upkbyte(recep_char,size+1 , 1);
  gmp_sscanf(recep_char, "%Zd", travail_courant_global_bis);
  free(recep_char);

  pvm_upkint(&size, 1, 1);
  recep_char2 = calloc(size+1, sizeof(char));
  pvm_upkbyte(recep_char, size+1, 1);
  gmp_sscanf(recep_char, "%Zd", pas_bis);
  free(recep_char2);
  mpz_add(fin_exec_global_bis, travail_courant_global_bis, pas_bis);
  mpz_cdiv_q_ui(pas_bis, pas_bis, nb_threads);

  if (bufid < 0)
    {
      fprintf(stderr, "Erreur de reception : %d\n", bufid);
      terminaison();
    }

  //conversion de l entier en chaine de caracteres                                                                                                                                       
  //conversion(travail_courant_bis, solution);
  gmp_fprintf(stderr, "travail_courant :%Zd pas :%Zd max_travail :%Zd\n", travail_courant_global_bis, pas_bis, fin_exec_global_bis);
  pthread_mutex_unlock(&communication_lock);
  return NULL;
}

//fonction qui effectue le calcul
void calcul_local(mpz_t travail_local, mpz_t fin_exec_local, char* solution)
{
  conversion(travail_local, solution);
  while(mpz_cmp(fin_exec_local, travail_local)>0)
	{
	  //test si la solution est egale au mot de passe
	  // si oui, envoi au maitre
	  if (strcmp(solution, mdp) == 0)
	    {
	     /* fprintf(stderr, "solution trouvee:%s\n", solution);
	      pvm_initsend(PvmDataDefault);
	      pvm_pkstr(solution);
	      pvm_send(parenttid,0);*/
	      //envoi par le thread de communication
	      pthread_mutex_lock(&travail_courant_lock);
	      termine =0;
	      pthread_mutex_unlock(&travail_courant_lock);
	      break;
	    }

	  //incrementation de la solution                                                                                                               
	  incr(solution, longueur_mdp);
	  mpz_add_ui(travail_local,travail_local,1);
	}
}

//fonction qui effectue la pioche pour le thread
void pioche_donnees(mpz_t travail_local, mpz_t fin_exec_local)
{
  mpz_t aux;
  mpz_init(aux);
  if (mpz_cmp(travail_courant_global, fin_exec_global)<0)
    {
      pthread_mutex_lock(&travail_courant_lock);
      if (mpz_cmp(travail_courant_global, fin_exec_global)<0)
	{
	  mpz_set(travail_local, travail_courant_global);
	  mpz_set(fin_exec_local, travail_local);
	  
	  mpz_sub(aux, fin_exec_global, travail_courant_global);
	  if (mpz_cmp(aux, pas_global)<0)
	    {
	      mpz_set(pas_global, aux);
	    }
	  
	  mpz_add(fin_exec_local,fin_exec_local,  pas_global);
	  mpz_set(travail_courant_global, fin_exec_local);
	}
      if (mpz_cmp(travail_courant_global, fin_exec_global)>=0 && donnees_reserve==1)
	{
	  pthread_mutex_lock(&donnees_reserve_lock);
	  if(donnees_reserve==1)
	    {
	      pthread_mutex_lock(&communication_lock);
	      mpz_set(fin_exec_global, fin_exec_global_bis);
	      mpz_set(pas_global, pas_bis);
	      mpz_set(travail_courant_global, travail_courant_global_bis);
	      donnees_reserve=0;
	      pthread_mutex_unlock(&communication_lock);
	    }
	  pthread_mutex_unlock(&donnees_reserve_lock);
	}
      else if (mpz_cmp(travail_courant_global, fin_exec_global)>=0 && donnees_reserve==0)
	{
	  pthread_mutex_lock(&donnees_reserve_lock);
	  if(donnees_reserve ==0)
	    {
	      termine = 0;
	    }
	  pthread_mutex_unlock(&donnees_reserve_lock);
	}

      pthread_mutex_unlock(&travail_courant_lock);
    }
}

//fonction execute par les threads
void* thread_exec(void *arg)
{
  char* solution = (char*) calloc(longueur_mdp+1, sizeof(char)); 
  mpz_t travail_local, fin_exec_local;
  mpz_t aux;
  mpz_init(travail_local);
  mpz_init(fin_exec_local);

  while(1)
    {
      if (termine == 0)
	{
	  pthread_mutex_lock(&travail_courant_lock);
	  if (termine == 0)
	    {
	      break;
	    }
	  pthread_mutex_unlock(&travail_courant_lock);
	}
      mpz_cdiv_q_ui(aux, fin_exec_global, 2);
      if (mpz_cmp(travail_courant_global, aux)>0 && donnees_reserve != 1)
	{
	  pthread_mutex_lock(&donnees_reserve_lock);
	  pthread_mutex_lock(&travail_courant_lock);
	  if (mpz_cmp(travail_courant_global, aux)>0 && donnees_reserve != 1)
	    {
	      pthread_create(&communication,NULL, demande_donnees, NULL);
	    }
	  pthread_mutex_unlock(&travail_courant_lock);
	  pthread_mutex_unlock(&donnees_reserve_lock);
	}
      pioche_donnees(travail_local, fin_exec_local);
      
      calcul_local(travail_local, fin_exec_local, solution);
    }
  return NULL;
}

int main (int argc, char* argv[])
{
	if(argc !=4)
	{
	  printf("usage Appeler craquage, et non cet executable : ./craquage_esclave t r m\n");
	  return EXIT_FAILURE;
	}
	//gestion de la terminaison propre lors de l appel de pvm_kill
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term_signal;
	sigaction(SIGTERM, &action, NULL);

	// Initialisation des variables
	longueur_mdp = atoi(argv[2]);
	mdp = (char*) calloc(strlen(argv[3]+1), sizeof(char));
	strcpy(mdp, argv[3]);
	
	nb_threads = atoi(argv[1]);
	threads = calloc(nb_threads, sizeof(pthread_t));
	
	int parenttid = pvm_parent();

	solution_globale = (char*) calloc(longueur_mdp+1, sizeof(char)); 

	mpz_init(fin_exec_global);
	mpz_init(fin_exec_global_bis);
	mpz_init(pas_global);
	mpz_init(pas_bis);
	mpz_init(travail_courant_global);
	mpz_init(travail_courant_global_bis);
	mpz_set_ui(fin_exec_global, 0);
	mpz_set_ui(travail_courant_global, 1);
	
	demande_donnees(NULL);
	//pthread_create(&comm, NULL, demande_donnees, NULL);
	int i;
	for( i=0;i<nb_threads; i++)
	  {
	    pthread_create(&threads[i], NULL, thread_exec, NULL);
	  }

	for (i=0; i<nb_threads; i++)
	  {
	    pthread_join(threads[i], NULL);
	  }
	pthread_join(communication, NULL);

	//int bufid, size;
	//char * recep_char;
	//char * recep_char2;
	
	terminaison();
	
	return EXIT_SUCCESS;
}
