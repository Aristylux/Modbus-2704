#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <string.h>

#define NO 0
#define YES 1

#define DEBUG NO

typedef enum typemode
{
    MODE_HELP = 0,
    MODE_MAIN,
    MODE_DEBUG,
    MODE_ERROR_TMA, // too many arguments
    MODE_ERROR_IA   // invalid argument
} TypeMode;


void printDebug(char *function, char *message);
int checkArguments(int argc, char **argv);

void printHelp(void);

#endif