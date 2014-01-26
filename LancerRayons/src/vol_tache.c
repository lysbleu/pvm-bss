#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int i, nb_taches;
int recu_terminaison = 0;
int envoi_terminaison =0;

void traitement_tache(int temps)
{
	printf("Temps : %d\n", temps);
	sleep(temps);
}

void generation_aleatoire(int *taches)
{
	int randomData = open("/dev/urandom", O_RDONLY);	
	read(randomData, ((char*)&nb_taches), sizeof(int));
	nb_taches = abs(nb_taches)%10+1;
	read(randomData, ((char*)taches), sizeof(int)*nb_taches);
	int j;
	for(j=0; j<nb_taches; j++)
	{
		taches[j]=abs(taches[j])%20 +1;
	}
	
	close(randomData);
}

void *communication(void *arg)
{
	int myrank, size, donnees_recues, donnees_envoyees;
	int *taches = arg;

	int flag, demande_en_cours;
	demande_en_cours = 0;

	MPI_Request envoi, recep;
	MPI_Status status;
	
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	MPI_Send_init(&donnees_envoyees, 1, MPI_INT, (myrank+size+1)%size, 0, MPI_COMM_WORLD, &envoi);
	MPI_Recv_init(&donnees_recues, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &recep);

	MPI_Start(&recep);
		
	while(!recu_terminaison || !envoi_terminaison)
	{
		MPI_Request_get_status(recep, &flag, &status);
		if(flag)
		{
			switch(status.MPI_TAG){
				case 0://demande de donnees
				printf("Rang:%d Recep donnees\n", myrank);
				if(donnees_recues == myrank && envoi_terminaison !=1)
				{
					printf("Rang:%d Envoi Fin\n", myrank);
					MPI_Send(&donnees_envoyees, 1, MPI_INT, (myrank+size+1)%size, 2, MPI_COMM_WORLD);
					envoi_terminaison =1;
				} 
				else if (nb_taches -(i+1)>=2)
				{
					printf("Rang:%d Envoi Tache\n", myrank);
					donnees_envoyees = taches[i];
					i++;
					MPI_Send(&donnees_envoyees, 1, MPI_INT, donnees_recues, 1, MPI_COMM_WORLD);
				}
				else if(envoi_terminaison !=1)
				{
					printf("Rang:%d Transfert\n", myrank);
					donnees_envoyees = donnees_recues;
					MPI_Start(&envoi);
				}
				break;
				case 1://reception donnees
				demande_en_cours = 0;
				printf("Rang:%d Donnees recues : %d Origine:%d\n", myrank, donnees_recues, status.MPI_SOURCE);
				taches[nb_taches]=donnees_recues;
				nb_taches++;
				break;
				case 2://terminaison
				printf("Rang:%d Envoi Terminaison : %d Recu Fin\n", myrank, envoi_terminaison);
				recu_terminaison = 1;
				if(envoi_terminaison != 1)
				{
					MPI_Send(&donnees_envoyees, 1, MPI_INT, (myrank+size+1)%size, 2, MPI_COMM_WORLD);
					envoi_terminaison = 1;
				}
				default:
				break;
			}
			if(recu_terminaison !=1)
			{
				MPI_Start(&recep);
			}		
		}
		if(i>=nb_taches)
		{
			MPI_Request_get_status(envoi, &flag, &status);
			if(flag && (demande_en_cours == 0) && (recu_terminaison == 0) && (envoi_terminaison == 0))
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
	return NULL;
}

int main(int argc, char* argv[])
{

	int taches[100];
	pthread_t thread_communication;
	
	generation_aleatoire(taches);

	i=0;
	pthread_create(&thread_communication, NULL, communication, taches);
	while(!recu_terminaison || !envoi_terminaison || i<nb_taches)
	{		
		if(i<nb_taches)
		{
			i++;
			traitement_tache(taches[i-1]);
		}
	}
	pthread_join(thread_communication, NULL);
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
