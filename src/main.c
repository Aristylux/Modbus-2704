#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include "debug.h"
#include "E2704.h"

int main(int argc, char **argv)
{
    BOOL debugEnabled = FALSE;

    // -- Welcome message --
    printf("****************************************************************************\n");
    printf("*                         PROTOCOLE MODBUS E2704                           *\n");
    printf("****************************************************************************\n");

    // -- Check arguments --
    switch (checkArguments(argc, argv))
    {
    case MODE_MAIN:
        break;
    case MODE_HELP:
        printHelp();
        return EXIT_SUCCESS;
    case MODE_ERROR_IA:
        puts("Invalid Arguments");
        return EXIT_FAILURE;
    case MODE_ERROR_TMA:
        puts("Too many arguments");
        return EXIT_FAILURE;
    case MODE_DEBUG:
        debugEnabled = TRUE;
    }
    
    // -- Connection --
    printDebug("main", "open modbus connection");
    HANDLE handleSerialPort = NULL;

    // Creation et ouverture du support de communication
    handleSerialPort = connectionSerialPort();

    // If the connection is not established, exit
    if (handleSerialPort == NULL)
    {
        puts("Echec de connexion.");
        return EXIT_FAILURE;
    }

    // -- Main program --
    // If debug mode enabled, go to verbal program
    // If debug mode disabled, go to E2704 visualisator
    if (debugEnabled == FALSE)
        E2704_main(handleSerialPort);
    else
        E2704_debug(handleSerialPort);

    // Fermeture du support de communication
    printDebug("main", "close modbus connection");
    terminateSerialPort(handleSerialPort);

    return EXIT_SUCCESS;
}