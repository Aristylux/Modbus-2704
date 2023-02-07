#include "debug.h"

/**
 * @brief Print debug if actif
 * 
 * @param function actual function name
 * @param message debug message
 */
void printDebug(char * function, char * message) {
    if (DEBUG == YES)
        printf("[DEBUG] %s: %s\n", function, message);
}

/**
 * @brief Check arguments
 * 
 * @param argc number of arguments
 * @param argv all arguments
 * @return int mode : MODE_MAIN, MODE_HELP, MODE_DEBUG, ERROR
 */
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

/**
 * @brief print help
 * 
 */
void printHelp(void)
{
	puts("\tHELP\n");
	puts("Usage:");
	puts("\t.\\Mod_E2704.exe [Option]\n");
	puts("Options:");
	puts("\t-h\tPrint Help");
	puts("\t-d\tDebug mode\n");
	puts("Examples:");
	puts("\t.\\Mod_E2704.exe");
	puts("\t.\\Mod_E2704.exe -h");
	puts("\t.\\Mod_E2704.exe -d\n\n");
}