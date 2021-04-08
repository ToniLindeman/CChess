/*
File            Main.c
Author          Toni Lindeman
Description:    Program starts here, all gameplay logic is handled in the rest of the source files.
*/

#include <stdlib.h>
#include "Gameplay.h"

// Command line argument 1: show welcome text
int main(int argc, char * argv[]) {
    if(argc == 2) {
        int showWelcome = 1;
        showWelcome = atoi(argv[1]);
        start(showWelcome);
    }
    else {
        start(1);
    }

    return 0;
}
