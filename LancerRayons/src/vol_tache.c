#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

int _index_tache_courante_, _nb_taches_;
int _recu_terminaison_ = 0;
int _envoi_terminaison_ = 0;
pthread_mutex_t  _mutex_tache_courante_ = PTHREAD_MUTEX_INITIALIZER;

void *communication(void *arg)
{
	int myrank, size, donnees_recues, donnees_envoyees;
	int *taches = arg;

	int flag, demande_en_cours;
	demande_en_cours = 0;

	MPI_Request envoi, recep;
	MPI_Status status;
	
	//~ MPI_Init(NULL, NULL);
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
				printf("Rang:%d Recep donnees\n", myrank);
				if(donnees_recues == myrank && _envoi_terminaison_ !=1)
				{
					printf("Rang:%d Envoi Fin\n", myrank);
					MPI_Send(&donnees_envoyees, 1, MPI_INT, (myrank+size+1)%size, 2, MPI_COMM_WORLD);
					_envoi_terminaison_ =1;
				} 
				else if (_nb_taches_ -(_index_tache_courante_+1)>=2)
				{
					printf("Rang:%d Envoi Tache\n", myrank);
					pthread_mutex_lock(&_mutex_tache_courante_);
					donnees_envoyees = taches[_index_tache_courante_];
					_index_tache_courante_++;
					pthread_mutex_unlock(&_mutex_tache_courante_);
					MPI_Send(&donnees_envoyees, 1, MPI_INT, donnees_recues, 1, MPI_COMM_WORLD);
				}
				else if(_envoi_terminaison_ !=1)
				{
					printf("Rang:%d Transfert\n", myrank);
					donnees_envoyees = donnees_recues;
					MPI_Start(&envoi);
				}
				break;
				case 1://reception donnees
				demande_en_cours = 0;
				printf("Rang:%d Donnees recues : %d Origine:%d\n", myrank, donnees_recues, status.MPI_SOURCE);
				taches[_nb_taches_]=donnees_recues;
				_nb_taches_++;
				break;
				case 2://terminaison
				printf("Rang:%d Envoi Terminaison : %d Recu Fin\n", myrank, _recu_terminaison_);
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
				printf("Rang:%d Envoi demande données\n", myrank);
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
	//~ MPI_Finalize();

	return NULL;
}
