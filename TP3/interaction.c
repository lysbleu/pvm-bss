#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include "atom.h"
#include "parser.h"

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
    // parsing
    int elementsNumber; // argv[?] ?

    Atome *initialDatas;

    int myrank, size, maxElem;
    MPI_Comm_size( MPI_COMM_WORLD, &size);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	
	elementsNumber = parse(filename, &initialDatas, myrank, size, &maxElem);
    MPI_Comm_size( MPI_COMM_WORLD, &size); //cas ou plus de procs que de lignes dans le fichier
	
	Atome *buffer0 = calloc(maxElem, sizeof(Atome));
    Atome *buffer1 = calloc(maxElem, sizeof(Atome));
	
    int blockLength = 3;
    MPI_Datatype object;
    MPI_Aint stride = sizeof(Atome);
    MPI_Type_vector(3, blockLength, stride, MPI_DOUBLE, &object);
    MPI_Type_commit(&object); 
    
    MPI_Request oddSend;
    MPI_Request oddReceive;
    MPI_Request evenSend;
    MPI_Request evenReceive;
    
    MPI_Send_init(buffer0, maxElem, object, (myrank + 1) % size, 2,
				MPI_COMM_WORLD, &evenSend);
    MPI_Send_init(buffer1, maxElem, object, (myrank + 1) % size, 1,
				MPI_COMM_WORLD, &oddSend);
    MPI_Recv_init(buffer1, maxElem, object, (myrank + 1) % size, 2,
				MPI_COMM_WORLD, &evenReceive);
    MPI_Recv_init(buffer0, maxElem, object, (myrank + 1) % size, 1,
				MPI_COMM_WORLD, &oddReceive);
        
    //initialisation du buffer local
    // TODO copie de initialsData dans buffer0
    for(int i = 0; i<=nb_iter; i++)
    {
        MPI_Start(&evenSend);
        MPI_Start(&evenReceive);
		//TODO calcul
        MPI_Wait(&evenReceive,MPI_STATUS_IGNORE);

        MPI_Start(&oddSend);
        MPI_Start(&oddReceive);
        // TODO calcul
        MPI_Wait(&oddReceive,MPI_STATUS_IGNORE);
        
    }
    
    MPI_Finalize();
    free(buffer0);
    free(buffer1);
    //~ free(massesBuffer);
    free(initialDatas);

    return EXIT_SUCCESS;
} 


