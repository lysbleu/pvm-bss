#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mpi.h>
#include "vol_tache.h"

extern int _index_tache_courante_, _nb_taches_;
extern int _recu_terminaison_;
extern int _envoi_terminaison_;

void traitement_tache(int temps)
{
	printf("Temps : %d\n", temps);
	sleep(temps);
}

void generation_aleatoire(int **taches)
{
	int randomData = open("/dev/urandom", O_RDONLY);	
	read(randomData, ((char*)&_nb_taches_), sizeof(int));
	_nb_taches_ = abs(_nb_taches_)%20+1;
	*taches=calloc(_nb_taches_, sizeof(int));
	read(randomData, ((char*)(*taches)), sizeof(int)*_nb_taches_);
	int j;
	for(j=0; j<_nb_taches_; j++)
	{
		(*taches)[j]=abs((*taches)[j])%20 +1;
	}
	
	close(randomData);
}

int main(int argc, char* argv[])
{
	int *taches;
	pthread_t thread_communication;
	
	generation_aleatoire(&taches);
	MPI_Init(NULL, NULL);

	_index_tache_courante_ = 0;
	pthread_create(&thread_communication, NULL, communication, &taches);
	while(!_recu_terminaison_ || !_envoi_terminaison_ || _index_tache_courante_<_nb_taches_)
	{		
		if(_index_tache_courante_<_nb_taches_)
		{
			_index_tache_courante_++;
			traitement_tache(taches[_index_tache_courante_-1]);
		}
	}
	pthread_join(thread_communication, NULL);
	free(taches);
	MPI_Finalize();
	return EXIT_SUCCESS;
}
