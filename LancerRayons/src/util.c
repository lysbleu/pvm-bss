#include <mpi.h>
#include "primes.c"
int nb_carreaux(const int i, const int j, const int taille)
{
	return ((i+taille-1)/taille)*((j+taille-1)/taille);
}


int choix_N(const int nb_procs, const int nb_tuiles)
{
	int i;
	for(i=0; i<PRIMESTABLE_SIZE; i++)
	{
		if(primes[i]>nb_procs && gcd(primes[i],nb_tuiles)==1)
		{
			return primes[i];
		}
	}
	return -1;
}

int stocke_carreaux(int ** tab_carreaux, const int i, const int j, const int taille, int *myrank)
{
	int size,N,k;
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,myrank);
	
	int nb_tuiles = nb_carreaux(i,j, taille);
	int my_nb_tuiles = nb_tuiles/size+(((*myrank)<(nb_tuiles%size))?1:0);
	int nb_tuiles_precedentes = (((*myrank)<(nb_tuiles%size))?(*myrank):(nb_tuiles%size)) + (*myrank)*(nb_tuiles/size);
	//~ printf("proc %d : nb_tuiles prec:%d nb_tuiles:%d nb % reste:%d\n", (*myrank), nb_tuiles_precedentes,my_nb_tuiles,nb_tuiles%size); 
		
	*tab_carreaux = calloc(my_nb_tuiles, sizeof(int));
	N = choix_N(size, nb_tuiles);
	
	for(k=0; k<my_nb_tuiles; k++)
	{
		(*tab_carreaux)[k]=((k+nb_tuiles_precedentes)*N)%nb_tuiles;
	}
	
	return my_nb_tuiles;
} 

void reduce_in_place(float *TabColor, int count, int root)
{
	int myrank; 
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	if(myrank == root)
	{
		MPI_Reduce(MPI_IN_PLACE, TabColor, count, MPI_FLOAT, MPI_SUM, root, MPI_COMM_WORLD);
	}else
	{
		MPI_Reduce(TabColor, NULL, count, MPI_FLOAT, MPI_SUM, root, MPI_COMM_WORLD);
	}
}

void liberation(int* tab_carreaux)
{
	free(tab_carreaux);
	MPI_Finalize();
}
