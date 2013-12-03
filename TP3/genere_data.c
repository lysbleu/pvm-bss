#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

char zero[100] =  "0.0000E00";
char masse[100] = "1.0000E10";
char abs[100] ="";
char ord[100]="";
char command[1000] = "";
int max = atoi(argv[1]);
for (int i=0; i<max; i++)
{
	if(i<10)
	{
		sprintf(abs, "1.0000E0%d", i);
	}
	else
	{
		sprintf(abs, "1.0000E%d", i);
	}
	for (int j=0; j<max; j++)
	{
		if(j<10)
		{
			sprintf(ord, "1.0000E0%d", j);
		}
		else
		{
			sprintf(ord, "1.0000E%d", j);
		}
				
		if(i==0 && j==0)//ecrasement si fichier existe deja
		{
			sprintf(command, "echo %s %s %s %s %s> data/data_%d.txt", masse, abs, ord , zero, zero,max);	
			system(command);
		}
		else//concatenation des donnees
		{
			sprintf(command, "echo %s %s %s %s %s>> data/data_%d.txt", masse, abs, ord , zero, zero,max);	
			system(command);	
		}
	}
}
		
return EXIT_SUCCESS;
}
