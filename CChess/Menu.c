/*
File:           Menu.c
Author:         Toni Lindeman
Description:    Functionality for the menu.
*/

#include <stdio.h>
#include <string.h>
#include "UserInput.h"

#define FILE_INFO "info.info"
#define FILE_WELCOME "welcome.info"
#define FILE_SCNO_EDIT_INFO "scenario_editor.info"

int menu() {
    /*
     Prints menu and gets menu choice.
     */
    printf("1 - Play game\n");
    printf("2 - Load game\n");
    printf("3 - Scenario builder\n");
    printf("4 - Info\n");
    printf("0 - Exit\n\n");

    // Get user choice
    int userChoice = 0;
    userChoice = getMenuChoice(0, 4);

    return userChoice;
}

int scenarioMenu() {
    /*
     Prints menu and gets menu choice.
     */
    printf("1-3 - Play scenario 1-3\n");
    printf("4-6 - Edit scenario 1-3\n");
    printf("7   - New scenario\n");
    printf("0   - Cancel\n\n");

    // Get user choice
    int userChoice = 0;
    userChoice = getMenuChoice(0, 7);

    return userChoice;
}

int saveScenarioMenu() {
    printf("Save scenario? Saving overwrites slot.\n");
    printf("0 - No\n");
    printf("1 - Scenario slot 1\n");
    printf("2 - Scenario slot 2\n");
    printf("3 - Scenario slot 3\n");

    // Get user choice
    int userChoice = 0;
    userChoice = getMenuChoice(0, 3);

    return userChoice;
}

void printInfo(char * whichText) {
    // Get pointer to welcome file
    FILE * file = NULL;
    if(!strcmp(whichText, "welcome")) {
        file = fopen(FILE_WELCOME, "r");
    }
    else if(!strcmp(whichText, "information")) {
        file = fopen(FILE_INFO, "r");
    }
    else if(!strcmp(whichText, "scenario")) {
        file = fopen(FILE_SCNO_EDIT_INFO, "r");
    }
    else {
        printf("Unknown value provided to printInfo!\n");
        return;
    }


    // If file was opened successfully
    if(file) {
        // String buffer
        char buffer[100];

        // Read the file and print line by line until end of file.
        while(!feof(file)) {
            fgets(buffer, 100, file);
            printf("%s\n", buffer);
        }

        // Close file
        fclose(file);

        // Wait for user input
        promptReturnToContinue();
    }
    else {
        printf("Sorry, failed to load info file :/\n");
    }
}
