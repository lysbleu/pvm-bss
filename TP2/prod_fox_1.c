#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

void affiche_matrice(int *A, int n)
{
	int i,j;
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			printf("%3d  ", A[j+i*n]);
		}
		printf("\n");
	}
}

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
	int coords[2], tmp_coords[2];
	int tmp_rank;
	int rank_ligne, rank_colonne, size_colonne, size_ligne;
	MPI_Dims_create(nb_blocs, 2, dims);
	
	int periods[2] = {1,1};
	
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods,1, &world); 
	MPI_Comm_rank( world, &myrank ); 
	MPI_Comm_size( world, &size);
	
	int * C = calloc(taille_bloc*taille_bloc, sizeof(int));
	int * A = calloc(taille_bloc*taille_bloc, sizeof(int));
	int * B = calloc(taille_bloc*taille_bloc, sizeof(int));
	
	int * A_bis = calloc(taille_bloc*taille_bloc, sizeof(int));

	int *sendcnts, *displs;

	MPI_Datatype bloc;
	
	MPI_Type_vector(taille_bloc, taille_bloc, n, MPI_INT, &bloc);
	
	MPI_Type_create_resized(bloc,0,sizeof(int),&bloc);	
	
	MPI_Type_commit(&bloc); 


	int *A_final, *B_final, *C_final;
	
	if(myrank == 0)
	{
		printf("n: %d\nnb: %d\ntaille:%d\n",n,nb_blocs_ligne,taille_bloc);

		A_final = calloc(n*n ,sizeof(int));
		B_final = calloc(n*n ,sizeof(int));
		C_final = calloc(n*n ,sizeof(int));
		
		for (i = 0; i<n; i++)
		{
			for (j=0; j<n;j++)
			{
				A_final[i+j*n]=rand()%20;
				B_final[i+j*n]=rand()%20;
				
				if(i<taille_bloc && j<taille_bloc)
				{
					A[i+j*n] = A_final[i+j*n];
				}
			}
		}
		
	
	sendcnts = calloc(nb_blocs, sizeof(int));
	displs = calloc(nb_blocs, sizeof(int));
	
	for (j = 0; j < nb_blocs_ligne; j++)
	{
		for (i= 0; i < nb_blocs_ligne; i++)
		{
			sendcnts[i+j*nb_blocs_ligne]=1;
			displs[i+j*nb_blocs_ligne]=(j*n+i)*taille_bloc;
		}
	}
		printf("A:\n");
		affiche_matrice(A_final, n);
		printf("\n\n");
		printf("B:\n");
		affiche_matrice(B_final,taille_bloc);
		printf("\n\n");
	}
		
	//~ MPI_Scatter(A_final,1, bloc, A, taille_bloc*taille_bloc, MPI_INT, 0, world);
	MPI_Scatterv(A_final, sendcnts, displs,bloc, A, taille_bloc*taille_bloc, MPI_INT, 0, world);
	
	
	//initialisation
	MPI_Cart_coords(world, myrank, 2, coords);
	
		if(myrank == 1){
			printf("coords i : %d, j : %d\n", coords[0], coords[1]);
			affiche_matrice(A,taille_bloc);
	printf("\n\n");
	}
	
	MPI_Comm ligne;
	MPI_Comm_split(world, coords[0], 0, &ligne);
	
	//inutile
	//~ MPI_Comm colonne;
	//~ MPI_Comm_split(world, coords[1], 0, &colonne);

	MPI_Comm_rank( ligne, &rank_ligne);
	//inutile
	//~ MPI_Comm_rank( colonne, &rank_colonne ); 
	
	MPI_Comm_size( ligne, &size_ligne);
	//inutile
	//~ MPI_Comm_size( colonne, &size_colonne ); 
	
	for (k = 0; k<n ;k++)
	{
		tmp_coords[0] = k;
		tmp_coords[1] = k;
		//copie pour permettre le broadcast
		if(coords[1] == k && coords[1] == k)
		{
			for (i = 0; i< taille_bloc*taille_bloc; i++)
			{
				A_bis[i] = A[i];
			}
		}		
		
		//broadcast sur la ligne
		MPI_Bcast(A_bis, taille_bloc*taille_bloc, MPI_INT, MPI_Cart_rank(world, tmp_coords, &tmp_rank), ligne);

		produit_matriciel(C,A_bis, B, taille_bloc);	

		MPI_Cart_shift(world, 1, 1, &rank_source, &rank_dest);
		MPI_Sendrecv_replace(B, taille_bloc*taille_bloc, MPI_INT,rank_dest, 1, rank_source, MPI_ANY_TAG, world, &status);
	}
	
	MPI_Gatherv(C, taille_bloc*taille_bloc, MPI_INT, C_final, sendcnts, displs, bloc, 0, world);

	MPI_Type_free(&bloc);
	MPI_Finalize();
	
	free(A);
	free(B);
	free(C);
	free(A_bis);
	
	if (myrank == 0)
	{
		printf("C:\n");
		affiche_matrice(C_final, n);
		free(A_final);
		free(B_final);
		free(C_final);	
	}
  return EXIT_SUCCESS;
} 
