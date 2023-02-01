/*
 *  StringLib.h
 *  isentlib
 *
 *  Created by SIEPRAWSKI Audrey on 18/03/2015.
 *
 */

#ifndef STRINGLIB_H
#define STRINGLIB_H

// Renvoie la position du caractere "i_car" dans la trame "i_trame"
int indexOfCharInTrame(char* i_trame, const char i_car);

// Extraire les i_nbCar caracteres a partir de i_indexDebut de la chaine source trameIn dans la chaine destination trameOut
void extractBufferToTrame(char* trameIn, int i_indexDebut, int i_nbCar, char* trameOut);


#endif
