/*
File:           UserInput.c
Author:         Toni Lindeman
Description:    Functionality for getting and validating user input.
                For consistency, ascii decimal values are avoided here. Be aware of this when reading the code.
                Ex. '0' = 48, '\0' = 0 (null terminator).
                Also for consistency, I take all input into a char array, e.g. scanf("%d") is not used to get integers
                from user. Rather, getInput function is used for all input.
*/

#include <stdio.h>
#include <stdlib.h>

// TODO: Consider changing player move to be [from] [to] format, rather than asking separately.

void promptReturnToContinue() {

    // Simple press return to continue.
    printf("Press return to continue...");

    // Clear buffer
    while(getchar() != '\n');
}

void getInput(char * charArray, int size) {
    /* A little help function for taking input.
     * We want to consume all of stdin, so there isn't anything there next time we read from it.
     *
     * Implication: This also means that if the user enters more than expected, the function disregards the extra input.
     * I do not consider this a problem for this program.
    */

    // Size mustn't exceed (max - 1). Max - 1, because the last element is the NULL terminator.
    if(size > 99) {
        printf("Size given to getInput exceeds its maximum of 100!\n");
        promptReturnToContinue();
        return;
    }

    // Allocate large buffer in heap.
    char * buffer = (char *) calloc(100, sizeof(char));

    // Read from stdin
    fgets(buffer, 100, stdin);

    // If user entered anything
    if(buffer[0] != '\n') {
        // Put chars to charArray according to given size.
        for(int i = 0; i < size; i++) {
            charArray[i] = buffer[i];
        }
    }
    // Else user didn't enter anything and we set first element to ascii NULL to let the caller know.
    else {
        charArray[0] = '\0';
    }


    // Free allocated memory
    free(buffer);
    // There shouldn't be risk of double freeing here, but let's play it super safe.
    buffer = NULL;
}

int asciiNumberToDecimal(char number) {
    // Help function to turn '0'-'9' to 0 - 9.
    return (number - 48);
}

int promptYesNo(char * prompt) {
    // Simple yes/no prompt, if strict is true only 'y' is affirmative.
    char userChoice[2] = "\0";

    while(userChoice[0] == '\0') {
        // Prompt user
        printf("%s", prompt);
        // Get input
        getInput(userChoice, 2);

        if(userChoice[0] == '\0') {
            continue;
        }

        // If users says no
        if(userChoice[0] == 'N' || userChoice[0] == 'n' || userChoice[0] == '0') {
            return 0;
        }

        // If yes and not strict checking
        else if((userChoice[0] == 'Y' || userChoice[0] == 'y' || userChoice[0] == '1')) {
            return 1;
        }

        // Else we do not understand what the user wants.
        else {
            printf("\nSorry, I don't understand.\n");
            printf("Yes => Y, y or 1.\n");
            printf("No => N, n or 0.\n\n");
            userChoice[0] = '\0';
        }
    }
    // We should never get here but if we do, return -1 for error.
    return -1;
}

int promptPromotePawn() {
    // Player chooses what rank pawn is promoted to.
    // Options: rook, knight, bishop and queen (2-5).
    char userChoice[] = "\0";

    printf("\nYou can now promote the pawn.\n");
    printf("2: rook; 3: knight; 4: bishop; 5: queen \n");

    while(asciiNumberToDecimal(userChoice[0]) < 2 || asciiNumberToDecimal(userChoice[0]) > 5) {
        // Prompt user
        printf("What rank should the pawn get? ");

        getInput(userChoice, 2);

        if(userChoice[0] == '\0') {
            continue;
        }

        // Validate input
        if(asciiNumberToDecimal(userChoice[0]) < 2 || asciiNumberToDecimal(userChoice[0]) > 5) {
            printf("Please select from 2-5 (rook, knight, bishop, queen).\n");
        }
    }

    return asciiNumberToDecimal(userChoice[0]);
}

int getMenuChoice(int min, int max) {
    // Get user choice for menu options.
    // For consistency I use getInput with char array, rather than using scanf("%d") and a int.
    // Read description on top for more information.
    // Note that this function only handles single digit values.

    char userChoice[] = "\0";

    while(asciiNumberToDecimal(userChoice[0]) < min || asciiNumberToDecimal(userChoice[0]) > max) {
        // Prompt user
        printf("What would you like to do? ");

        getInput(userChoice, 2);

        if(userChoice[0] == '\0') {
            continue;
        }

        // Validate input
        if(asciiNumberToDecimal(userChoice[0]) < min || asciiNumberToDecimal(userChoice[0]) > max) {
            printf("Please choose an option from the menu (%d - %d)\n", min, max);
        }
    }
    return asciiNumberToDecimal(userChoice[0]);
}

