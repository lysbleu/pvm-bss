#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
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
    MPI_Status status;
    int nb_iter = atoi(argv[2]);
    int mode = atoi(argv[3]);
    char *filename = calloc(strlen(argv[1])+1, sizeof(char));
    strcpy(filename, argv[1]);

    Atome *initialDatas;

    int myrank, size, maxElem;
    MPI_Comm_size( MPI_COMM_WORLD, &size);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	
    int elementsNumber = parse(filename, &initialDatas, myrank, size, &maxElem);

    Atome *buffer0 = calloc(maxElem, sizeof(Atome));
    Atome *buffer1 = calloc(maxElem, sizeof(Atome));
	
    int blockLength = 3;
    MPI_Datatype object;
    MPI_Aint stride = sizeof(Atome)/sizeof(double);
    MPI_Type_vector(3, blockLength, stride, MPI_DOUBLE, &object);
    MPI_Type_commit(&object); 
    
    MPI_Request sendRequest[2];
    MPI_Request recvRequest[2];

    //~ sleep(myrank);
    //~ printf("proc:%d, src:%d dest:%d maxElem:%d, blocklength:%d, stride:%u\n", myrank, (myrank +size - 1) % size, (myrank + 1) % size, maxElem, blockLength, stride);
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
	for (int k = 0; k<nb_iter; k++)
	{
		for(int i = 0; i<size; i++)
		{
			MPI_Start(&(sendRequest[i%2]));
			//~ if(i ==0 && k==0)
			  //~ MPI_Barrier(MPI_COMM_WORLD);
			MPI_Start(&(recvRequest[i%2]));
			
			if(i!=0)//cas general
			{
				//TODO calcul
			}
			else//cas particulier, ne pas prendre en compte l influence sur soi meme
			{
				//TODO calcul
			}
			MPI_Wait(&(recvRequest[i%2]),MPI_STATUS_IGNORE);
			//~ printf("Proc:%d Status:%d\n", myrank, status);
			//MPI_Wait(&(sendRequest[i%2]),MPI_STATUS_IGNORE);
		}
		  //TODO calcul de fin
		//ecriture du resultat
		for (int j=0; j<elementsNumber; j++)
		{
			if(j==0)
			{
				sprintf(command, "echo %lf %lf > results/res_%d_%d.txt", initialDatas[j].pos[0],initialDatas[j].pos[1], myrank, j);	
				//~ printf("Commande: %s\n", command);
				system(command);
			}
			else
			{
				sprintf(command, "echo %lf %lf >> results/res_%d_%d.txt", initialDatas[j].pos[0],initialDatas[j].pos[1], myrank, j);	
				//~ printf("Commande: %s\n", command);
				system(command);	
			}
		}
    }
    //~ printf("Fin programme\n");
    MPI_Request_free(&(sendRequest[0]));
    MPI_Request_free(&(sendRequest[1]));
    MPI_Request_free(&(recvRequest[0]));
    MPI_Request_free(&(recvRequest[1]));
    MPI_Type_free(&object);
    free(buffer0);
    free(buffer1);
    free(initialDatas);

    MPI_Finalize();
    return EXIT_SUCCESS;
} 


