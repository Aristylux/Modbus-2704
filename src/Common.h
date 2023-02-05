/*
 *  CommonLib.h
 *  CommonLib
 *
 *  Created by SIEPRAWSKI Audrey on 18/03/2015.
 *
 */

#ifndef COMMONLIB_H
#define COMMONLIB_H

// eumeration des codes d'erreurs possibles sur transmission
typedef enum errorcomm
{
	ERRORCOMM_NOERROR = 0,
	ERRORCOMM_ERROR,
	ERRORCOMM_TIMEOUT,
	ERRORCOMM_BCC,
	ERRORCOMM_NAK,
	ERRORCOMM_EOT,
	ERRORCOMM_READ,
	ERRORCOMM_WRITE,
	ERRORCOMM_EVENT,
	ERRORCOMM_IOPENDING,
	ERRORCOMM_WAITOBJ, // erreur sur WaitForSingleObject
	ERRORCOMM_INCONNU, // Commande inconnue
	ERRORCOMM_INTERRUPT
} ErrorComm;

int checksum(char *s);
void decodeTrameNMEA(char *i_trameReceive);
void extractGPSParameters(char *buffer);

#endif