void getSelectSquare(int * outX, int * outY, int promptSelectOrMove) {
    // This function is for picking a chess piece or square to move to.
    // The function does not validate whether the selection/move was valid, only that the format is valid.

    // char array of size 3 (including null terminator)
    char userChoice[] = "xx";

    do {
        // Prompt user
        if(!promptSelectOrMove) {
            printf("Select chess piece (A1 - H8): ");
        }
        else {
            printf("Select where to move (A1 - H8): ");
        }

        // Get input from user
        getInput(userChoice, 3);

        // If user didn't enter anything (just pressed return), continue from top.
        if(userChoice[0] == '\0') {
            continue;
        }

        // Check for exit value
        if(userChoice[0] == '0') {
            *outX = -1;
            *outY = -1;
            // Exit function
            return;
        }

        // Turn lowercase letters a-h (first char) to upper case letters
        if(userChoice[0] >= 'a' && userChoice[0] <= 'h') {
            userChoice[0] -= 32;
        }

        // Validate input
        if(userChoice[0] < 'A' || userChoice[0] > 'H' || userChoice[1] < '1' || userChoice[1] > '8') {
            printf("That is not a valid square selection.\n");
            printf("Please pick from A1, A2, A3 ... H8.\n");
            // Validation failed, set userChoice back to '\0' to continue looping.
            userChoice[0] = '\0';
        }

    } while(userChoice[0] == '\0');

    // Set out x and out y values converted to range 0...7.
    // Char 2 must also be converted since e.g. ascii 1 has integer value 49.
    *outX = userChoice[0] - 'A';
    *outY = userChoice[1] - '1';
}

void getScenarioPlacement(int * placement) {
    // This function is for making a placement in scenario editor
    char userChoice[] = "0 0 xx";

    do {
        // Prompt user
        printf("Place: ");

        getInput(userChoice, 7);

        // If user didn't enter anything (just pressed return)
        if(userChoice[0] == '\0') {
            continue;
        }

        // Check for exit value
        if(userChoice[0] == '0' && userChoice[3] == '\0') {
            placement[0] = -1;
            // Exit function
            return;
        }

        // TODO: The following validation code probably needs some cleanup / improvement.

        // Check if "clear square"
        if(userChoice[0] == '0') {

            // Then index 2 and 3 must hold the position
            // Turn lowercase letters a-h (first char) to upper case letters
            if(userChoice[2] >= 'a' && userChoice[2] <= 'h') {
                userChoice[2] -= 32;
            }

            // Validate format
            if(userChoice[2] < 'A') {
                printf("Invalid clearing format.\n");
                printf("For clearing use '0 [position]', ex. '0 B1'.\n");
                // Set flag to continue loop
                userChoice[0] = '\0';
            }

            // Validate
            else if(userChoice[2] < 'A' || userChoice[2] > 'H' || userChoice[3] < '1' || userChoice[3] > '8') {
                printf("That is not a valid square selection.\n");
                printf("Please pick from A1, A2, A3 ... H8.\n");
                // Set flag to continue loop
                userChoice[0] = '\0';
            }
            else {
                // Valid clear square, break out of loop.
                break;
            }
        }
        // Else it must be normal placement
        else {

            // Validate player
            if (userChoice[0] < '1' || userChoice[0] > '2') {
                printf("Invalid input, player must be 1 or 2.\n");
                userChoice[0] = '\0';
            }

            // Validate rank (1 - 6);
            else if (userChoice[2] < '1' || userChoice[2] > '6') {
                printf("Invalid input, rank must be 1-6.\n");
                userChoice[0] = '\0';
            }

            // To upper if needed.
            if (userChoice[4] >= 'a' && userChoice[4] <= 'h') {
                userChoice[4] -= 32;
            }

            // Validate position
            if (userChoice[4] < 'A' || userChoice[4] > 'H' || userChoice[5] < '1' || userChoice[5] > '8') {
                printf("That is not a valid square selection.\n");
                printf("Please pick from A1, A2, A3 ... H8.\n");
                // Set flag to continue loop
                userChoice[0] = '\0';
            }
        }

    } while(userChoice[0] == '\0');


    // Set values for caller (player, rank, x, y)
    // Clearing a square
    if(userChoice[0] == '0') {
        placement[0] = 0;                   // Player
        placement[1] = 0;                   // Rank
        placement[2] = userChoice[2] - 'A';  // X
        placement[3] = userChoice[3] - '1';  // Y
    }

    // Placing a chess piece
    else {
        placement[0] = userChoice[0] - '0';  // Player
        placement[1] = userChoice[2] - '0';  // Rank
        placement[2] = userChoice[4] - 'A';  // X
        placement[3] = userChoice[5] - '1';  // Y
    }

}

