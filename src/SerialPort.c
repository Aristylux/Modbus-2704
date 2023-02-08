/*
 *  SerialPortLib.c
 *  isentlib
 *
 *  Created by Audrey SIEPRAWSKI on 21/03/2015.
 *
 */


#include "SerialPort.h"
#include <conio.h> 

void GetPortName(int idPort, char* portName)
{
    if (idPort <= 9)
        sprintf(portName, "COM%d", idPort);
	else
        sprintf(portName, "\\\\.\\COM%d", idPort);

    return;
}

// Cree une nouvelle connexion sur le port serie
HANDLE createSerialPort(int i_numPort)
{
	HANDLE hPort = INVALID_HANDLE_VALUE;
    char portName[10];
    GetPortName(i_numPort, portName);

	// ouverture port communication, retourne le handle du port
    hPort = CreateFileA( portName,
		GENERIC_READ | GENERIC_WRITE, 
		0,	// aucun privilege de partage (pas en reseau)
        0,	// securite par defaut
		OPEN_EXISTING,	// peripherique n'est pas un fichier
		FILE_FLAG_OVERLAPPED, // ASYNCHRONE => permet d'executer les opérations I/O en arrière plan. Le thread
							  // n'est pas bloqué grace à la gestion par evenement
        NULL);

	return hPort; 
}


// Essaie de brancher une connexion sur une machine dont l'adresse et le port de communication sont donnes
BOOL setParamSerialPort(HANDLE hPort, int i_baudRate, BYTE i_byteSize, BYTE i_parity, BYTE i_stopBits)
{
	BOOL commOk = FALSE;
	if(hPort != NULL && hPort != INVALID_HANDLE_VALUE)
	{
		DCB sysDcb;

		// lecture valeur timeout sur port communication
		COMMTIMEOUTS timeouts;
		commOk = GetCommTimeouts(hPort, &timeouts);

		timeouts.ReadIntervalTimeout=10;
		timeouts.ReadTotalTimeoutConstant=0;
		timeouts.ReadTotalTimeoutMultiplier=0;
		commOk = SetCommTimeouts(hPort, &timeouts);

		commOk = GetCommState(hPort, &sysDcb);

		// mise a jour DCB vitesse, nb bits de donnees, parite, et bits de stop
		switch (i_baudRate)
		{
            case 300:   sysDcb.BaudRate = CBR_300;   break;
            case 600:   sysDcb.BaudRate = CBR_600;   break;
            case 1200:  sysDcb.BaudRate = CBR_1200;  break;
            case 2400:  sysDcb.BaudRate = CBR_2400;  break;
            case 4800:  sysDcb.BaudRate = CBR_4800;  break;
            case 9600:  sysDcb.BaudRate = CBR_9600;  break;
            case 19200: sysDcb.BaudRate = CBR_19200; break;
            case 38400: sysDcb.BaudRate = CBR_38400; break;
            default:	sysDcb.BaudRate = CBR_9600;  break;
		}

		sysDcb.ByteSize	= i_byteSize;
		sysDcb.Parity	= i_parity;
		sysDcb.StopBits	= i_stopBits;
		sysDcb.DCBlength	= sizeof(sysDcb);

		// envoi nouvel etat du DCB vers le port
		commOk = SetCommState(hPort, &sysDcb);

		PurgeComm(hPort, PURGE_TXCLEAR|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_RXABORT);
		EscapeCommFunction(hPort, SETDTR);

	}
	return commOk;
}

void terminateSerialPort(HANDLE hPort)
{
	if (hPort!=NULL && hPort!=INVALID_HANDLE_VALUE)
		CloseHandle(hPort);

	return;
}

ErrorComm sendAndReceiveSerialPort(HANDLE hPort, int i_iTimeOut, char* i_sTrameIn, int i_lengthIn, char* i_sTrameOut, int* i_lengthOut)
{

	//puts("[DEBUG] sendAndReceiveSerialPort: ");

	ErrorComm result = ERRORCOMM_NOERROR;

	BOOL codRet = PurgeComm( hPort, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR );
	
	//puts("[DEBUG] sendAndReceiveSerialPort: purge ok");

	if(codRet)
	{
		FlushFileBuffers( hPort );

		result = sendSerialPort(hPort, i_iTimeOut,  i_sTrameIn, i_lengthIn );
		//puts("[DEBUG] sendAndReceiveSerialPort: result");
		if ( result == ERRORCOMM_NOERROR )
		{
            result = recvSerialPort(hPort, i_iTimeOut, i_sTrameOut, i_lengthOut);
			//puts("[DEBUG] sendAndReceiveSerialPort: result");
		}
	}

	return result;
}


