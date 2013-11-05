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
pthread_mutex_t termine_lock = PTHREAD_MUTEX_INITIALIZER;
int nb_threads;
int parenttid;
int donnees_reserve = 0;
int termine = 1;
int longueur_mdp;

//fonction de terminaison, qui nettoie la memoire avant de quitter 
void terminaison()
{
  int i ;
  fprintf(stderr, "Cleaning in progress...\n");
  for(i=0; i<nb_threads; i++)
    {
      //pthread_cancel(threads[i]);
    }
  free(pthreads);	
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
  //pthread_mutex_lock(&communication_lock); //fait avant le thread_create
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  int info, bytes, type, source;
  if(arg==NULL)
    {
      //fprintf(stderr, "test1\n");
      pvm_initsend(PvmDataDefault);
      pvm_send(parenttid,1);
      int bufid,size;
      char *recep_char, *recep_char2;
      bufid = pvm_recv(parenttid, -1 );
      info = pvm_bufinfo( bufid, &bytes, &type, &source );
      
      if(type == 0)// tag0 donc nouvelle intervalle recu
	{
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
	  mpz_cdiv_q_ui(pas_bis, pas_bis, nb_threads*longueur_mdp);

	  if (bufid < 0)
	    {
	      fprintf(stderr, "Erreur de reception : %d\n", bufid);
	      pthread_exit(NULL);
	    }
	
	  //gmp_fprintf(stderr, "travail_courant_bis :%Zd pas_bis :%Zd max_travail_bis :%Zd\n", travail_courant_global_bis, pas_bis, fin_exec_global_bis);
	}
      else //tag 1 donc le maitre n a plus de donnees à partager
	{
	  mpz_set_ui(travail_courant_global_bis, 0);
	  mpz_set_ui(pas_bis, 0);
	  mpz_set_ui(fin_exec_global_bis, 0);
	  pthread_mutex_lock(&termine_lock);
	  termine = 0;
	  pthread_mutex_unlock(&termine_lock);
	}
      pthread_mutex_unlock(&communication_lock);
    }
  else if (arg == (void*)1) // un thread a trouve la solution, envoi du resultat au maitre
    {
      pvm_initsend(PvmDataDefault);
      pvm_pkstr(solution_globale); 
      pvm_send(parenttid,0);
    }
  pthread_mutex_unlock(&communication_lock);
  return NULL;
}

//fonction qui effectue le calcul
void calcul_local(mpz_t travail_local, mpz_t fin_exec_local, char* solution)
{
  //gmp_fprintf(stderr, "travail_local:%Zd fin_exec_local:%Zd", travail_local, fin_exec_local); 
  conversion(travail_local, solution);
  while(mpz_cmp(fin_exec_local, travail_local)>0)
	{
	  //test si la solution est egale au mot de passe
	  // si oui, envoi au maitre
	  if (strcmp(solution, mdp) == 0)
	    {
	      fprintf(stderr, "solution trouvee:%s\n", solution);
	      //envoi par le thread de communication
	      pthread_mutex_lock(&termine_lock);
	      strcpy(solution_globale, solution);
	      termine =0;
	      pthread_mutex_lock(&communication_lock);
	      pthread_create(&communication,NULL, demande_donnees, (void*)1);
	      pthread_mutex_unlock(&termine_lock);
	      break;
	    }

	  //incrementation de la solution                                                                                                               
	  incr(solution, longueur_mdp);
	  mpz_add_ui(travail_local,travail_local,1);
	}
}e

