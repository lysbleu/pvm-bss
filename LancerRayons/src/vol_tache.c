#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

//~ #define _AFFICHE_ECHANGE_

int _index_tache_courante_, _nb_taches_;
int _recu_terminaison_ = 0;
int _envoi_terminaison_ = 0;
pthread_mutex_t _mutex_dernier_element_ = PTHREAD_MUTEX_INITIALIZER;

void *communication(void *arg)
{
	int myrank, size, donnees_recues, donnees_envoyees;
	int **taches = arg;
	int flag, demande_en_cours;
	demande_en_cours = 0;

	MPI_Request envoi, recep;
	MPI_Status status;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	MPI_Send_init(&donnees_envoyees, 1, MPI_INT, (myrank+size+1)%size, 0, MPI_COMM_WORLD, &envoi);
	MPI_Recv_init(&donnees_recues, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &recep);

	MPI_Start(&recep);
		
	while(!_recu_terminaison_ || !_envoi_terminaison_)
	{
		MPI_Request_get_status(recep, &flag, &status);
		if(flag)
		{
			switch(status.MPI_TAG){
				case 0://demande de donnees
				#ifdef _AFFICHE_ECHANGE_
				printf("Rang:%d Recep donnees\n", myrank);
				#endif /*_AFFICHE_ECHANGE_*/
				if(donnees_recues == myrank && _envoi_terminaison_ !=1)
				{
					#ifdef _AFFICHE_ECHANGE_
					printf("Rang:%d Envoi Fin\n", myrank);
					#endif /*_AFFICHE_ECHANGE_*/
					MPI_Send(&donnees_envoyees, 1, MPI_INT, (myrank+size+1)%size, 2, MPI_COMM_WORLD);
					_envoi_terminaison_ =1;
				} 
				else if (_nb_taches_ -(_index_tache_courante_+1)>=2)
				{
					#ifdef _AFFICHE_ECHANGE_
					printf("Rang:%d Envoi Tache\n", myrank);
					#endif /*_AFFICHE_ECHANGE_*/
					_nb_taches_--;
					donnees_envoyees = (*taches)[_nb_taches_];
					MPI_Send(&donnees_envoyees, 1, MPI_INT, donnees_recues, 1, MPI_COMM_WORLD);
				}
				else if(_envoi_terminaison_ !=1)
				{
					#ifdef _AFFICHE_ECHANGE_
					printf("Rang:%d Transfert\n", myrank);
					#endif /*_AFFICHE_ECHANGE_*/
					donnees_envoyees = donnees_recues;
					MPI_Start(&envoi);
				}
				break;
				case 1://reception donnees
				demande_en_cours = 0;
				#ifdef _AFFICHE_ECHANGE_
				printf("Rang:%d Donnees recues : %d Origine:%d\n", myrank, donnees_recues, status.MPI_SOURCE);
				#endif /*_AFFICHE_ECHANGE_*/
				pthread_mutex_lock(&_mutex_dernier_element_);
				(*taches)[0]=donnees_recues;
				_nb_taches_ = 1;
				_index_tache_courante_ = 0;
				pthread_mutex_unlock(&_mutex_dernier_element_);
				break;
				case 2://terminaison
				#ifdef _AFFICHE_ECHANGE_
				printf("Rang:%d Envoi Terminaison : %d Recu Fin\n", myrank, _recu_terminaison_);
				#endif /*_AFFICHE_ECHANGE_*/
				_recu_terminaison_ = 1;
				if(_envoi_terminaison_ != 1)
				{
					MPI_Send(&donnees_envoyees, 1, MPI_INT, (myrank+size+1)%size, 2, MPI_COMM_WORLD);
					_envoi_terminaison_ = 1;
				}
				default:
				break;
			}
			if(_recu_terminaison_ !=1)
			{
				MPI_Start(&recep);
			}		
		}
		if(_index_tache_courante_>=_nb_taches_)
		{
			MPI_Request_get_status(envoi, &flag, &status);
			if(flag && (demande_en_cours == 0) && (_recu_terminaison_ == 0) && (_envoi_terminaison_ == 0))
			{
				#ifdef _AFFICHE_ECHANGE_
				printf("Rang:%d Envoi demande données\n", myrank);
				#endif /*_AFFICHE_ECHANGE_*/
				demande_en_cours = 1;
				donnees_envoyees = myrank;
				MPI_Start(&envoi);
			}
		}
		usleep(10);
	}
	MPI_Wait(&envoi, MPI_STATUS_IGNORE);
	MPI_Request_free(&envoi);
	MPI_Request_free(&recep);

	return NULL;
}
