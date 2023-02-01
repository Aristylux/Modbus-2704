#include "debug.h"

void printDebug(char * function, char * message) {
    if (DEBUG == YES)
        printf("[DEBUG] %s: %s\n", function, message);
}