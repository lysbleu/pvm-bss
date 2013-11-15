#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include "atom.h"
#include "parser.h"

int main( int argc, char **argv ) {
    if(argc != 3)
    {
        printf("usage: ./interaction nom_de_fichier mode\n");
        return EXIT_FAILURE;
    }

    MPI_Init( NULL, NULL ); 
    
    int mode = atoi(argv[2]);
    char *filename = calloc(strlen(argv[1])+1, sizeof(char));
    // parsing
    int elementsNumber; // argv[?] ?

    Atome *initialDatas;

    int myrank, size;
    MPI_Comm_size( MPI_COMM_WORLD, &size);

    MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
    MPI_Comm_size( MPI_COMM_WORLD, &size);
	
	elementsNumber = parse(filename, &initialDatas, myrank, size);
	
	Atome *buffer0 = calloc(elementsNumber, sizeof(Atome));
    Atome *buffer1 = calloc(elementsNumber, sizeof(Atome));
	
    int blockLength = 3;
    MPI_Datatype object;
    MPI_Aint stride = sizeof(Atome);
    MPI_Type_vector(3, blockLength, stride, MPI_DOUBLE, &object);
    MPI_Type_commit(&object); 
    
    MPI_Request oddSend;
    MPI_Request oddReceive;
    MPI_Request evenSend;
    MPI_Request evenReceive;
    
    MPI_Send_init(buffer0, elementsNumber, object, (myrank + 1) % size, 2,
				MPI_COMM_WORLD, &evenSend);
    MPI_Send_init(buffer1, elementsNumber, object, (myrank + 1) % size, 1,
				MPI_COMM_WORLD, &oddSend);
    MPI_Recv_init(buffer1, elementsNumber, object, (myrank + 1) % size, 2,
				MPI_COMM_WORLD, &evenReceive);
    MPI_Recv_init(buffer0, elementsNumber, object, (myrank + 1) % size, 1,
				MPI_COMM_WORLD, &oddReceive);
        
    
    // TODO
    while(42) {
        MPI_Start(&evenSend);
        MPI_Start(&evenReceive);
        // TODO
        MPI_Wait(&evenReceive,MPI_STATUS_IGNORE);

        MPI_Start(&oddSend);
        MPI_Start(&oddReceive);
        // TODO
        MPI_Wait(&oddReceive,MPI_STATUS_IGNORE);
        
    }
    
    MPI_Finalize();
    free(buffer0);
    free(buffer1);
    //~ free(massesBuffer);
    free(initialDatas);

    return EXIT_SUCCESS;
} 