//fonction qui effectue la pioche pour le thread
void pioche_donnees(mpz_t travail_local, mpz_t fin_exec_local)
{
  mpz_t aux;
  mpz_init(aux);
  //fprintf(stderr, "test pioche 0\n");
  pthread_mutex_lock(&travail_courant_lock);
  
  //si l intervalle actuel est vide et qu il y a des donnees
  if (mpz_cmp(travail_courant_global, fin_exec_global)>=0 && donnees_reserve==1)
    {
      pthread_mutex_lock(&donnees_reserve_lock);
      if(donnees_reserve==1)
	{
	  pthread_mutex_lock(&communication_lock);
	  //on recupere les donnees
	  mpz_set(fin_exec_global, fin_exec_global_bis);
	  mpz_set(pas_global, pas_bis);
	  mpz_set(travail_courant_global, travail_courant_global_bis);
	  donnees_reserve=0;//plus de donnees en reserve
	  //gmp_fprintf(stderr, "pioche donnees travail_courant_global :%Zd pas_global :%Zd fin_exec_global :%Zd\n", travail_courant_global, pas_global, fin_exec_global);   
	  pthread_mutex_unlock(&communication_lock);
	}
      pthread_mutex_unlock(&donnees_reserve_lock);
    }
  //si il reste du travail dans l intervalle actuel
  if (mpz_cmp(travail_courant_global, fin_exec_global)<0)
    {
      //on assigne le debut de l intervalle
      mpz_set(travail_local, travail_courant_global);
      mpz_set(fin_exec_local, travail_local);
	  
      //calcul du pas reel
      mpz_sub(aux, fin_exec_global, travail_courant_global);
      if (mpz_cmp(aux, pas_global)<0)
	{
	  mpz_set(pas_global, aux);
	}
      //calcul de la fin de l intervalle de calcul
      mpz_add(fin_exec_local,fin_exec_local,  pas_global);
      //augmentation de l intervalle global
      mpz_set(travail_courant_global, fin_exec_local);
    }
      
  pthread_mutex_unlock(&travail_courant_lock);
}

//fonction execute par les threads
void* thread_exec(void *arg)
{
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  char* solution = (char*) calloc(longueur_mdp+1, sizeof(char)); 
  mpz_t travail_local, fin_exec_local;
  mpz_t aux;
  mpz_init(travail_local);
  mpz_init(fin_exec_local);
  mpz_init(aux);
  while(1)
    {
      if (termine == 0)
	{
	  //test de terminaison de boucle
	  pthread_mutex_lock(&termine_lock);
	  if (termine == 0)
	    {
	      break;
	    }
	  pthread_mutex_unlock(&termine_lock);
	}
      mpz_cdiv_q_ui(aux, fin_exec_global, 2);
      //si on a consomme plus de la moitie de l intervalle de calcul, et qu il n y a pas de donnees en reserve
      if (mpz_cmp(travail_courant_global, aux)>=0 && donnees_reserve != 1)
	{
	  pthread_mutex_lock(&travail_courant_lock);
	  mpz_cdiv_q_ui(aux, fin_exec_global, 2);
	  
	  pthread_mutex_lock(&donnees_reserve_lock);
	  if (mpz_cmp(travail_courant_global, aux)>=0 && donnees_reserve != 1)
	    {
	      //on demande de nouvelles donnees
	      //gmp_fprintf(stderr, "demande de donnees\n");
	      donnees_reserve = 1;
	      pthread_mutex_lock(&communication_lock);
	      pthread_create(&communication,NULL, demande_donnees, NULL);
	    }
	  pthread_mutex_unlock(&donnees_reserve_lock);
	  pthread_mutex_unlock(&travail_courant_lock);
	}
      // on recupere de nouvelle donnees pour le thread
       pioche_donnees(travail_local, fin_exec_local);

      //on calcule sur l intervalle de donnees du thread
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
	
	parenttid = pvm_parent();

	solution_globale = (char*) calloc(longueur_mdp+1, sizeof(char)); 

	mpz_init(fin_exec_global);
	mpz_init(fin_exec_global_bis);
	mpz_init(pas_global);
	mpz_init(pas_bis);
	mpz_init(travail_courant_global);
	mpz_init(travail_courant_global_bis);
	mpz_set_ui(fin_exec_global, 0);
	mpz_set_ui(travail_courant_global, 1);
	
	pthread_mutex_lock(&communication_lock);
	demande_donnees(NULL);
	//on recupere les donnees
	mpz_set(fin_exec_global, fin_exec_global_bis);
	mpz_set(pas_global, pas_bis);
	mpz_set(travail_courant_global, travail_courant_global_bis);
	donnees_reserve=0;//plus de donnees en reserve
	     
	int i;
	//creation des threads de calcul
	for( i=0;i<nb_threads; i++)
	  {
	    pthread_create(&threads[i], NULL, thread_exec, NULL);
	  }

	//attente de terminaison
	for (i=0; i<nb_threads; i++)
	  {
	    pthread_join(threads[i], NULL);
	  }
	pthread_join(communication, NULL);
	
	terminaison();
	
	return EXIT_SUCCESS;
}
