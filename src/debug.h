#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <string.h>

#define NO 0
#define YES 1

#define DEBUG YES

typedef enum typemode {
  MODE_HELP = 0,
  MODE_DEBUG,
  MODE_ERROR_TMA, // too many arguments
  MODE_ERROR_IA   // invalid argument
} TypeMode;

// Print debug if actif
void printDebug(char * function, char * message);

// Check arguments
int checkArguments(int argc, char ** argv);

#endif