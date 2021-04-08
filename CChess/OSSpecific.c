/*
File:           OSSpecific.c
Author:         Toni Lindeman
Description:    Contains system calls that may be OS specific. Check at least these if building for
                a different system.
 */

#include <stdlib.h>

void clearConsole() {
    // Call to clear console.
    system("clear");
}


