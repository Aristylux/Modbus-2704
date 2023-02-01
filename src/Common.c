// commonTP.cpp
//
#include <stdio.h>
#include <string.h>

#include "Common.h"
#include "String.h"

const char Ip_CR	 = (char)(13);
const char Ip_LF	 = (char)(10);
const char Ip_CtrlQ	 = (char)(17);
const char Ip_Dollar = (char)(36);


// Calcul le "checksum" a partir des donnees de la trame
int checksum(char *s)
{
    int c = 0;
 
    while(*s)
        c ^= *s++;
 
    return c;
}

void decodeTrameNMEA(char* i_trameReceive)
{
    // A COMPLETER

	return;
}
