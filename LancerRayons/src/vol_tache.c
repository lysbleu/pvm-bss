#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <time.h>

void traitement_tache(int temps)
{
	//~ printf("Bonjour c'est moi la tâche, je suis Stéphane\n");
	printf("Temps : %d\n", temps);
	sleep(temps);
}

int main( int argc, char* argv[])
{
	int myrank, size, donnees_recues, donnees_envoyees, nb_taches;
	int recu_terminaison = 0;
	int envoi_terminaison =0;
	int flag, demande_en_cours;
	MPI_Request envoi, recep;
	MPI_Status status;
	int taches[100];
	int i;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	//~ sleep(myrank);
	srand(time(NULL));
	demande_en_cours = 0;
	
	MPI_Send_init(&donnees_envoyees, 1, MPI_INT, (myrank+size+1)%size, 0, MPI_COMM_WORLD, &envoi);
	MPI_Recv_init(&donnees_recues, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &recep);
	nb_taches  = 5;

	for(i=0; i<nb_taches; i++)
	{
		taches[i]=myrank*((rand()% 20) + 1);
	}
	i=0;
	
	MPI_Start(&recep);
	while(!recu_terminaison || !envoi_terminaison || i<nb_taches)
	{
		MPI_Request_get_status(recep, &flag, &status);
		if(flag)
		{
			//~ printf("rang :%d Recep\n", myrank);
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
				printf("rang :%d envoi term : %d Recu Fin \n", myrank, envoi_terminaison);
				recu_terminaison =1;
				if(envoi_terminaison !=1)
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
		
		if(i<nb_taches)
		{
			traitement_tache(taches[i]);
			i++;
		}
		else
		{
			MPI_Request_get_status(envoi, &flag, &status);
			if(flag && (demande_en_cours == 0) && (recu_terminaison == 0) && (envoi_terminaison == 0))
			{
				printf("rang :%d Envoi demande données\n", myrank);
				demande_en_cours = 1;
				donnees_envoyees = myrank;
				MPI_Start(&envoi);
			}
		}
	}
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
