#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
<<<<<<< HEAD
#include "atom.h"
=======
#include <string.h>
#include "atom.h"
#include "parser.h"

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
>>>>>>> 8f7a38f2daea1e28de746239ac9b9cb237eb1fe3

int main( int argc, char **argv ) {
    if(argc != 4)
    {
        printf("usage: ./interaction nom_de_fichier nb_iter mode\n");
        return EXIT_FAILURE;
    }

    MPI_Init( NULL, NULL ); 
    
    int nb_iter = atoi(argv[2]);
    int mode = atoi(argv[3]);
    char *filename = calloc(strlen(argv[1])+1, sizeof(char));
    strcpy(filename, argv[1]);

    Atome *initialDatas;

    int myrank, size, maxElem;
    MPI_Comm_size( MPI_COMM_WORLD, &size);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	
	int elementsNumber = parse(filename, &initialDatas, myrank, size, &maxElem);

    MPI_Comm_size( MPI_COMM_WORLD, &size); //cas ou plus de procs que de lignes dans le fichier
	
	Atome *buffer0 = calloc(maxElem, sizeof(Atome));
    Atome *buffer1 = calloc(maxElem, sizeof(Atome));
	
    int blockLength = 3;
    MPI_Datatype object;
    MPI_Aint stride = sizeof(Atome);
    MPI_Type_vector(3, blockLength, stride, MPI_DOUBLE, &object);
    MPI_Type_commit(&object); 
    
    //~ MPI_Request oddSend;
    MPI_Request sendRequest[2];
    MPI_Request recvRequest[2];
    //~ MPI_Request oddReceive;
    //~ MPI_Request evenSend;
    //~ MPI_Request evenReceive;
    
    MPI_Send_init(buffer0, maxElem, object, (myrank + 1) % size, 2,
				MPI_COMM_WORLD, &(sendRequest[0]));
				//~ MPI_COMM_WORLD, &evenSend);
    MPI_Send_init(buffer1, maxElem, object, (myrank + 1) % size, 1,
				MPI_COMM_WORLD, &(sendRequest[1]));
				//~ MPI_COMM_WORLD, &oddSend);
    MPI_Recv_init(buffer1, maxElem, object, (myrank + 1) % size, 2,
				MPI_COMM_WORLD, &(recvRequest[0]));
				//~ MPI_COMM_WORLD, &evenReceive);
    MPI_Recv_init(buffer0, maxElem, object, (myrank + 1) % size, 1,
				MPI_COMM_WORLD, &(recvRequest[1]));
				//~ MPI_COMM_WORLD, &oddReceive);
    
    char command[100];

    //initialisation du buffer local
	for (int i = 0; i <maxElem; i++)
	{
		copyAtome(&(buffer0[i]), &(initialDatas[i]));
	}
	
    for(int i = 0; i<=nb_iter; i++)
    {
		MPI_Start(&(sendRequest[i%2]));
		MPI_Start(&(recvRequest[i%2]));
		//TODO calcul
        MPI_Wait(&(recvRequest[i%2]),MPI_STATUS_IGNORE);
		
  /*      MPI_Start(&evenSend);
        MPI_Start(&evenReceive);
		//TODO calcul
        MPI_Wait(&evenReceive,MPI_STATUS_IGNORE);
        * 
        MPI_Start(&oddSend);
        MPI_Start(&oddReceive);
        // TODO calcul
        MPI_Wait(&oddReceive,MPI_STATUS_IGNORE);
  */      
	//ecriture du resultat
	for (int j=0; j<elementsNumber; j++)
		{
			sprintf(command, "echo %lf %lf >> results/res_%d_%d.txt", initialDatas[i].pos[0],initialDatas[i].pos[1], myrank, j);	
			system(command);
		}
    }
    
    MPI_Finalize();
    free(buffer0);
    free(buffer1);
    //~ free(massesBuffer);
    free(initialDatas);

    return EXIT_SUCCESS;
} 


