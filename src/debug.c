#include "debug.h"

void printDebug(char * function, char * message) {
    if (DEBUG == YES)
        printf("[DEBUG] %s: %s\n", function, message);
}

int checkArguments(int argc, char ** argv){
	if (argc == 1)
        return MODE_MAIN;
    else if (argc == 2)
    {
        // Print help
        if (strcmp(argv[1], "-h") == 0)
            return MODE_HELP;
        // Enable debug mode (like TP3)
        else if (strcmp(argv[1], "-d") == 0)
            return MODE_DEBUG;
        // No correspondance
        else
            return MODE_ERROR_IA;
    }
    // Print error
    else if (argc >= 3) 
        return MODE_ERROR_TMA;
}