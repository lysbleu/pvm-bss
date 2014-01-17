#include "type_mpi.h"
#include "util.c"
#include "mpi.h"
#include <string.h>
#include "unistd.h"

int main(int argc, char ** argv)
{
    MPI_Init(NULL, NULL);
    int myrank, nb_procs;
    MPI_Comm_size( MPI_COMM_WORLD, &nb_procs);
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank ); 
    
    blas_t * matrice;
    blas_t * recv_mat;
    int i,j;
    
    
    int m = 4 * nb_procs;
    int n = m; 
    int block_nb_col = 2; //20;
    int block_size = block_nb_col * m;
    int nb_blocks = n /block_nb_col;
    int nb_blocks_by_proc = nb_blocks/nb_procs;
    
    alloc_matrice(&matrice, m, n);
    alloc_matrice(&recv_mat, m, n / nb_procs);
    
    memset(recv_mat, 0, (m * n / nb_procs) * sizeof(blas_t));
    
    MPI_Datatype colBlock;
    MPI_Type_vector(block_nb_col, m , m, MPI_DOUBLE, &colBlock);
    MPI_Type_commit(&colBlock);
	
	if(myrank == 0)
	{
		for(i=0; i < nb_blocks; i++)
		{
			if((i/nb_procs)%2 == 0)
			{
				if(i%nb_procs!=0)
				{
					MPI_Send(&(matrice[i*block_size]), 1, colBlock, i%nb_procs, i%nb_procs, MPI_COMM_WORLD);
				}
				else
				{
					memcpy(&(recv_mat[(i/nb_procs)*block_size]), &(matrice[i*block_size]), block_size*sizeof(double));
				}
			}
			else
			{
				if(nb_procs-(i%nb_procs)-1!=0)
				{
					MPI_Send(&(matrice[i*block_size]), 1, colBlock, nb_procs-(i%nb_procs)-1, nb_procs-(i%nb_procs)-1, MPI_COMM_WORLD);
				}
				else
				{
					memcpy(&(recv_mat[(i/nb_procs)*block_size]), &(matrice[i*block_size]), block_size*sizeof(double));
				}
			}
		}	
	}
    else
    {
		for(j = 0; j < nb_blocks_by_proc; j++)
		{
			MPI_Recv(&(recv_mat[j * block_size]), 1 , colBlock, 0, myrank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
 
	if(myrank == 0)
	{
		printf("myrank: %d\n", myrank);
		affiche(m, n, matrice, m, stdout);
	}

	sleep(myrank);
	printf("myrank: %d\n", myrank);
	affiche(m, n / nb_procs, recv_mat, m, stdout);
	
	MPI_Finalize();
	destruction(matrice);
	destruction(recv_mat);
	return 0;
}