ErrorComm sendSerialPort( HANDLE hPort, int timeOut,const char *i_pDdonnees, int i_iLongueurDonnees)
{
	ErrorComm code_erreur = ERRORCOMM_NOERROR;
	DWORD nbBytes;


	// Structures OVERLAPPED pour communication asynchrone
    OVERLAPPED lpOverWrite;

	//------------------------------------------------
	// Ecriture de la trame de commande

    //creation et controle des evenements d'ecriture sur le port serie
	lpOverWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(lpOverWrite.hEvent == NULL)
		code_erreur =ERRORCOMM_EVENT;
	else
	{
		// envoi de la trame de commande. WriteFile retourne FALSE car asynchrone
		BOOL writeOk = WriteFile(hPort, i_pDdonnees, i_iLongueurDonnees, &nbBytes, &lpOverWrite);

		if ( !writeOk && GetLastError() != ERROR_IO_PENDING)
		{
			code_erreur = ERRORCOMM_WRITE;
		}
		else
		{
			// ecriture en attente de terminaison
			// resultat de fonctions WIN32
            DWORD dwWrite = WaitForSingleObject (lpOverWrite.hEvent, timeOut);
			switch (dwWrite)
			{
				case WAIT_OBJECT_0:
					if(!GetOverlappedResult(hPort,&lpOverWrite, &nbBytes, TRUE))
					{
						code_erreur =  ERRORCOMM_WRITE;
					}
					break;
				case WAIT_TIMEOUT:
					code_erreur =  ERRORCOMM_TIMEOUT;
					break;
				default:
					code_erreur =  ERRORCOMM_WAITOBJ;
					break;
			}
		}

		// dans tous les cas, fermeture de l'event local
		CloseHandle(lpOverWrite.hEvent);
	}

	return code_erreur;
}


// Receptionne a partir d'une connexion un flux de donnees dans une zone de memoire passee en parametre
// Le nombre d'octets lus est renvoye
ErrorComm recvSerialPort( HANDLE hPort, int timeOut, char *i_pDonnees, int *i_nbOctetsLus)
{	
	static BOOL interrup = FALSE;
	ErrorComm code_erreur = ERRORCOMM_NOERROR;
	DWORD nbBytes = 0;
	char trame[500];
	memset(trame,'\0',500);

	// Structures OVERLAPPED pour communication asynchrone
    OVERLAPPED lpOverRead;
	
	//------------------------------------------------
	// Lecture de la trame de reponse

    //creation d'un evenements de lecture sur le port serie: windows nous notifie quand un caractère est reçu
	lpOverRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(lpOverRead.hEvent == NULL)
		code_erreur = ERRORCOMM_EVENT;
	else
	{
		DWORD dwErrorCode = 0;
		COMSTAT status;
		BOOL stopRead = FALSE;
        int nbCarRead = 0;

		ClearCommError(hPort, &dwErrorCode, &status);
		
		// lecture de la trame recue
		// En asynchrone, readfile retourne FAUX et ERROR_IO_PENDING	
		while (!stopRead && code_erreur == ERRORCOMM_NOERROR)
		{
			// If there is an interruption for exit transmition, exit
			if(interrup == TRUE) {
				code_erreur = ERRORCOMM_INTERRUPT;
				break;
			}
			
			// If a keypress is active, active interruption
			if (kbhit())
			{
				char key = getch();
				if (key == 'q'){
					interrup = TRUE;
					code_erreur = ERRORCOMM_INTERRUPT;
					break;
				}
			}

            int nbBytesToRead = MAX_CAR_TRAME - nbCarRead;
            int readfileStatus = ReadFile(hPort,&trame + nbCarRead, nbBytesToRead, &nbBytes, &lpOverRead);

			if (readfileStatus!=0)
			{
				stopRead=TRUE;
				nbCarRead+=nbBytes;
			}
			else
			{
                DWORD lastError = GetLastError();
				if (lastError == ERROR_IO_PENDING)
				{
					// resultat de fonctions WIN32
                    DWORD dwRead = WaitForSingleObject (lpOverRead.hEvent, timeOut);
					
					switch (dwRead)
					{
						case WAIT_OBJECT_0:
							GetOverlappedResult(hPort,&lpOverRead, (&nbBytes), FALSE);
							nbCarRead=lpOverRead.InternalHigh;							
							stopRead = TRUE;
							break;

						case WAIT_TIMEOUT:
							GetOverlappedResult(hPort,&lpOverRead, &nbBytes, FALSE);
							nbCarRead=lpOverRead.InternalHigh;							
							stopRead = FALSE;
							
                            code_erreur = ERRORCOMM_TIMEOUT;
							break;

						default:
							code_erreur =  ERRORCOMM_WAITOBJ;
							break;
					}
				}
				else
					// Probleme de communication
					code_erreur =  ERRORCOMM_READ;
			}

		} // fin while


        if (nbCarRead <= *i_nbOctetsLus)
		{
			memcpy(i_pDonnees, &trame, nbCarRead);
            *i_nbOctetsLus = nbCarRead;
		}
		else
        // La trame recue est trop long et depasse MAX_CAR_TRAME
		{
            memcpy(i_pDonnees, &trame, *i_nbOctetsLus);
		}

		CloseHandle(lpOverRead.hEvent);
	}

	return code_erreur;
}
