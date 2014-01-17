#include "type_mpi.h"
#include "util.c"
#include "mpi.h"
#include <string.h>

int main(int argc, char ** argv)
{
    MPI_Init(NULL, NULL);
    int myrank, nb_procs;
    MPI_Comm_size( MPI_COMM_WORLD, &nb_procs);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
    
    blas_t * matrice;
    blas_t * recv_mat;
    
    
    int m = 4 * nb_procs;
    int n = m; 
    int block_nb_col = 2; //20;
    int block_size = block_nb_col * m;
    int nb_blocks = n /block_nb_col;
    
    alloc_matrice(&matrice, m, n);
    alloc_matrice(&recv_mat, m, n / nb_procs);
    
    memset(recv_mat, 0, (m * n / nb_procs) * sizeof(blas_t));
    
    MPI_Datatype object;
    MPI_Aint stride = m * nb_procs * block_nb_col;
    MPI_Type_vector( block_nb_col, m , m, MPI_DOUBLE, &object);
    MPI_Type_commit(&object);
	
	MPI_Status status;
	
	int indice_debut;
	if(myrank == 0)
	{
		for(int i = 1; i < nb_procs; i++)
		{
			for(int j = 0; j < nb_blocks ; j++)
			{
				indice_debut = (i + j * nb_procs) * (block_nb_col * m) ;
				indice_debut += ((j%2) * block_nb_col * (nb_procs - (i+1))); // inversion pour serpentin
				printf("indice début: %d\n", indice_debut);
				MPI_Send(&(matrice[indice_debut]), 1, object, i, i, MPI_COMM_WORLD);
			}
		}		
	}
    else
    {
		for(int j = 0; j < nb_blocks; j++)
		{
			indice_debut = j * block_nb_col * m;
			MPI_Recv(&recv_mat[indice_debut],block_nb_col * m , MPI_DOUBLE, 0, myrank, MPI_COMM_WORLD, &status);
		}
	}
 
	if(myrank == 0)
	{
		printf("myrank: %d\n", myrank);
		affiche(m, n, matrice, m, stdout);
	}
	else
	{
		printf("myrank: %d\n", myrank);
		affiche(m, n / nb_procs, matrice, m, stdout);
	}
	
	MPI_Finalize();
}
