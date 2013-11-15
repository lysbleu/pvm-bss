#include "../parser.h"

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv)
{
	Atome* at;
	int taille = parse("../data/data_test.txt", &at, 0, 5);
	displayAtoms(at, taille);
	free(at);
	printf("\n");
	taille = parse("../data/data_test.txt", &at, 1, 5);
	displayAtoms(at, taille);
	free(at);
	printf("\n");
	taille = parse("../data/data_test.txt", &at, 2, 5);
	displayAtoms(at, taille);
	free(at);
	printf("\n");
	taille = parse("../data/data_test.txt", &at, 3, 5);
	displayAtoms(at, taille);
	free(at);
	printf("\n");
	taille = parse("../data/data_test.txt", &at, 4, 5);
	displayAtoms(at, taille);
	free(at);
	

	
	return EXIT_SUCCESS;
}
