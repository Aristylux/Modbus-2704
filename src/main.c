#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include "debug.h"
#include "E2704.h"

int main(int argc, char **argv)
{
    // Welcome message
    printf("****************************************************************************\n");
    printf("*                         PROTOCOLE MODBUS E2704                           *\n");
    printf("****************************************************************************\n");

    BOOL debugEnabled = FALSE;

    // Check arguments
    if (argc == 2)
    {
        // Print help
        if (strcmp(argv[1], "-h") == 0)
        {
            printHelp();
            return EXIT_SUCCESS;
        }
        // Enable debug mode (like TP3)
        else if (strcmp(argv[1], "-d") == 0)
            debugEnabled = TRUE;
        else
        {
            printf("Invalid Arguments");
            return EXIT_FAILURE;
        }
    }
    else if (argc >= 3)
    {
        // Print error
        puts("Too many arguments");
        return EXIT_FAILURE;
    }

    // -- Connection --
    HANDLE handleSerialPort = NULL;

    // Creation et ouverture du support de communication
    handleSerialPort = connectionSerialPort();

    // If the connection is not established, exit
    if (handleSerialPort == NULL)
    {
        puts("Echec de connexion.");
        return 1;
    }

    // -- Main program --

    if (debugEnabled == FALSE)
        E2704_main(handleSerialPort);
    else
        E2704_debug(handleSerialPort);

    // Fermeture du support de communication
    printDebug("main", "close modbus connection");
    terminateSerialPort(handleSerialPort);

    return EXIT_SUCCESS;
}