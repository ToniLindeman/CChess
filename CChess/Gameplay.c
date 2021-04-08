/*
File:           Gameplay.c
Author:         Toni Lindeman
Description:    Gameplay logic.
*/

#include <stdio.h>
#include <stdlib.h>
#include "Menu.h"
#include "UserInput.h"
#include "ChessPiece.h"
#include "Chessboard.h"

void playGame(int gameMode) {
    /* Gameplay logic
     * gameMode:
     *      0 -> Load game state
     *      1-3 -> Load scenario 1-3
     *      4 -> New standard game
     * */

    // White (player 1) always starts.
    int whoseTurn = 1;

    // ints for square selection and move.
    int xSelect = -1;
    int ySelect = -1;
    int xMove = -1;
    int yMove= -1;

    // Program checks if king is checked from the start of the move.
    // If so, only moves that end check are valid.
    int kingCheckedStart = 0;

    // Checkmate flag
    int checkmate = 0;

    // Clear console before game begins
    clearConsole();

    // Initialize a chessboard for the game.
    struct chessPiece * chessboard = NULL;
    chessboard = getInitChessboard();

    if(gameMode >= 0 && gameMode <= 3) {
        // If something goes wrong loading, free memory and get init chessboard.
        if(!loadGameState(chessboard, &whoseTurn, gameMode)) {
            promptReturnToContinue();
            // Load failed, free memory...
            freeChessboardMemory(chessboard);
            // and load initial chessboard setup.
            chessboard = getInitChessboard();
        }
    }

    // Make a copy of the chessboard to keep previous move.
    // We need this for check and checkmate validation.
    struct chessPiece * chessboardPrevious = NULL;
    chessboardPrevious = getEmptyChessboard();
    copyChessboard(chessboard, chessboardPrevious);

    // Game loop, make absolutely sure the game always can end in some way (quit or end condition).
    while(1) {
        // Clear console
        clearConsole();
        // Exit info text
        printf("Exit game / cancel move by entering '0' in selection.\n\n");
        // Draw chessboard
        printChessboard(chessboard);

        // Ask current player to make a move
        if(whoseTurn == 1) {
            printf("Player 1 turn\n");
        }
        else {
            printf("Player 2 turn\n");
        }

        // Check if king is checked from the start of the move.
        kingCheckedStart = checkForCheckedKing(chessboard, whoseTurn, 0);

        // If king is checked, check for checkmate.
        if(kingCheckedStart) {

            if(checkForCheckmate(chessboard, whoseTurn)) {
                checkmate = 1;
                break;
            }

        }

        while(1) {
            // Get player pick
            getSelectSquare(&xSelect, &ySelect, 0);

            // Exit loop if player has entered '0'.
            if(xSelect == -1) {
                break;
            }

            // Validate select
            if(validateSelect(chessboard, ySelect, xSelect, whoseTurn)) {
                    // Valid selection, break out of loop.
                    break;
            }
        }

        // Exit game loop if player has entered '0'.
        if(xSelect == -1) {
            break;
        }

        while(1) {
            // Prompt where player wants to move
            getSelectSquare(&xMove, &yMove, 1);
            if(xMove == -1) {
                break;
            }

            // Validate move
            if(validateAndMakeMove(chessboard, ySelect, xSelect, yMove, xMove, whoseTurn, 0, 0)) {
                // Now we need to check if king became / remains checked.
                if(checkForCheckedKing(chessboard, whoseTurn, 0)) {
                    // Kings is / remains checked => illegal move.
                    if(kingCheckedStart) {
                        printf("King remains checked, you must save the king.\n");
                    }
                    else {
                        printf("That move endangers your king, that is not allowed!\n");
                    }
                    // Cancel move
                    copyChessboard(chessboardPrevious, chessboard);

                }
                else {
                    // Move was ok, make move on chessboardPrevious and break out.
                    validateAndMakeMove(chessboardPrevious, ySelect, xSelect, yMove, xMove, whoseTurn, 0, 1);
                    break;
                }
            }
        }
        // Player chose to cancel move.
        if(xMove == -1) {
            continue;
        }

        // Change player turn
        whoseTurn = (whoseTurn % 2) + 1;
    }

    // If game ended in checkmate
    if(checkmate) {
        printf("\n\nPlayer %d won the game!\n", (whoseTurn % 2) + 1);
        promptReturnToContinue();
    }

    // Ask user whether they wish to save their game.
    else if(promptYesNo("Would you like to save the game (overwrites last save)? ")) {
        save(chessboard, whoseTurn, 0);
        promptReturnToContinue();
    }

    // Free memory allocated to chessboard and chessboard previous.
    chessboard = freeChessboardMemory(chessboard);
    chessboardPrevious = freeChessboardMemory(chessboardPrevious);

}

