/*
 *  SerialPortLib.h
 *  isentlib
 *
 *  Created by AudreySIEPRAWSKI on 21/03/15.
 *
 */


#ifndef SERIALPRTLIB_H
#define SERIALPRTLIB_H

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <windows.h>

#include "Common.h"

#define MAX_CAR_TRAME 499
#define TIMEOUT_SERIAL_PORT 3000
#define RS232_PORTNB    (10)


static int Cport[RS232_PORTNB],
    error;

static const char* comports[RS232_PORTNB]={"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4","/dev/ttyS5",
                                    "/dev/ttyS6","/dev/ttyS7","/dev/ttyS8","/dev/ttyS9"};


// Entrees et Sorties
typedef union _IsenDCB 
{
    int		_idCom;

	// Vitesse de transmission
	unsigned long		_baudRate;
    // number of bits/byte, 4-8 
	unsigned char		_byteSize;  
    // 0-4=no,odd,even,mark,space 
	unsigned char		_parity;               
    // 0,1,2 = 1, 1.5, 2 
	unsigned char		_stopBits;   
} IsenDCB;


// Cree une nouvelle connexion sur le port serie
HANDLE createSerialPort(int i_numPort);

// Essaie de brancher une connexion sur une machine dont l'adresse et le port de communication sont donnes
BOOL setParamSerialPort(HANDLE hPort, int i_baudRate, BYTE i_byteSize, BYTE i_parity, BYTE i_stopBits);


// Envoie sur une connexion le flux de donnees fourni
// Le nombre d'octets ecrits est renvoye
ErrorComm sendSerialPort( HANDLE hPort, int timeOut, const char *i_pDdonnees, int i_iLongueurDonnees);

// Receptionne a partir d'une connexion un flux de donnees dans une zone de memoire passee en parametre
// Le nombre d'octets lus est renvoye
ErrorComm recvSerialPort( HANDLE hPort, int timeOut, char *i_pDonnees, int *i_iLongueurDonnees);

ErrorComm sendAndReceiveSerialPort(HANDLE hPort, int i_iTimeOut, char* i_sTrameIn, int i_lengthIn, char* i_sTrameOut, int* i_lengthOut);

// Termine la connexion
void terminateSerialPort(HANDLE hPort);


#endif
