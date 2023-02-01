#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include "E2704.h"
#include "Modbus.h"

int main(void)
{
    BOOL connexionOk = FALSE;
    HANDLE handleSerialPort = NULL;

    printf("****************************************************************************\n");
    printf("*                             PROTOCOLE MODBUS                             *\n");
    printf("****************************************************************************\n");

    //*******************************************************************************
    // Creation et ouverture du support de communication
    handleSerialPort = connectionSerialPort();
    //*******************************************************************************

    if (handleSerialPort == NULL)
    {
        printf("Echec de connexion.");
        return 1;
    }

    TypeRequest requestType = NO_REQUEST;
    TypeVal typeVal = NO_TYPE;

    while (requestType != REQUEST_QUIT)
    {
        char trameToSend[100];
        int lengthTrameToSend = 0;
        char trameReceived[100];
        int lengthTrameReceived = 99;
        memset(trameReceived, '\0', sizeof(trameReceived));

        ErrorComm codret = ERRORCOMM_ERROR;

        printf("\n****************************************************************************\n");
        printf("\t1. Demande de lecture.\n");
        printf("\t2. Demande d'ecriture.\n");
        printf("\t3. Quitter.\n");
        printf("Que souhaitez-vous faire?\n");
        scanf("%d", &requestType);

        //*******************************************************************************
        // Creation de la trame de requete Modbus
        if (requestType == REQUEST_READ || requestType == REQUEST_WRITE)
            lengthTrameToSend = createRequestTrame(requestType, trameToSend, &typeVal);
        else
            continue;

        printTrame("Send", trameToSend, lengthTrameToSend);

        //*******************************************************************************
        // Envoie de la requete Modbus sur le supporte de communication et reception de la trame reponse
        if (lengthTrameToSend)
        {
            // A COMPLETER (fait)
            puts("[DEBUG] main: set request modbus");
            codret = sendAndReceiveSerialPort(handleSerialPort, INFINITE, trameToSend, lengthTrameToSend, trameReceived, &lengthTrameReceived);

            printf("code: %d, lengthTrameReceived: %d", codret, lengthTrameReceived);
        }

        //*******************************************************************************
        // Decodage de la trame reçue
        if (codret != ERRORCOMM_NOERROR || lengthTrameReceived == 0) printState(codret);
        else
        {
            printTrame("Receive", trameReceived, lengthTrameReceived);

            codret = parseModbusResponse(trameReceived, lengthTrameReceived, requestType, typeVal);
            if (codret != ERRORCOMM_NOERROR) printState(codret);
            //*******************************************************************************
        }
    }

    //*******************************************************************************
    // Fermeture du support de communication

    // A COMPLETER (fait)
    puts("[DEBUG] main: close modbus connection");
    terminateSerialPort(handleSerialPort);

    //*******************************************************************************
    return 0;
}