void scenarioEditor() {
    // Allows user to build chess scenarios
    clearConsole();
    int scenarioChoice = scenarioMenu();

    // If cancel
    if(scenarioChoice == 0) {
        return;
    }

    // Scenario chessboard.
    struct chessPiece * chessboard = NULL;
    // In scenarios player 1 always starts, we still need a variable for loading.
    int whoseTurn = 1;

    // Play scenario
    if(scenarioChoice >= 1 && scenarioChoice <= 3) {
        playGame(scenarioChoice);
        return;
    }

    // Load scenario
    else if(scenarioChoice >= 4 && scenarioChoice <= 6) {
        chessboard = getInitChessboard();
        // If loading scenario fails, e.g. file doesn't exist, create empty chessboard.
        if(!loadGameState(chessboard, &whoseTurn, scenarioChoice - 3)) {
            printf("Failed to load scenario, continuing with empty chessboard.\n");
            scenarioChoice = 7;
            chessboard = freeChessboardMemory(chessboard);
            promptReturnToContinue();
        }
    }

    // New scenario
    if(scenarioChoice == 7) {
        // Check if user wants to start with standard setup
        if(promptYesNo("Would you like to initialize scenario with standard setup? ") == 1) {
            chessboard = getInitChessboard();

        }
        else {
            chessboard = getEmptyChessboard();
        }
    }

    // Check that we have a chessboard to work with.
    if(!chessboard) {
        printf("Failed to initialize scenario editor!\n");
        promptReturnToContinue();
        return;
    }

    // Print info about scenario editor
    clearConsole();
    printInfo("scenario");

    // Array to keep track of number of chess pieces.
    // numPieces[0-5] => player1 pawn, rook, knight, bishop, queen, king
    // numPieces[6-11] => player2 -- || --
    int * numPieces = (int *)calloc(12, sizeof(int));
    int isLimitBreached = 0;
    int pieceLimits[] = {8, 2, 2, 2, 1, 1};

    // Count chess pieces.
    countChessPieces(chessboard, numPieces);

    /*
     * Scenario only allows:
     *      8 pawns
     *      2 rooks, knights and bishops
     *      1 queen and king.
     * */
    // TODO: Allow more freedom with piece placement, taking pawn promotion into account.
    //      This will require a more comprehensive check for piece counts. E.g. player has 10 queens (which is fine),
    //      then player cannot have a single pawn.

    // Array to hold chess piece placement data.
    // Player, chess piece, x, y
    int placement[4] = {0, 0, 0, 0};

    // Scenario editor loop
    while(1) {
        // First clear console.
        clearConsole();

        // Print info on top
        printf("0: Empty; 1: Pawn; 2: Rook; 3; Knight; 4: Bishop; 5: Queen; 6: King\n");
        printf("Place: '[player] [rank] [position]' | Clear: '0 [position]'.\n");
        printf("Enter '0' to stop.\n\n");
        printChessboard(chessboard);

        getScenarioPlacement(placement);

        // TODO: Allow user to exit without saving when requirements aren't met

        // Check for exit
        if(placement[0] == -1) {

            // Minimum requirements:
            //      - King per side
            //      - Neither player is in checkmate.
            //      - At least one other piece.

            // Check that both sides have a king
            if(numPieces[5] != 1 || numPieces[11] != 1) {
                printf("Both sides must have a king.\n");
                promptReturnToContinue();
                continue;
            }
            if(checkForCheckedKing(chessboard, 1, 0)) {
                if(checkForCheckmate(chessboard, 1)) {
                    printf("Player 1 is in checkmate, please modify the scenario.\n");
                    promptReturnToContinue();
                    continue;
                }
            }
            if(checkForCheckedKing(chessboard, 2, 0)) {
                if(checkForCheckmate(chessboard, 2)) {
                    printf("Player 2 is in checkmate, please modify the scenario.\n");
                    promptReturnToContinue();
                    continue;
                }
            }
            // Check that there is at least one other chess piece in addition to the kings.
            int pieceCounter = 0;
            for(int i = 0; i < 12; i++) {
                pieceCounter += numPieces[i];
            }
            if(pieceCounter < 3) {
                printf("This setup is a stalemate, please modify the scenario.\n");
                promptReturnToContinue();
                continue;
            }

            printf("You are about to stop editing this scenario.\n");
            if(promptYesNo("Are you sure? ") == 1) {
                break;
            }
            else {
                continue;
            }
        }

        // Get piece at current position
        struct chessPiece overwriteSquare = chessboard[(8 * placement[3]) + placement[2]];

        // Decrement piece counter unless square is empty --------------------------------------------------------------
        switch(overwriteSquare.rank) {
            case 0:
                break;

            // Pawn decrement
            case 1:
                if(overwriteSquare.owner == 1) {
                    numPieces[0] -= 1;
                }
                else {
                    numPieces[6] -= 1;
                }
                break;

            // Rook decrement
            case 2:
                if(overwriteSquare.owner == 1) {
                    numPieces[1] -= 1;
                }
                else {
                    numPieces[7] -= 1;
                }
                break;

            // Knight decrement
            case 3:
                if(overwriteSquare.owner == 1) {
                    numPieces[2] -= 1;
                }
                else {
                    numPieces[8] -= 1;
                }
                break;
            // Bishop decrement
            case 4:
                if(overwriteSquare.owner == 1) {
                    numPieces[3] -= 1;
                }
                else {
                    numPieces[9] -= 1;
                }
                break;
            // Queen decrement
            case 5:
                if(overwriteSquare.owner == 1) {
                    numPieces[4] -= 1;
                }
                else {
                    numPieces[10] -= 1;
                }
                break;
            // King decrement
            case 6:
                if(overwriteSquare.owner == 1) {
                    numPieces[5] -= 1;
                }
                else {
                    numPieces[11] -= 1;
                }
                break;
        }

        // Increment piece counter unless new piece is "clear square" --------------------------------------------------
        // Note: we allow exceeding count limit for a piece here.
        // Limit breach is checked later. This is done in the hopes of making code more readable here.
        switch(placement[1]) {
            case 0:
                break;

            // Pawn increment
            case 1:
                if(placement[0] == 1) {
                    numPieces[0] += 1;
                }
                else {
                    numPieces[6] += 1;
                }
                break;

            // Rook increment
            case 2:
                if(placement[0] == 1) {
                    numPieces[1] += 1;
                }
                else {
                    numPieces[7] += 1;
                }
                break;

            // Knight increment
            case 3:
                if(placement[0] == 1) {
                    numPieces[2] += 1;
                }
                else {
                    numPieces[8] += 1;
                }
                break;
            // Bishop increment
            case 4:
                if(placement[0] == 1) {
                    numPieces[3] += 1;
                }
                else {
                    numPieces[9] += 1;
                }
                break;
            // Queen increment
            case 5:
                if(placement[0] == 1) {
                    numPieces[4] += 1;
                }
                else {
                    numPieces[10] += 1;
                }
                break;
            // King increment
            case 6:
                if(placement[0] == 1) {
                    numPieces[5] += 1;
                }
                else {
                    numPieces[11] += 1;
                }
                break;
        }

        // Reset limit breach flag
        isLimitBreached = 0;

        // Check limit breach
        if(placement[0] != 0) {
            // TODO: This comparison is stupid complicated => refactor?
            if(numPieces[(placement[1] - 1) + (6 * (placement[0] - 1))] > pieceLimits[placement[1] - 1]) {
                printf("Invalid placement: Too many %ss, you have %d/%d.\n",
                       getRankStr(placement[1]),
                       numPieces[(placement[1] - 1) + (6 * (placement[0] - 1))] - 1,
                       pieceLimits[placement[1] - 1]);

                // Let user read error before moving on.
                promptReturnToContinue();

                // Decrement limit violator back to limit.
                numPieces[(placement[1] - 1) + (6 * (placement[0] - 1))] -= 1;

                // Set limit breached flag.
                isLimitBreached = 1;
            }

        }

        // If everything is fine, place chess piece.
        if(!isLimitBreached) {
            // Everything should be clear, so let's place the piece / clear square.
            chessboard[(8 * placement[3]) + placement[2]].owner = placement[0];
            chessboard[(8 * placement[3]) + placement[2]].rank = placement[1];
        }
    }

    // Free memory allocated to numPieces
    free(numPieces);
    // Set pointer to NULL so we don't risk double freeing this.
    numPieces = NULL;

    int userChoice = 0;
    printf("\n");

    while(userChoice == 0) {
        // User: save or not?
        userChoice = saveScenarioMenu();

        // Confirm no save
        if(userChoice == 0) {
            printf("\nThis scenario build will be gone for good.\n");
            userChoice = promptYesNo("Are you sure? ");
            if(userChoice == 1) {
                userChoice = 0;
                break;
            }
        }
        printf("\n");
    }

    // If save scenario
    if(userChoice > 0 && userChoice < 4) {
        save(chessboard, 1, userChoice);
    }

    // Finally, free memory allocated to chessboard.
    chessboard = freeChessboardMemory(chessboard);

}

void start(int showWelcome) {
    // Clear console before we get started.
    clearConsole();

    // Print welcome if showWelcome.
    if(showWelcome) {
        printInfo("welcome");
    }

    // Game state is where in the program we are (play game, exit, etc.)
    int gameState = 0;

    do {
        // Clear the console
        clearConsole();
        // Print menu
        gameState = menu();

        switch(gameState) {
            // Play game
            case 1:
                playGame(4);
                break;
            // Load game
            case 2:
                playGame(0);
                break;
            // Scenario builder
            case 3:
                scenarioEditor();
                break;
            // Information
            case 4:
                clearConsole();
                printInfo("information");
                break;

            default:
                printf("\nThank you for playing!\n\n");
        }

    } while(gameState != 0);

}

