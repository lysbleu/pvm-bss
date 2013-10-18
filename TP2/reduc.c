#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define NB_TOURS 10
int main( int argc, char **argv ) {
  int myrank, size, res, tmp;
  MPI_Status status; 
  MPI_Init( NULL, NULL ); 
  
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
  MPI_Comm_size( MPI_COMM_WORLD, &size);
  
	MPI_Comm  new_world = MPI_COMM_WORLD;

  res = myrank;
  while(size!=1)
  {
  MPI_Comm_size( new_world, &size);
  MPI_Comm_rank( new_world, &myrank ); 

		  if(myrank %2 == 1)
		  { 
			  MPI_Send(&myrank, 1 , MPI_INT, myrank-1, 99, new_world);
			  MPI_Comm_split(new_world, 1, 1, &new_world);

			  printf("Proc %d quitte : %d.\n", myrank, res);
			  MPI_Finalize();
			  exit(0);
		  }
		  else
		  {			  
			  if(myrank != size-1)
			  {
					MPI_Recv(&tmp, 1, MPI_INT, myrank+1, MPI_ANY_TAG, new_world, &status); 
					res += tmp;
	  			  printf("Proc %d a recu : %d\n", myrank, res);
				}
				
				MPI_Comm_split(new_world, 0, 1, &new_world);
			}
}
/* Merci a stephane pour cette participation dans le code	*/
  printf("Fin n %d : %d\n", myrank, res);
  MPI_Comm_split(new_world, 0, 1, &new_world);
  MPI_Finalize();

  return EXIT_SUCCESS;
} 

