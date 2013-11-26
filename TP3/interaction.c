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
    double double_tmp;
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
	
    int blockLength = 3; //nb de donnees a envoyer
    
    //type MPI pour Atome, en n envoyant seulement m, pos1, pos2
    MPI_Datatype object; 
    MPI_Aint stride = sizeof(Atome)/sizeof(double);
    MPI_Type_vector(1, blockLength, stride, MPI_DOUBLE, &object);
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
	
    //boucle principale (nb_iter == nb de points par courbe)
    for (int k = 0; k<=nb_iter; k++)
    {
        //calcul du dt local pour chacun des points, on garde le min
        double_tmp = 0;
        for(int n = 0; n < maxElem; n++)
        {
            double_tmp = calc_dt(initialDatas[n], dist_min[n]);
            if(dt > double_tmp || dt < 0)
            {
                dt = double_tmp;
            }
        }
        //calcul du dt global avec un MPI_Allreduce
        printf("\navant allreduce: %d, dt:%lf\n", myrank, dt);
        MPI_Allreduce(MPI_IN_PLACE, &dt, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
        printf("boucle proc: %d, dt:%lf\n", myrank, dt);
        for(int z = 0; z<maxElem; z++)
        {
            initialDatas[z].vit[0]=0;
            initialDatas[z].vit[1]=0;
            initialDatas[z].acc[0]=0;
            initialDatas[z].acc[1]=0;	
        }
		

        inputDatas = (k%2) == 0 ? &(buffer0[0]) : &(buffer1[0]); 
		
        //pour chaque processus
        for(int i = 0; i<size; i++)
        {
            //debut des communications non bloquantes
            MPI_Start(&(sendRequest[i%2]));
            MPI_Start(&(recvRequest[i%2]));
			
            //calcul des forces et influence sur les positions 
            if(i!=0)//cas general
            {
                for(int m = 0; m < maxElem; m++)
                {
                    for(int n = 0; n < maxElem; n++)
                    {
                        //MAJ des distances min
                        double_tmp = distance(initialDatas[m], inputDatas[n]);
                        if(dist_min[m]>double_tmp)
                        {
                            dist_min[m]=double_tmp;
                        }
						
                        // attention, chacune des fonctions
                        // suivantes ne calcule que l'influence
                        // de l'atome courant
                        double_tmp = force_inter(initialDatas[m], inputDatas[n]);
						
                        double_tmp_ptr[0] = double_tmp * cos(distance(initialDatas[m], inputDatas[n]));
                        double_tmp_ptr[1] = double_tmp * sin(distance(initialDatas[m], inputDatas[n]));
                        acceleration(&(initialDatas[m]), double_tmp_ptr);
                        if(k!=0)
                        {
                            vitesse(&(initialDatas[m]), dt);
                        }
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
                            //MAJ des distances min
                            double_tmp = distance(initialDatas[m], inputDatas[n]);
                            if(dist_min[m]>double_tmp)
                            {
                                dist_min[m]=double_tmp;
                            }
							
                            // attention, chacune des fonctions
                            // suivantes ne calcule que l'influence 
                            // de l'atome actuel
                            double_tmp = force_inter(initialDatas[m], inputDatas[n]);
						
                            double_tmp_ptr[0] = double_tmp * cos(distance(initialDatas[m], inputDatas[n]));
                            double_tmp_ptr[1] = double_tmp * sin(distance(initialDatas[m], inputDatas[n]));
						
                            //~ printf("dist:%lf, force_inter:%lf\n", distance(initialDatas[m], inputDatas[n]), force_inter(initialDatas[m], inputDatas[n]));
						
                            acceleration(&(initialDatas[m]), double_tmp_ptr);
                            if(k!=0)
                            {
                                vitesse(&(initialDatas[m]), dt);
                            }
                        }
                        //~ else
                        //~ {
                        //~ dist_min[m]=DBL_MAX;
                        //~ }
                    }
                }			
            }
			
            //attente de la reception des donnees avant l etape suivante
            MPI_Wait(&(recvRequest[i%2]),MPI_STATUS_IGNORE);
			
        }
        if(k!=0)
        {
            for(int z = 0; z<maxElem; z++)
            {
                new_pos(&(initialDatas[z]), dt);	
            }
            memcpy(buffer0, initialDatas, maxElem * sizeof(Atome));
        }
				
        // ecriture du resultat
        if(k!=0)
        {
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


