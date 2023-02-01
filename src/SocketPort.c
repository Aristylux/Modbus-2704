/*
 *  SocketLib.c
 *  isentlib
 *
 *  Created by Ghislain Oudinet on 27/09/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */


#include "SocketPort.h"


// Cree une nouvelle connexion qui pourra ensuite etre branchee sur une adresse et un port particulier
SOCKET createSocket(int i_dSizeBuffer /* = 512 */, int i_dProtocole /* = PROTOCOLE_TCP */, int i_dTimeoutsend /*= -1*/, int i_dTimeoutrecv /*= -1*/)
{
	SOCKET idSocket = INVALID_SOCKET;

	// Ouverture d'un flux de donnees Internet
	switch (i_dProtocole)
	{
		case PROTOCOLE_TCP:
			idSocket = socket (AF_INET, SOCK_STREAM, PROTOCOLE_TCP);
			break;
		case PROTOCOLE_UDP:
			idSocket = socket (AF_INET, SOCK_DGRAM , PROTOCOLE_UDP);
			break;
	}

	if (idSocket == INVALID_SOCKET )
	{
		printf("Creation de socket impossible, protocole: %d", i_dProtocole);
		return INVALID_SOCKET;
	}
	else
	{
		// par defaut, le buffer est limite a 512 octets
        // on redimensionne les buffers d'emission et de reception a la taille passee en appel
		if ( setsockopt(idSocket, SOL_SOCKET, SO_RCVBUF, (char *) &i_dSizeBuffer, sizeof(i_dSizeBuffer)) == -1 )
		{
			printf("Create : erreur lors du redimensionnement du buffer de reception a %d",i_dSizeBuffer) ;
		}

		if ( setsockopt(idSocket, SOL_SOCKET, SO_SNDBUF, (char *) &i_dSizeBuffer, sizeof(i_dSizeBuffer)) == -1 )
		{
			printf("Create : erreur lors du redimensionnement du buffer d'emission a %d", i_dSizeBuffer) ;
		}

		//gestion du timeout en recpetion
		if ( i_dTimeoutrecv >0 &&
			 setsockopt(idSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&i_dTimeoutrecv, sizeof(i_dTimeoutrecv)) == -1 )
		{
			printf("Parametrage socket : erreur gestion timeout sur reception (SO_RCVTIMEO)");
		}

		//gestion du timeout en emission
		if ( i_dTimeoutsend >0 &&
			 setsockopt(idSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&i_dTimeoutsend, sizeof(i_dTimeoutsend)) == -1 )
		{
            printf("Parametrage socket : erreur gestion timeout sur emission (SO_SNDTIMEO)");
		}
	}

	return idSocket; 
}

// Essaie de brancher une connexion sur une machine dont l'adresse et le port de communication sont donnes
BOOL connectSocket(SOCKET i_dDescripteurConnexion, const char *i_sAdresseIP, short i_dPort)
{
	struct sockaddr_in adresseConnexion;

	adresseConnexion.sin_family = AF_INET; // La famille d'adressage ciblee est celle d'Internet
	adresseConnexion.sin_port = htons (i_dPort);// Le port selectionne
	adresseConnexion.sin_addr.s_addr = inet_addr(i_sAdresseIP); // L'adresse selectionne
	memset(adresseConnexion.sin_zero, '\0', sizeof(adresseConnexion.sin_zero)); // Mise a zero obligatoire

	// On essaie de se connecter
	if (connect(i_dDescripteurConnexion, (struct sockaddr *)&adresseConnexion, sizeof(adresseConnexion)) == 0)
	{
		printf("Connect OK.");
		return TRUE;
	}
	else
	{
		printf("Connect KO.");
		return FALSE;
	}
}

// Attend qu'une connexion tierce soit etablie sur la machine presente et sur le port donne en parametre
BOOL bindSocket(SOCKET i_dDescripteurConnexion, short port)
{
	struct sockaddr_in adresseLocale;

	int connexion = 0;

	adresseLocale.sin_family = AF_INET;; // La famille d'adressage ciblee est celle d'Internet
	adresseLocale.sin_port = htons(port); // Le port selectionne
	adresseLocale.sin_addr.s_addr = htonl(INADDR_ANY); // Choix automatique de l'adresse IP locale
	memset(adresseLocale.sin_zero, '\0', sizeof(adresseLocale.sin_zero)); // Mise a zero obligatoire

	if (bind(i_dDescripteurConnexion, (struct sockaddr *)&adresseLocale, sizeof(adresseLocale)) != -1) // On autorise le referencement de la connexion
	{
		printf("Bind OK.");
		return TRUE;
	}	
	else
	{
		printf("Bind KO.");
		return FALSE;
	}
}

SOCKET acceptSocket(SOCKET i_dDescripteurConnexion, short i_dPort, short i_dLqueue /* = 5 */)
{
	SOCKET idClientSocket = INVALID_SOCKET;

    BOOL sucess = bindSocket(i_dDescripteurConnexion, i_dPort);
	if (sucess != SOCKET_ERROR)
	{
		sucess = listen(i_dDescripteurConnexion, i_dLqueue);
		if (sucess != SOCKET_ERROR)
		{
			struct sockaddr_in adresseLocale;
			struct sockaddr_in adresseClient;

#ifdef _WIN32
			int sockaddr_in_size = sizeof(adresseClient);
#else
			socklen_t sockaddr_in_size = (socklen_t)sizeof(adresseClient);
#endif
			int connexion = 0;

			adresseLocale.sin_family = AF_INET;; // La famille d'adressage ciblee est celle d'Internet
			adresseLocale.sin_port = htons(i_dPort); // Le port selectionne
			adresseLocale.sin_addr.s_addr = htonl(INADDR_ANY); // Choix automatique de l'adresse IP locale

			memset(adresseLocale.sin_zero, '\0', sizeof(adresseLocale.sin_zero)); // Mise a zero obligatoire
			
			idClientSocket = accept(i_dDescripteurConnexion, (struct sockaddr *)&adresseClient, &sockaddr_in_size); // On accepte le flux d'informations qui en arrive
			if (idClientSocket != INVALID_SOCKET)
			{
				printf("Accept OK.");
				return TRUE;
			}	
			else
			{
				printf("Accept KO.");
				return FALSE;
			}
		}
	}

	return (sucess!= SOCKET_ERROR) ? idClientSocket : INVALID_SOCKET;
}



