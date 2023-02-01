/*
 *  String.c
 *  isentlib
 *
 *  Created by Audrey SIEPRAWSKI on 18/03/2015.
*
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "String.h"


int
indexOfCharInTrame(char* i_trame, const char i_car)
{
	int index = -1;
	int i = 0;

	for (i=0 ; i < strlen(i_trame) ; i++)
	{
		if (i_trame[i] == i_car)
		{
			index = i;
			break;
		}
	}

	return index;
}

void extractBufferToTrame(char* trameIn, int i_indexDebut, int i_nbCar, char* trameOut)
{
	//dimensionnement de la chaine destination
    size_t lengthTrameIn = strlen(trameIn);
    size_t lgDest = (i_nbCar <= (lengthTrameIn - i_indexDebut)) ? i_nbCar : (lengthTrameIn - i_indexDebut);

	if (lgDest>0)
		memcpy( trameOut, (trameIn + i_indexDebut), lgDest);
		  
	return;
}




