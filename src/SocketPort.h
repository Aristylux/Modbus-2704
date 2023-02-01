/* Veuillez reporter tout commentaire a ghislain.oudinet@isen.fr */

/*
 *  SocketLib.h
 *  isentlib
 *
 *  Created by Ghislain Oudinet on 27/09/07.
 *
 */

// Sous Windows : linker avec -lwsock32

#ifndef SOCKETLIB_H
#define SOCKETLIB_H

#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#else
#include <winsock.h>
#endif

#define PROTOCOLE_UDP	IPPROTO_UDP  //17
#define PROTOCOLE_TCP	IPPROTO_TCP  //6

#define SOCKET_SELECTALL		0
#define SOCKET_SELECTREAD		1
#define SOCKET_SELECTWRITE		2

#include "Common.h"

// Cree une nouvelle connexion qui pourra ensuite etre branchee sur une adresse et un port particulier
SOCKET createSocket(int i_dSizeBuffer /* = 512 */, int i_dProtocole /* = PROTOCOLE_TCP */, int i_dTimeoutsend /*= -1*/, int i_dTimeoutrecv /*= -1*/);

// Essaie de brancher une connexion sur une machine dont l'adresse et le port de communication sont donnes
BOOL connectSocket(SOCKET i_dDescripteurConnexion, const char *i_sAdresseIP, short i_dPort);

// Attend qu'une connexion tierce soit etablie sur la machine presente et sur le port donne en parametre
BOOL bindSocket(SOCKET i_dDescripteurConnexion, short port);
SOCKET acceptSocket(SOCKET i_dDescripteurConnexion, short i_dPort, short i_dLqueue);

// Envoie sur une connexion le flux de donnees fourni (attention a l'endianness des donnees)
// Le nombre d'octets ecrits est renvoye
int sendSocket( SOCKET i_dDescripteurConnexion, const char *i_pDdonnees, int i_iLongueurDonnees);

int sendToSocket( SOCKET i_dDescripteurConnexion, const char *i_pDdonnees, int i_iLongueurDonnees, const char *i_sAdresseIP, short i_dPort);

// Receptionne a partir d'une connexion un flux de donnees dans une zone de memoire passee en parametre
// Le nombre d'octets lus est renvoye
int recvSocket( SOCKET i_dDescripteurConnexion,char *i_pDonnees, int i_iLongueurDonnees);

int recvFromSocket( SOCKET i_dDescripteurConnexion, char *i_pDonnees, int i_iLongueurDonnees);

ErrorComm sendAndReceiveSocket(SOCKET i_dDescripteurConnexion, int i_iTimeOut, char* i_sTrameIn, int i_lengthIn, char* i_sTrameOut, int* i_lengthOut);

BOOL selectSocket (SOCKET i_dDescripteurConnexion, int i_iTimeInMiliSec, int i_iFlags);

// Termine la connexion
void terminateSocket(SOCKET i_dDescripteurConnexion);

#ifdef WIN32
// A appeler sous Windows avant toute utilisation des sockets  =>  initialise la DLL windows permettant d'utiliser les sockets
void activeWinsocket(void);

// A appeler sous Windows apres l'utilisation des sockets
void desactiveWinsocket(void);
#endif

#endif