//*    TCP   *//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Envoie sur une connexion le flux de donnees fourni (attention a l'endianness des donnees)
// Le nombre d'octets ecrits est renvoye
int sendSocket( SOCKET i_dDescripteurConnexion, const char *i_pDdonnees, int i_iLongueurDonnees)
{
	return send(i_dDescripteurConnexion, i_pDdonnees, i_iLongueurDonnees, 0);
}

// Receptionne a partir d'une connexion un flux de donnees dans une zone de memoire passee en parametre
// Le nombre d'octets lus est renvoye
int recvSocket( SOCKET i_dDescripteurConnexion,char *i_pDonnees, int i_iLongueurDonnees)
{
	int nbCar = recv(i_dDescripteurConnexion, i_pDonnees, i_iLongueurDonnees, 0);

	return nbCar;
}

ErrorComm sendAndReceiveSocket(SOCKET i_dDescripteurConnexion, int i_iTimeOut, char* i_sTrameIn, int i_lengthIn, char* i_sTrameOut, int* i_lengthOut)
{
    ErrorComm result = ERRORCOMM_NOERROR;

    selectSocket(i_dDescripteurConnexion, i_iTimeOut, SOCKET_SELECTWRITE);

    if( sendSocket(i_dDescripteurConnexion,  i_sTrameIn, i_lengthIn) != SOCKET_ERROR)
        result = ERRORCOMM_NOERROR;
    else
        result = ERRORCOMM_WRITE;

    if (result == ERRORCOMM_NOERROR)
    {
        selectSocket(i_dDescripteurConnexion, i_iTimeOut, SOCKET_SELECTREAD);
        *i_lengthOut = recvSocket(i_dDescripteurConnexion, i_sTrameOut ,*i_lengthOut);

        if ((*i_lengthOut == 0) || (*i_lengthOut == SOCKET_ERROR))
            result = ERRORCOMM_TIMEOUT;
    }

    return result;
}

//*    UDP   *//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Envoie sur une connexion le flux de donnees fourni (attention a l'endianness des donnees)
// Le nombre d'octets ecrits est renvoye
int sendToSocket( SOCKET i_dDescripteurConnexion, const char *i_pDdonnees, int i_iLongueurDonnees, const char *i_sAdresseIP, short i_dPort)
{
	struct sockaddr_in to;
	to.sin_family = AF_INET;; // La famille d'adressage ciblee est celle d'Internet
	to.sin_port = htons(i_dPort); // Le port selectionne
	to.sin_addr.s_addr = inet_addr(i_sAdresseIP); // L'adresse selectionne
	memset(to.sin_zero, '\0', sizeof(to.sin_zero)); // Mise a zero obligatoire

	return sendto(i_dDescripteurConnexion, i_pDdonnees, i_iLongueurDonnees, 0,(SOCKADDR *)&to, sizeof(to));
}

// Receptionne a partir d'une connexion un flux de donnees dans une zone de memoire passee en parametre
// Le nombre d'octets lus est renvoye
int recvFromSocket( SOCKET i_dDescripteurConnexion, char *i_pDonnees, int i_iLongueurDonnees)
{
    SOCKADDR_IN from = { 0 };
    int fromsize = sizeof from;

    int nbCar = recvfrom(i_dDescripteurConnexion, i_pDonnees, i_iLongueurDonnees, 0,(SOCKADDR *)&from, &fromsize);

    return nbCar;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL selectSocket(SOCKET i_dDescripteurConnexion, int i_iTimeInMiliSec, int i_iFlags)
{
	FD_SET fd = {1, i_dDescripteurConnexion};
	TIMEVAL tv = {i_iTimeInMiliSec, 0};

	int nbFile = 0;
	switch (i_iFlags)
	{
		case SOCKET_SELECTALL:
			nbFile = select(0, &fd, &fd, NULL, &tv);
			break;
		case SOCKET_SELECTREAD:
			nbFile = select(0, &fd , NULL, NULL, &tv);
			break;
		case SOCKET_SELECTWRITE:
			nbFile = select(0, NULL, &fd, NULL, &tv);
			break;
	}

	return (nbFile) ? TRUE : FALSE;
}


// Termine la connexion
void terminateSocket(SOCKET i_dDescripteurConnexion)
{
#ifndef _WIN32
	close(i_dDescripteurConnexion);
#else
	closesocket(i_dDescripteurConnexion);
#endif
}


#ifdef _WIN32
// A appeler sous Windows avant toute utilisation des sockets =>  initialise la DLL windows permettant d'utiliser les sockets
void activeWinsocket(void)
{
     WSADATA wsaData;
     if (WSAStartup(MAKEWORD(2, 2), &wsaData)) exit(1);
}

// A appeler sous Windows apres l'utilisation des sockets
void desactiveWinsocket(void)
{
     WSACleanup();
}

#endif
