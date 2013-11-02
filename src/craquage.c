#include "pvm3.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <limits.h>
#include<sys/time.h>
#include "gmp/gmp.h"

#define TAILLE_MAX  1000
#define NB_TRANCHES 10
#define MIN(n1,n2) n1 < n2 ? n1 : n2
#define MAX(n1,n2) n1 > n2 ? n1 : n2

int main (int argc, char* argv[])
{
	if(argc !=4)
	{
		printf("usage : ./craquage p r m\n");
		return EXIT_FAILURE;
	}
	
	//Initialisation des variables
	int nb_esclaves = atoi(argv[1]);
	int* tids = (int*) calloc(nb_esclaves, sizeof(int));
	int longueur_mdp = atoi(argv[2]);
	char* mdp = (char*) calloc(strlen(argv[3])+1, sizeof(char));
	strcpy(mdp, argv[3]);

	/*char *debut_sequence, *pas_reel, *pas, *fin_exec;
	debut_sequence = calloc(1, sizeof(char)*((4*longueur_mdp+1)/3 +1));
	pas_reel = calloc(1, sizeof(char)*((4*longueur_mdp+1)/3 +1));
	pas = calloc(1, sizeof(char)*((4*longueur_mdp+1)/3 +1));
	fin_exec = calloc(1, sizeof(char)*((4*longueur_mdp+1)/3 +1));
	*/
	//declaration de type de tres long entiers (avec bibliotheque GMP)
	mpz_t debut_sequence, pas_reel, pas, fin_exec;
	//mpz_inits(debut_sequence, pas_reel, pas, fin_exec, NULL);
	mpz_init(debut_sequence);
	mpz_init(pas_reel);
	mpz_init(pas);
	mpz_init(fin_exec);
	//recuperation du chemin de l executable
	char* chemin = getcwd(NULL, 1000);
	strcat(chemin, "/craquage_esclave");
	printf("%s\n", chemin);

	//creation des arguments pour l esclave
	char *argv_esclave[3];
	argv_esclave[2]=NULL;
	argv_esclave[0] = (char*) calloc(strlen(argv[2])+1, sizeof(char));
	strcpy(argv_esclave[0],argv[2]);
	 
	argv_esclave[1] = (char*) calloc(strlen(argv[3])+1, sizeof(char));
	strcpy(argv_esclave[1],argv[3]);
	printf("strlen %d, %d\n", strlen(argv[2]), strlen(argv[3]));
	printf("nb_esclaves %d\n", nb_esclaves);
	int i;
	int trouve = 0;
	int fini = 0;
	int size;
	char* envoi_char;
	//unsigned long int debut_sequence, pas_reel, pas, fin_exec;
	
	int bufid, info, bytes, type, source;
	char * solution;
	pvm_catchout(stderr);
	struct timeval tv1, tv2;
	gettimeofday(&tv1, NULL);
	pvm_spawn(chemin, argv_esclave, PvmTaskDefault,"", nb_esclaves, tids);

	//calcul du pas, fin_exec (= fin execution)
	mpz_set_ui(debut_sequence, 0);
	mpz_ui_pow_ui(fin_exec, 15, longueur_mdp+1);	
	mpz_sub_ui(fin_exec, fin_exec, 15);
	mpz_cdiv_q_ui(fin_exec, fin_exec, 14);
	
	mpz_set(pas, fin_exec);
	mpz_cdiv_q_ui(pas, pas, nb_esclaves*longueur_mdp);
	if(mpz_cmp_ui(pas, 0)==0)
	  {
	    mpz_set_ui(pas,1); 
	  }

	gmp_printf("fin_exec: %Zd\npas:%Zd\ndebut_sequence:%Zd\n",fin_exec, pas, debut_sequence);
	//debut_sequence = 0;
	//fin_exec = ((pow(15, longueur_mdp)-1)*15)/14;
	//pas = MAX(*fin_exec/(nb_esclaves*longueur_mdp), 1);

	//printf("debut_sequence fin_exec: %lu, pas:%lu\n", fin_exec, pas); 
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
			  if(mpz_cmp(debut_sequence, fin_exec)< 0){
			    
			    mpz_sub(pas_reel, fin_exec, debut_sequence);
			    if(mpz_cmp(pas, pas_reel)<0)
			      {
				mpz_set(pas_reel, pas);
			      }
			        //*pas_reel = MIN(*pas, *fin_exec - *debut_sequence);
				//prendre en compte la fin des donnees dans le calcul du pas
				//envoi des donnes a l esclave
				pvm_initsend(PvmDataDefault);
				size = gmp_asprintf(&envoi_char, "%Zd", debut_sequence);
				pvm_pkint(&size, 1, 1);
				pvm_pkbyte(envoi_char,size+1, 1);
				free(envoi_char);
				
				size = gmp_asprintf(&envoi_char, "%Zd", pas_reel);
			      	pvm_pkint(&size, 1, 1);
				pvm_pkbyte(envoi_char,size+1 , 1);
				//pvm_pkulong(&debut_sequence, 1, 1);
				//pvm_pkulong(&pas_reel, 1, 1);
				pvm_send(source,0);
				
				if(mpz_cmp(pas_reel,pas)!=0)
				  {
				    mpz_set(debut_sequence,fin_exec);
				  }
				else
				  {
				    mpz_add(debut_sequence, debut_sequence,pas);
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
	//gettimeofday(&tv2, NULL);
	//printf("%d %ld\n",longueur_mdp,(tv2.tv_sec-tv1.tv_sec)*1000 + (tv2.tv_usec-tv1.tv_usec)/1000); 
	//mpz_clears(debut_sequence, pas_reel, pas, fin_exec, NULL);
	mpz_clear(debut_sequence);
	mpz_clear(pas_reel);
	mpz_clear(pas);
	mpz_clear(fin_exec);
	free(tids);
	free(mdp);
	free(argv_esclave[0]);
	free(argv_esclave[1]);

	return EXIT_SUCCESS;
}
