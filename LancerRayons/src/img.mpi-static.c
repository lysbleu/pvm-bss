/***************************************************************************
 *                       Lanceur de rayons simplifie                       *
 ***************************************************************************
 * ENSERB, 3me annee Informatique                               1995-1996  *
 * Option AAMP, Module MP2                                                 *
 *                                                                         *
 ***************************************************************************
 * Author : Gilles Subrenat                                                *
 * Date   : 15 janvier 1996                                                *
 *                                                                         *
 * $Id$
 ***************************************************************************/

#include <stdio.h>
#include <string.h>

#include "img.h"

#include "macro.h"
#include "const.h"
#include "type.h"
#include "type_spec.h"

#include "lanceur.h"
#include "scn.h"
#include "bnd.h"
#include "cmr.h"

#include "util.h"
//~ #include <mpi.h>

typedef struct {
  COUPLE  Pixel;
} IMG_BASIC;

static IMG_BASIC  Img;


BOOL
file_img (FILE *File)
{
	GET_COUPLE (Img.Pixel);
	return (TRUE);
}


static COLOR
pixel_basic (INDEX i, INDEX j)
{
	RAY    Ray;
	HIT    Hit;
	REAL   u, v;

	Ray.Level = 1;
	Ray.Color = White;
	u = 2.0 * i / Img.Pixel.i - 1.0;
	v = 2.0 * j / Img.Pixel.j - 1.0;
	cmr (&Ray, u, v);

	if (! hit_bnd (&Ray, &Hit))
	return (MainScn->Atm);
	get_scn (&Ray, &Hit);

	return (Ray.Color);
}


void
img (const char *FileNameImg)
{
	FILE   *FileImg;
	COLOR  *TabColor, *Color;
	STRING Name;
	INDEX  i, j,k;
	BYTE   Byte;
	int *tab_carreaux;
	int nb_carreaux, myrank;
	int nb_carreaux_colonnes = (Img.Pixel.i+TAILLE-1)/TAILLE;

	strcpy (Name, FileNameImg);
	strcat (Name, ".ppm");
	INIT_FILE (FileImg, Name, "w");
	fprintf (FileImg, "P6\n%d %d\n255\n", Img.Pixel.i, Img.Pixel.j);
	INIT_MEM (TabColor, Img.Pixel.i*Img.Pixel.j, COLOR);

	nb_carreaux = stocke_carreaux(&tab_carreaux, Img.Pixel.i, Img.Pixel.j, TAILLE, &myrank);

	int I, J;
	for (k=0; k<nb_carreaux; k++)
	{
		I = tab_carreaux[k]%nb_carreaux_colonnes;
		J = tab_carreaux[k]/nb_carreaux_colonnes;
		for (j = J*TAILLE; j < MIN((J+1)*TAILLE, Img.Pixel.j) ; j++)
		{
			for (i = I*TAILLE; i < MIN((I+1)*TAILLE,Img.Pixel.i); i++)
			{
				TabColor [i*Img.Pixel.j+j] = pixel_basic (i, j);
			}
		}
	}
	
	reduce_in_place(TabColor, Img.Pixel.i*Img.Pixel.j*3, 0);
	
	if(myrank == 0)
	{		
		for (i = 0, Color = TabColor; i < Img.Pixel.i*Img.Pixel.j; i++, Color++) {
		  Byte = Color->r < 1.0 ? 255.0*Color->r : 255.0;
		  putc (Byte, FileImg);
		  Byte = Color->g < 1.0 ? 255.0*Color->g : 255.0;
		  putc (Byte, FileImg);
		  Byte = Color->b < 1.0 ? 255.0*Color->b : 255.0;
		  putc (Byte, FileImg);
		}
		fflush (FileImg);
	  EXIT_FILE (FileImg);
	}
	EXIT_MEM (TabColor);
	liberation(tab_carreaux);
}
