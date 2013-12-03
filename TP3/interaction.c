#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <float.h>
#include <string.h>
#include "atom.h"
#include "parser.h"

int main( int argc, char **argv ) {
    if(argc != 3)
    {
        printf("usage: ./interaction filename #iteration\n");
        return EXIT_FAILURE;
    }

    MPI_Init( NULL, NULL ); 
    
    int nb_iter = atoi(argv[2]);
    double dt = -1;

    Atome *initialDatas;

    int myrank, size, maxElem;
    double double_tmp, sin_a, cos_a, dist_tmp;
    double *double_tmp_ptr = NULL;
    MPI_Comm_size( MPI_COMM_WORLD, &size);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	
    //parsing du fichier de configuration
    /*
     * format : 50 caracteres par ligne, flottants en ecriture scientifique
     * flottant : 1.1111E10 (9 caracteres)
     * ordre : masse pos1 pos2 vit1 vit2\n
     */	
    int elementsNumber = parse(argv[1], &initialDatas, myrank, size, &maxElem);

    Atome *buffer0 = calloc(maxElem, sizeof(Atome));
    Atome *buffer1 = calloc(maxElem, sizeof(Atome));
    Atome *inputDatas = NULL;  //variable qui pointera vers le buffer des données en cours de traitement.
    double *dist_min = calloc(maxElem, sizeof(double));
	
    for(int t=0; t<maxElem; t++)
    {
        dist_min[t] = DBL_MAX;
    }
	int indice;
    int blockLength = 3; //nb de donnees a envoyer
    
    //type MPI pour Atome, en n envoyant seulement m, pos1, pos2
    MPI_Datatype object; 
    MPI_Aint stride = sizeof(Atome)/sizeof(double);
    MPI_Type_vector(1, blockLength, stride, MPI_DOUBLE, &object);
    
    //changement de taille du type pour recevoir dans des Atomes
    MPI_Type_create_resized(object, 0, sizeof(Atome), &object);
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
    memcpy(buffer0, initialDatas, maxElem * sizeof(Atome));
    double_tmp_ptr = calloc(2, sizeof(double));
	
	//boucle principale (nb_iter == nb de points par objet)
	//tour k=0 pour initialisation des distances min
	for (int k = 0; k<=nb_iter; k++)
	{
		//calcul du dt local pour chacun des points, on garde le min
		double_tmp = 0;
		dt = DBL_MAX;
		
		if(k!=0)
		{
			for(int n = 0; n < maxElem; n++)
			{
				double_tmp = calc_dt(initialDatas[n], dist_min[n]);
				if(double_tmp > 0 && dt > double_tmp)
				{
					dt = double_tmp;
				}
			}

			//calcul du dt global avec un MPI_Allreduce
			MPI_Allreduce(MPI_IN_PLACE, &dt, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
		}
		
		for(int z = 0; z<maxElem; z++)
		{
			initialDatas[z].acc[0]  = 0;
			initialDatas[z].acc[1] = 0;	
		}
		
        //pour chaque processus
        for(int i = 0; i<size; i++)
        {
	        inputDatas = (inputDatas != &(buffer0[0])) ? &(buffer0[0]) : &(buffer1[0]); 
			indice = (inputDatas == &(buffer0[0])) ? 0 : 1;
			 
            //debut des communications non bloquantes
            MPI_Start(&(sendRequest[indice]));
            MPI_Start(&(recvRequest[indice]));
			
            //calcul des forces et influence sur les positions 
                for(int m = 0; m < maxElem; m++)
                {
                    for(int n = 0; n < maxElem; n++)
                    {
						if(!(m==n && i==0) && (initialDatas[m].m >0) && (inputDatas[n].m>0))
						{
							//MAJ des distances min
							dist_tmp = distance(initialDatas[m], inputDatas[n]);
							if(dist_min[m] > dist_tmp)
							{
								dist_min[m] = dist_tmp;
							}
							
							// attention, chacune des fonctions
							// suivantes ne calcule que l'influence
							// de l'atome courant
							double_tmp = force_inter(initialDatas[m], inputDatas[n]);
							
							sin_a = (initialDatas[m].pos[1] - inputDatas[n].pos[1])/dist_tmp;
							cos_a = (initialDatas[m].pos[0] - inputDatas[n].pos[0])/dist_tmp;
							
							double_tmp_ptr[0] = double_tmp * cos_a;
							double_tmp_ptr[1] = double_tmp * sin_a;

							acceleration(&(initialDatas[m]), double_tmp_ptr);
							if(k!=0)
							{
								vitesse(&(initialDatas[m]), dt);
							}
						}
                    }
                }

            //attente de la reception des donnees avant l etape suivante
            MPI_Wait(&(recvRequest[indice]),MPI_STATUS_IGNORE);
        }
        
        if(k!=0)
        {
            for(int z = 0; z<maxElem; z++)
            {
                new_pos(&(initialDatas[z]), dt);	
            }
            memcpy(inputDatas, initialDatas, maxElem * sizeof(Atome));
        }
				
        // ecriture du resultat
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


