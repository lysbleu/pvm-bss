#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "atom.h"
#include <string.h>
#include "atom.h"
#include "parser.h"
#include <unistd.h>

void copyAtome(Atome *dest, Atome *src)
{
		dest->m = src->m;
		dest->pos[0] = src->pos[0];
		dest->pos[1] = src->pos[1];
		dest->vit[0] = src->vit[0];
		dest->vit[1] = src->vit[1];
		dest->acc[0] = src->acc[0];
		dest->acc[1] = src->acc[1];
}

int main( int argc, char **argv ) {
    if(argc != 4)
    {
        printf("usage: ./interaction nom_de_fichier nb_iter mode\n");
        return EXIT_FAILURE;
    }

    MPI_Init( NULL, NULL ); 
    
    int nb_iter = atoi(argv[2]);
    int mode = atoi(argv[3]);
    double dt = 0;
    char *filename = calloc(strlen(argv[1])+1, sizeof(char));
    strcpy(filename, argv[1]);

    Atome *initialDatas;

    int myrank, size, maxElem;
    double double_tmp, dt_min;
    double *double_tmp_ptr = NULL;
    MPI_Comm_size( MPI_COMM_WORLD, &size);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	
	//parsing du fichier de configuration
	/*
	 * format : 50 caracteres par ligne, flottants en ecriture scientifique
	 * flottant : 1.1111E10 (9 caracteres)
	 * ordre : masse pos1 pos2 vit1 vit2\n
	 */	
    int elementsNumber = parse(filename, &initialDatas, myrank, size, &maxElem);

    Atome *buffer0 = calloc(maxElem, sizeof(Atome));
    Atome *buffer1 = calloc(maxElem, sizeof(Atome));
    Atome *inputDatas = NULL;  //variable qui pointera vers le buffer des données en cours de traitement.
	double *dist_min = calloc(size, sizeof(double));
	
    int blockLength = 3; //nb de donnees a envoyer
    
    //type MPI pour Atome, en n envoyant seulement m, pos1, pos2
    MPI_Datatype object; 
    MPI_Aint stride = sizeof(Atome)/sizeof(double);
    MPI_Type_vector(1, blockLength, stride, MPI_DOUBLE, &object);
    MPI_Type_commit(&object); 
    
    MPI_Request sendRequest[2];
    MPI_Request recvRequest[2];

	//initialisation des communications persistantes
    MPI_Send_init(buffer0, maxElem, object, (myrank + 1) % size, 2,
				 MPI_COMM_WORLD, &(sendRequest[0]));
				
    MPI_Send_init(buffer1, maxElem, object, (myrank + 1) % size, 1,
				MPI_COMM_WORLD, &(sendRequest[1]));
				 
    MPI_Recv_init(buffer1, maxElem, object, (myrank + size -1) % size, 2,
				 MPI_COMM_WORLD, &(recvRequest[0]));
				 
    MPI_Recv_init(buffer0, maxElem, object, (myrank + size -1) % size, 1,
				 MPI_COMM_WORLD, &(recvRequest[1]));
    
    char command[200];

    //initialisation du buffer local
	for (int i = 0; i <maxElem; i++)
	{
		copyAtome(&(buffer0[i]), &(initialDatas[i]));
	}
	
	//initialisation du tableau des distances min
	//faire un tour d'algo
	for(int i = 0; i<size; i++)
	{
		MPI_Sendrecv(initialDatas, maxElem, object, (myrank + 1) % size, 2,
                buffer0, maxElem,  object, (myrank + size - 1) % size, MPI_ANY_TAG,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for(int j = 0; j<maxElem; j++)
		{
			for(int k = 0; k <maxElem; k++)
			{
				//dist_min[i]=min(dist recues)
				double_tmp = distance(buffer0[j], initialDatas[k]);
				if(dist_min[i] > double_tmp)
				{
					dist_min[i]= double_tmp;
				}
			}
		}
	}
	
	double_tmp_ptr = calloc(2, sizeof(double));
	
	//boucle principale (nb_iter == nb de points par courbe)
	for (int k = 0; k<nb_iter; k++)
	{
		//calcul du dt local pour chacun des points, on garde le min
		double_tmp = 0;
		for(int m = 0; m < maxElem; m++)
		{
			for(int n = 0; n < size; n++)
			{
				double_tmp = calc_dt(initialDatas[m], dist_min[n]);
				if(dt_min > double_tmp)
				{
					dt_min = double_tmp;
				}
			}
		}
		  
		//calcul du dt global avec un MPI_Allreduce
		MPI_Allreduce(MPI_IN_PLACE, &dt, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
		
		if((k%2) == 0)
		{
			inputDatas = &buffer0[0];
		}
		else
		{
			inputDatas = &buffer1[0];
		}
		
		//pour chaque processus
		for(int i = 0; i<size; i++)
		{
			//debut des communications non bloquantes
			MPI_Start(&(sendRequest[i%2]));
			MPI_Start(&(recvRequest[i%2]));
			
			//calcul des forces et influence sur les positions 
			for (int l=0; l<maxElem; l++)
			{
				if(i!=0)//cas general
				{
					for(int m = 0; m < maxElem; m++)
					{
						for(int n = 0; n < maxElem; n++)
						{
							// attention, chacune des fonctions
							// suivantes ne calcule que l'influence
							// de l'atome courrant
							double_tmp = force_inter(initialDatas[m], inputDatas[n]);
							
							double_tmp_ptr[0] = double_tmp * cos(distance(initialDatas[m], inputDatas[n]));
							double_tmp_ptr[1] = double_tmp * sin(distance(initialDatas[m], inputDatas[n]));
							
							acceleration(&initialDatas[m], double_tmp_ptr);
							vitesse(&initialDatas[m], dt);
							new_pos(&initialDatas[m], dt);
						}
					}
				}
				
				else//cas particulier, ne pas prendre en compte l influence sur soi meme
				{
					for(int m = 0; m < maxElem; m++)
					{
						for(int n = 0; n < maxElem; n++)
						{
							if(m != n)
							{
								// attention, chacune des fonctions
								// suivantes ne calcule que l'influence 
								// de l'atome courrant
								double_tmp = force_inter(initialDatas[m], inputDatas[n]);
							
								double_tmp_ptr[0] = double_tmp * cos(distance(initialDatas[m], inputDatas[n]));
								double_tmp_ptr[1] = double_tmp * sin(distance(initialDatas[m], inputDatas[n]));
							
								acceleration(&initialDatas[m], double_tmp_ptr);
								vitesse(&initialDatas[m], dt);
								new_pos(&initialDatas[m], dt);
							}
						}
					}
				}
					
				//MAJ du tab des distances min
				//TODO
			}
			//attente de la reception des donnees avant l etape suivante
			MPI_Wait(&(recvRequest[i%2]),MPI_STATUS_IGNORE);
		}
		
		//ecriture du resultat
		for (int j=0; j<elementsNumber; j++)
		{
			if(k==0)//ecrasement si fichier existe deja
			{
				sprintf(command, "echo %lf %lf > results/res_%d_%d.txt", initialDatas[j].pos[0],initialDatas[j].pos[1], myrank, j);	
				system(command);
			}
			else//concatenation des donnees
			{
				sprintf(command, "echo %lf %lf >> results/res_%d_%d.txt", initialDatas[j].pos[0],initialDatas[j].pos[1], myrank, j);	
				system(command);	
			}
		}
    }
    
    //liberation des allocations MPI
    MPI_Request_free(&(sendRequest[0]));
    MPI_Request_free(&(sendRequest[1]));
    MPI_Request_free(&(recvRequest[0]));
    MPI_Request_free(&(recvRequest[1]));
    
    MPI_Type_free(&object);
    
    //liberation des allocations dynamiques
    free(buffer0);
    free(buffer1);
    free(initialDatas);
    free(dist_min);
    free(double_tmp_ptr);

    MPI_Finalize();
    return EXIT_SUCCESS;
} 


