#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include <conio.h>      // keybord interruption
#include <time.h>       // clock()

#include "debug.h"
#include "E2704.h"
#include "Modbus.h"

int main(int argc, char** argv)
{
    // Welcome message
    printf("****************************************************************************\n");
    printf("*                         PROTOCOLE MODBUS E2704                           *\n");
    printf("****************************************************************************\n");

    // Check arguments
    if(argc == 2){
        if (strcmp(argv[1], "-h") == 0){
            // Print help

        }
        return EXIT_SUCCESS;
    } else if (argc >= 3) {
        // Print error
        puts("Too many arguments");
        return EXIT_FAILURE;
    }

    // Main program
    HANDLE handleSerialPort = NULL;
    
    // Creation et ouverture du support de communication
    handleSerialPort = connectionSerialPort();
    //*******************************************************************************

    // If the connection is not established, exit 
    if (handleSerialPort == NULL){
        puts("Echec de connexion.");
        return 1;
    }
    
    printf("\n\tPress 'q' to quit program.\n\tExecute .\\Mod_E2704 -h for help.\n\n");   

    clock_t begin, end;
    begin = clock();
    while (1)
    {
        // Get new tick
        end = clock();

        // If total tick > 1s, update & execute 
        if((end - begin) >= 1000) {
            begin = end;

            printf("This information is being printed every second\n");
        }
        
        // Keybord interruption
        if (kbhit())
        {
            char c = getch();
            if (c == 'q')
                break;
        }

        // Wait 10ms to avoid over speed processor
        Sleep(10);
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
            printDebug("main", "set request modbus");
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

    // Fermeture du support de communication
    printDebug("main", "close modbus connection");
    terminateSerialPort(handleSerialPort);

    return EXIT_SUCCESS;
}