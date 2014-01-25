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

int stocke_carreaux(int ** tab_carreaux, const int i, const int j, const int taille)
{
	int size,N,k;
	int myrank;
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	
	int nb_tuiles = nb_carreaux(i,j, taille);
	int my_nb_tuiles = (nb_tuiles-myrank)/size+((myrank<(nb_tuiles%size))?1:0);
	int nb_tuiles_precedentes = ((myrank<(nb_tuiles%size))?myrank:(nb_tuiles%size)) + myrank*(nb_tuiles-1)/size;
	printf("proc %d : my_nb_tuiles prec %d\n", myrank, nb_tuiles_precedentes); 
		
	*tab_carreaux = calloc(my_nb_tuiles, sizeof(int));
	N = choix_N(size, nb_tuiles);
	
	for(k=0; k<my_nb_tuiles; k++)
	{
		(*tab_carreaux)[k]=((k+nb_tuiles_precedentes)*N)%nb_tuiles;
	}
	
	return my_nb_tuiles;
} 
