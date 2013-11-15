#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main( int argc, char **argv ) {
	if(argc != 3)
	{
		printf("usage: ./interaction nom_de_fichier mode\n");
		return EXIT_FAILURE;
	}
	
	int mode = atoi(argv[2]);
	char * nom_fichier = calloc(strlen(argv[1])+1, sizeof(char));
	
	
	int myrank, size;
	MPI_Status status; 
	MPI_Init( NULL, NULL ); 
	MPI_Comm_size( MPI_COMM_WORLD, &size);

	MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
	MPI_Comm_size( MPI_COMM_WORLD, &size);
	
	//creation d un type pour decrire un bloc
	//~ MPI_Type_vector(taille_bloc, taille_bloc, n, MPI_INT, &bloc);
	//~ MPI_Type_create_resized(bloc,0,sizeof(int),&bloc);	//permet d utiliser Scatterv et Gatherv
	//~ MPI_Type_commit(&bloc); 

	
	
	MPI_Finalize();

  return EXIT_SUCCESS;
} 
