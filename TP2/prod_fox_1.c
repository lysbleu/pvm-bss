#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

void produit_matriciel(int* C, int* A, int* B, int n)
{
	int i, j, k;
	for(i=0; i<n;i++)
	{
		for (j=0; j<n; j++)
		{
			for(k=0; k<n; k++)
			{
				C[i+j*n] += A[i+k*n] * B[k + j*n]; 
			}
		}
	}
}

int main( int argc, char **argv ) {
	if(argc != 2)
	{
		printf("usage: ./prod_fox taille_matrice \n");
		return EXIT_FAILURE;
	}
	
	int myrank, size;
	MPI_Status status; 
	MPI_Init( NULL, NULL ); 
	MPI_Comm_size( MPI_COMM_WORLD, &size);

	int n = atoi(argv[1]);
	int nb_blocs = size;
	
	if(sqrt(size)<=0)
	{
		printf("nb_procs n est pas un carre\n");
		MPI_Finalize();
		return EXIT_FAILURE;
	}
	
	int nb_blocs_ligne = sqrt(nb_blocs);
	//~ printf("n: %d\nnb: %d\n",n,nb_blocs_ligne);
	
	if (n % nb_blocs_ligne != 0)
	{
		printf("taille matrice n est pas un multiple de de nb_procs\n");
		MPI_Finalize();
		return EXIT_FAILURE;	
	}

	int taille_bloc = n/nb_blocs_ligne;
	
	int i,j,k;
	int rank_source, rank_dest;
	MPI_Comm world;
	int dims[2] = {0,0};
	int coords[2];
	int rank_ligne, rank_colonne, size_colonne, size_ligne;
	MPI_Dims_create(nb_blocs, 2, dims);
	
	int periods[2] = {1,1};
	
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods,1, &world); 
	MPI_Comm_rank( world, &myrank ); 
	MPI_Comm_size( world, &size);
	
	int * C = calloc(taille_bloc, sizeof(int));
	int * A = calloc(taille_bloc, sizeof(int));
	int * B = calloc(taille_bloc, sizeof(int));
	
	int * A_bis = calloc(taille_bloc, sizeof(int));

	MPI_Datatype bloc;
	MPI_Type_vector(taille_bloc, taille_bloc, n, MPI_INT, &bloc);
	MPI_Type_commit(&bloc); 

	if(myrank == 0)
	{
		int* A_final = calloc(n*n ,sizeof(int));
		int* B_final = calloc(n*n ,sizeof(int));
		int* C_final = calloc(n*n ,sizeof(int));
		
		for (i = 0; i<n; i++)
		{
			for (j=0; j<n;j++)
			{
				A_final[i+j*n]=rand();
				B_final[i+j*n]=rand();
				
				if(i<taille_bloc && j<taille_bloc)
				{
					A[i+j*n] = A_final[i+j*n];
				}
			}
		}
		
		int coords_tmp[2];
		int rank_tmp;
		printf("debug 1 : %d\n", __LINE__);
		for (i = 0; i < nb_blocs_ligne; i++)
		{
			for (j = 0; j < nb_blocs_ligne;j++)
			{
				if(i!=0 || j!=0)
				{
					printf("debug inside : %d\ni:%d\nj:%d\n", __LINE__,i,j);
					coords[0]=i;
					coords[1]=j;
					MPI_Cart_rank(world, coords,&rank_tmp);
					printf("rank:%d\n",rank_tmp);
					printf("myrank:%d\n", myrank);
					MPI_Send(&A_final[i+j*n], 1 , bloc, rank_tmp , 1, world);
					printf("bla\n");
					MPI_Send(&B_final[i+j*n], 1 , bloc, rank_tmp , 1, world);
					}
			}
		}
	}
	else //myrank != 0
	{
		printf("rank en attente : %d\n", myrank);
		printf("size : %d\n", size);
		printf("taille bloc : %d\n", taille_bloc);

		MPI_Recv(A, taille_bloc*taille_bloc, MPI_INT, 0, MPI_ANY_TAG, world, &status); 
		printf("bla2\n");
		MPI_Recv(B, taille_bloc*taille_bloc, MPI_INT, 0, MPI_ANY_TAG, world, &status); 
	}
	
	printf("debug 2 : %d\n", __LINE__);

	//initialisation
	MPI_Cart_coords(world, myrank, 2, coords);
	
	MPI_Comm ligne;
	MPI_Comm_split(world, coords[0], 1, &ligne);
	
	MPI_Comm colonne;
	MPI_Comm_split(world, coords[1], 1, &colonne);

	MPI_Comm_rank( ligne, &rank_ligne);
	MPI_Comm_rank( colonne, &rank_colonne ); 
	
	MPI_Comm_size( ligne, &size_ligne);
	MPI_Comm_size( colonne, &size_colonne ); 
	
	printf("debug 3 : %d\n", __LINE__);

	for (k = 0; k<n ;k++)
	{
		if(coords[1] == k)
		{
			MPI_Bcast(A, taille_bloc*taille_bloc, MPI_INT, rank_ligne, ligne);
		}		
		MPI_Recv(A_bis, taille_bloc*taille_bloc, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, ligne, &status); 
		
		produit_matriciel(C,A_bis, B, taille_bloc);
		
		MPI_Cart_shift(world, 1, 1, &rank_source, &rank_dest);
		MPI_Sendrecv_replace(B, 1, bloc,rank_dest, 1, rank_source, MPI_ANY_TAG, world, &status);
	}
	printf("debug 4 : %d\n", __LINE__);

	//~ MPI_Send(&myrank, 1 , MPI_INT, myrank-1, 99, new_world);
	//~ MPI_Comm_split(new_world, 1, 1, &new_world);

	//~ printf("Fin n %d : %d\n", myrank, res);
	//~ MPI_Comm_split(new_world, 0, 1, &new_world);

	MPI_Finalize();

  return EXIT_SUCCESS;
} 

