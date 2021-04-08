/*
File:           ChessBoard.c
Author:         Toni Lindeman
Description:    Functionality for the chessboard.

Chessboard definition:
In this program a chessboard is a 2D chessPiece array (8*8). Empty squares are simply struct chessPieces with
rank 0.
*/

#include <stdio.h>
#include <stdlib.h>
#include "ChessPiece.h"
#include "UserInput.h"

#define FILE_GAMESTATE "gamestate.gst"
#define FILE_SCENARIO1 "scenario1.scn"
#define FILE_SCENARIO2 "scenario2.scn"
#define FILE_SCENARIO3 "scenario3.scn"

void printBoardLine() {
    for(int column = 1; column <= 8; column++) {
        if(column == 1) {
            printf("-");
        }
        printf("-----");
    }
    printf("\n");
}

// MEMORY MANAGEMENT ---------------------------------------------------------------------------------------------------
struct chessPiece * freeChessboardMemory(struct chessPiece * pointer) {
    free(pointer);
    pointer = NULL;
    return pointer;
}

int * freeIntArrayMemory(int * pointer) {
    free(pointer);
    pointer = NULL;
    return pointer;
}

// DATA PERSISTENCE ----------------------------------------------------------------------------------------------------
int save(struct chessPiece * chessboard, int playerTurn, int saveTo) {
    // Save the contents of a given chessboard.
    // Two integers represent a square => rank and owner.
    // Save to: 0: gamestate, 1-3: scenario 1-3

    FILE * filePointer = NULL;
    if(saveTo == 0) {
        filePointer = fopen(FILE_GAMESTATE, "w");
    }
    else if(saveTo == 1) {
        filePointer = fopen(FILE_SCENARIO1, "w");
    }
    else if(saveTo == 2) {
        filePointer = fopen(FILE_SCENARIO2, "w");
    }
    else if(saveTo == 3) {
        filePointer = fopen(FILE_SCENARIO3, "w");
    }


    // If opening file succeeded.
    if(filePointer) {
        // First save player turn.
        fprintf(filePointer, "%d ", playerTurn);
        fprintf(filePointer, "\n");

        // Loop through each square in chessboard.
        for(int i = 0; i < 64; i++) {
            fprintf(filePointer, "%d %d ", chessboard[i].rank, chessboard[i].owner);
            // To make file more readable for human eyes, add new line for each chessboard row.
            if(i > 0 && (i + 1) % 8 == 0) {
                fprintf(filePointer, "\n");
            }
        }
        // Close file
        fclose(filePointer);

        printf("Game saved successfully.\n");

        // Return 1 for success
        return 1;
    }

    // Print fail message
    printf("Failed to save game state.\n");

    // Return 0 for failure
    return 0;
}

int loadGameState(struct chessPiece * chessboard, int * outPlayerTurn, int loadFrom) {
    FILE * filePointer = NULL;

    if(loadFrom == 0) {
        filePointer = fopen(FILE_GAMESTATE, "r");
    }
    else if(loadFrom == 1) {
        filePointer = fopen(FILE_SCENARIO1, "r");
    }
    else if(loadFrom == 2) {
        filePointer = fopen(FILE_SCENARIO2, "r");
    }
    else if(loadFrom == 3) {
        filePointer = fopen(FILE_SCENARIO3, "r");
    }

    // Check if opening file failed.
    if(!filePointer) {
        // Print fail message
        printf("Failed to open file.\n");

        // Return 0 for failure
        return 0;
    }

    int ranks[64];
    int owners[64];

    // variables to hold data, used to check that file doesn't contain invalid data.
    int rank = -1;
    int owner = -1;

    // counter to keep track of how many positions we've read in.
    int counter = 0;

    // First load player turn
    fscanf(filePointer, "%d ", outPlayerTurn);

    while (!feof(filePointer)) {
        // gamestate ends in newline, so we get one extra iteration before eof.
        // So we terminate the loop when we have all our items loaded.
        if(counter == 64) {
            break;
        }

        fscanf(filePointer, "%d %d", &rank, &owner);

        if((rank < 0 || rank > 6) && (owner < 0 || owner > 2)) {
            // Print fail message
            printf("Error: invalid data found in file.\n");

            // Close file
            fclose(filePointer);

            // Return 0 for failure
            return 0;
        }

        // Set value in arrays.
        ranks[counter] = rank;
        owners[counter] = owner;

        // increment square counter
        counter++;
    }

    if(counter < 64) {
        // Print fail message
        printf("Couldn't load game state, not enough items in game state file.\n");

        // Close file
        fclose(filePointer);

        // Return 0 for failure
        return 0;
    }

    // Close file
    fclose(filePointer);

    // Data was loaded successfully, now load the chessboard
    for(int i = 0; i < 64; i++) {
        chessboard[i].rank = ranks[i];
        chessboard[i].owner = owners[i];
    }

    // Return 1 for success
    return 1;

}

// HELPER FUNCTIONS ----------------------------------------------------------------------------------------------------
void findPlayerKing(struct chessPiece * chessboard, int player, int kingCoordinates[2]) {
    // If fail to find king, first element will be -1.
    kingCoordinates[0] = -1;
    kingCoordinates[0] = -1;

    for(int row = 0; row < 8; row++) {

        for(int column = 0; column < 8; column++) {

            if(chessboard[(8 * row) + column].owner == player && chessboard[(8 * row) + column].rank == 6) {
                kingCoordinates[0] = row;
                kingCoordinates[1] = column;
                return;
            }
        }
    }
}

void copyChessboard(struct chessPiece * copyFrom, struct chessPiece * copyTo) {
    // Copies contents of one chessboard to another.

    for(int row = 0; row < 8; row++) {
        for(int column = 0; column < 8; column++) {
            copyTo[(8 * row) + column] = copyFrom[(8 * row) + column];
        }
    }
}

void printChessboard(struct chessPiece * chessboard) {
    // If scenario editor is set -> print scenario editor info.

    // Print top letters
    for(int column = 1; column <= 8; column++) {
        if(column == 1) {
            printf(" ");
        }
        // Print ascii large letters starting from A
        printf(" %c   ", column + 64);
    }
    printf("\n");
    // Print top line
    printBoardLine();

    // Print chess squares
    for(int row = 0; row < 8; row++) {
        for(int column = 0; column < 8; column++) {
            // Leftmost boarder.
            if(column == 0) {
                printf("|");
            }
            // Empty
            if(chessboard[(8 * row) + column].owner == 0) {
                printf("    |");

            }
            // Pawn
            else if(chessboard[(8 * row) + column].rank == 1) {
                printf(" p%d |", chessboard[(8 * row) + column].owner);
            }
            // Rook
            else if(chessboard[(8 * row) + column].rank == 2) {
                printf(" R%d |", chessboard[(8 * row) + column].owner);
            }
            // Knight
            else if(chessboard[(8 * row) + column].rank == 3) {
                printf(" N%d |", chessboard[(8 * row) + column].owner);
            }
            // Bishop
            else if(chessboard[(8 * row) + column].rank == 4) {
                printf(" B%d |", chessboard[(8 * row) + column].owner);
            }
            // Queen
            else if(chessboard[(8 * row) + column].rank == 5) {
                printf(" Q%d |", chessboard[(8 * row) + column].owner);
            }
            // King
            else if(chessboard[(8 * row) + column].rank == 6) {
                printf(" K%d |", chessboard[(8 * row) + column].owner);
            }

            // Move option numerations
            else if(chessboard[(8 * row) + column].rank > 10) {
                printf(" %d  |", chessboard[(8 * row) + column].rank - 10);
            }

            else {
                printf("????|");
            }
            // Print row number
            if(column == 7) {
                printf("  %d", row + 1);
            }
        }

        printf("\n");
        printBoardLine();
    }
    printf("\n");
}

int * getAttackPathToKing(int attackerRow, int attackerColumn, int kingRow, int kingColumn, int * outLength) {

    // This function maps the path between an attacker and the king.
    // Valid only for rooks, bishops and queens.
    // The function assumes that the attacker really can reach the king.
    // Violating these guidelines results in undefined behavior!

    // Allocate max amount of memory needed for an attack path.
    // Max length is 6 squares * 2 (x and y)
    int * attackPath = NULL;
    attackPath = (int *) calloc(12, sizeof(int));

    // If memory allocation fails
    if(!attackPath) {
        *outLength = 0;
        return attackPath;
    }

    // Length of attack path.
    int pathLength = 0;

    // Directions for the attack
    int directionX = 0;
    int directionY = 0;

    if(attackerRow - kingRow < 0) {
        directionY = 1;
    }
    else if(attackerRow - kingRow > 0) {
        directionY = -1;
    }

    if(attackerColumn - kingColumn < 0) {
        directionX = 1;
    }
    else if(attackerColumn - kingColumn > 0) {
        directionX = -1;
    }

    // Variables to hold current position on attack path. Initialized to attacker position
    int positionX = attackerColumn;
    int positionY = attackerRow;

    // While position is not king position + 1 direction.
    // If attacker is adjacent to king, this loop will not run at all.
    while((positionX + directionX) != kingColumn || (positionY + directionY) != kingRow) {
        // Next position
        positionX += directionX;
        positionY += directionY;

        // Set new position in path
        attackPath[pathLength * 2] = positionY;
        attackPath[(pathLength * 2) + 1] = positionX;

        // Increment length
        pathLength++;
    }

    // Reallocate memory if necessary
    if(pathLength < 6 && pathLength != 0) {
        // Shrink the allocated memory to what we need.
        int * tempPointer = NULL;
        tempPointer = realloc(attackPath, 2 * pathLength * sizeof(int));

        // If memory reallocation succeeds.
        if(tempPointer) {
            // Realloc should have handled freeing the memory, so we can set attackPath = tempPointer.
            // Caller must handle freeing the memory when it's no longer needed.
            attackPath = tempPointer;
        }
    }
    // If path is 0, just free the memory.
    else if(pathLength == 0) {
        attackPath = freeIntArrayMemory(attackPath);
    }

    // Set out length for caller
    *outLength = pathLength;

    return attackPath;
}

int findAttackerCoordinates(struct chessPiece * chessboard, int player, int targetRow, int targetColumn,
                            int * outAttackerRow, int * outAttackerColumn) {

    // Gives coordinates for a chess piece attacking a given target.
    // Note that if several pieces attack the target, only the first found is given.
    // Check if any opponent can reach king with move validation.

    int opponent = (player % 2) + 1;

    for(int row = 0; row < 8; row++) {
        for(int column = 0; column < 8; column++) {

            // If current square contains opponent piece
            if(chessboard[(8 * row) + column].owner == opponent) {
                // Try moving it to the target.
                if(validateAndMakeMove(chessboard, row, column, targetRow, targetColumn, opponent, 1, 1)) {
                    // Attacker found, set coordinates.
                    *outAttackerRow = row;
                    *outAttackerColumn = column;

                    return 1;
                }
            }
        }
    }

    // No attacker found
    return 0;
}

int * getPlayerChessPieces(struct chessPiece * chessboard, int player, int includeKing, int * outCount) {
    // Get all player pieces

    // Allocate maximum amount of memory we might need (16 pieces * 2 (x and y)).
    int * pieceCoordinates = NULL;
    pieceCoordinates = (int *) calloc(32, sizeof(int));

    // If allocating memory fails.
    if(!pieceCoordinates) {
        *outCount = 0;
        return pieceCoordinates;
    }

    // Counter for actual size needed
    int pieceCount = 0;

    // Find all player chess pieces.
    for(int row = 0; row < 8; row++) {

        for(int column = 0; column < 8; column++) {

            if(chessboard[(8 * row) + column].owner == player) {

                // If chess piece is king, add it only to the array if includeKing flag is set.
                if(chessboard[(8 * row) + column].rank == 6 && includeKing) {
                    pieceCoordinates[pieceCount * 2] = row;
                    pieceCoordinates[(pieceCount * 2) + 1] = column;
                    pieceCount++;
                }
                else if(chessboard[(8 * row) + column].rank != 6) {
                    pieceCoordinates[pieceCount * 2] = row;
                    pieceCoordinates[(pieceCount * 2) + 1] = column;
                    pieceCount++;
                }
            }
        }
    }
    // If no pieces were found, free the memory.
    if(pieceCount == 0) {
        pieceCoordinates = freeIntArrayMemory(pieceCoordinates);
    }

    // If pieceCount is less than max number of pieces.
    else if(pieceCount < 16) {
        // Shrink the allocated memory to what we need.
        int * tempPointer = NULL;
        tempPointer = realloc(pieceCoordinates, 2 * pieceCount * sizeof(int));

        // If memory reallocation succeeds.
        if(tempPointer) {
            // Realloc should have handled freeing the memory, so we can set pieceCoordinates = tempPointer.
            pieceCoordinates = tempPointer;
        }

        // If reallocating memory fails we are not facing a disaster, since outCount tells the caller
        // how many coordinates the array holds. We are wasting some memory but that's an acceptable cost.
        // Optionally we could: retry realloc or raise an exception.
    }

    // Set the piece count
    *outCount = pieceCount;

    return pieceCoordinates;

}

void countChessPieces(struct chessPiece * chessboard, int * countArray) {
    // Counts number of chess pieces on a chessboard.

    // Count array format:
    // arr[0-5] player 1 pawn - king
    // arr[6-11] player 2 pawn - king

    for(int row = 0; row < 8; row++) {
        for(int column = 0; column < 8; column++) {
            switch(chessboard[(8 * column) + row].rank) {
                // Pawn
                case 1:
                    // This messy indexing handles both player 1 and 2 cases.
                    // So we don't need another switch for owner.
                    countArray[(0 + (6 * (chessboard[(8 * column) + row].owner - 1)))]++;
                    break;

                    // Rook
                case 2:
                    countArray[(1 + (6 * (chessboard[(8 * column) + row].owner - 1)))]++;
                    break;
                    // Knight
                case 3:
                    countArray[(2 + (6 * (chessboard[(8 * column) + row].owner - 1)))]++;
                    break;
                    // Bishop
                case 4:
                    countArray[(3 + (6 * (chessboard[(8 * column) + row].owner - 1)))]++;
                    break;
                    // Queen
                case 5:
                    countArray[(4 + (6 * (chessboard[(8 * column) + row].owner - 1)))]++;
                    break;
                    // King
                case 6:
                    countArray[(5 + (6 * (chessboard[(8 * column) + row].owner - 1)))]++;
                    break;
            }
        }
    }


}

// CHESSBOARD MAKERS ---------------------------------------------------------------------------------------------------
struct chessPiece * getInitChessboard() {
    // Get a chessboard with standard setup.
    struct chessPiece * chessboard = NULL;

    chessboard = (struct chessPiece *) malloc(64 * sizeof(struct chessPiece));

    // If memory allocation worked
    if(chessboard) {
        // Fill chessboard
        for(int i = 0; i < 64; i++) {
            switch(i) {
                // Rook
                case 0:
                case 7:
                    (chessboard + i)->owner = 1;
                    (chessboard + i)->rank = 2;
                    break;

                case 56:
                case 63:
                    (chessboard + i)->owner = 2;
                    (chessboard + i)->rank = 2;
                    break;

                // Knight
                case 1:
                case 6:
                    (chessboard + i)->owner = 1;
                    (chessboard + i)->rank = 3;
                    break;

                case 57:
                case 62:
                    (chessboard + i)->owner = 2;
                    (chessboard + i)->rank = 3;
                    break;

                // Bishop
                case 2:
                case 5:
                    (chessboard + i)->owner = 1;
                    (chessboard + i)->rank = 4;
                    break;

                case 58:
                case 61:
                    (chessboard + i)->owner = 2;
                    (chessboard + i)->rank = 4;
                    break;

                // Queen
                case 3:
                    (chessboard + i)->owner = 1;
                    (chessboard + i)->rank = 5;
                    break;

                case 59:
                    (chessboard + i)->owner = 2;
                    (chessboard + i)->rank = 5;
                    break;
                // King
                case 4:
                    (chessboard + i)->owner = 1;
                    (chessboard + i)->rank = 6;
                    break;

                case 60:
                    (chessboard + i)->owner = 2;
                    (chessboard + i)->rank = 6;
                    break;

                // Pawn
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                    (chessboard + i)->owner = 1;
                    (chessboard + i)->rank = 1;
                    break;

                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                    (chessboard + i)->owner = 2;
                    (chessboard + i)->rank = 1;
                    break;

                default:
                    (chessboard + i)->owner = 0;
                    (chessboard + i)->rank = 0;

            }
        }
    }
    else {
        printf("Failed to allocate memory to chessboard!\n");
    }

    return chessboard;
}

struct chessPiece * getEmptyChessboard() {
    // Get empty chessboard for scenario builder
    // Get a chessboard with standard setup.
    struct chessPiece * chessboard = NULL;

    chessboard = (struct chessPiece *) malloc(64 * sizeof(struct chessPiece));

    // If memory allocation worked
    if(chessboard) {
        // Fill chessboard with empty squares
        for(int i = 0; i < 64; i++) {
            (chessboard + i) -> owner = 0;
            (chessboard + i) -> rank = 0;
        }
    }
    else {
        printf("Failed to allocate memory to chessboard!\n");
    }

    return chessboard;

}

// VALIDATION AND CHECKING ---------------------------------------------------------------------------------------------
int validateSelect(struct chessPiece * chessboard, int row, int column, int player) {
    /*
     * This function validates the following:
     *      - Chess piece belongs to player.
     *      - Selected square is not empty.
     *
     * */

    struct chessPiece selectedSquare = chessboard[(8 * row) + column];

    // Player tries to move empty square.
    if(selectedSquare.rank == 0) {
        printf("You cannot move an empty square!\n");
        return 0;
    }

    // Player tries to move the other player's piece.
    else if(selectedSquare.owner != player) {
        printf("That chess piece does not belong to you!\n");
        return 0;
    }

    return 1;

}

int checkForCheckedKing(struct chessPiece * chessboard, int player, int offset) {
    // Check if a player's king checked, returns 1 if king is checked.
    // We can use move validation to recognize if king is checked.
    // offset can be used if you want to know if the king is checked by more than 1 opponent piece.

    int opponent = (player % 2) + 1;
    int kingX = -1;
    int kingY = -1;

    // Find player king location
    for(int row = 0; row < 8; row++) {

        for(int column = 0; column < 8; column++) {

            // If current square is player king
            if(chessboard[(8 * row) + column].owner == player && chessboard[(8 * row) + column].rank == 6) {
                kingX = column;
                kingY = row;
                break;
            }
        }
        // When king is found, jump out of row-loop.
        if(kingX != -1) {
            break;
        }
    }

    int checkCounter = 0;

    // Check if any opponent can reach king with move validation.
    for(int row = 0; row < 8; row++) {
        for(int column = 0; column < 8; column++) {

            // If current square contains opponent piece
            if(chessboard[(8 * row) + column].owner == opponent) {
                // Try moving it to the king.
                if(validateAndMakeMove(chessboard, row, column, kingY, kingX, opponent, 1, 1)) {
                    // if offset value is reached and king still shows as checked.
                    if(checkCounter == offset) {
                        // King is checked
                        return 1;
                    }
                    else {
                        checkCounter++;
                    }
                }
            }
        }
    }

    // king is not checked
    return 0;

}

int checkForCheckmate(struct chessPiece * chessboard, int player) {

    // Chessboard to be used in checking.
    struct chessPiece * chessboardTemp = NULL;
    chessboardTemp = getEmptyChessboard();

    if(!chessboardTemp) {
        printf("checkForCheckmate error: failed to make temp chessboard.\n");
        return -1;
    }

    // Copy given chessboard to temp.
    copyChessboard(chessboard, chessboardTemp);

    // Find king
    int kingCoordinates[2] = {-1, -1};
    findPlayerKing(chessboard, player, kingCoordinates);



    // Check that king was found (logically king should always be present on chessboard).
    if(kingCoordinates[0] == -1) {
        printf("checkForCheckmate error: Failed to find king.\n");
    }

    // First check if king moving can resolve the check

    // Now let's try moving the king to escape the check.
    // King can move to any adjacent square.
    for(int x = -1; x <= 1; x++) {

        for(int y = -1; y <= 1; y++) {
            // 0 0 means no move, so skip that one.
            if(x == 0 && y == 0) {
                continue;
            }

            // Check that we aren't moving beyond board boundaries.
            if((kingCoordinates[0] + y) >= 0 && (kingCoordinates[0] + y) <= 7 &&
                (kingCoordinates[1] + x) >= 0 && (kingCoordinates[1] + x) <= 7) {

                // Check if move is valid and make move if it is.
                if(validateAndMakeMove(chessboardTemp, kingCoordinates[0], kingCoordinates[1],
                                       kingCoordinates[0] + y, kingCoordinates[1] + x, player, 0, 1)) {

                    // If king no longer is checked, we are not dealing with a checkmate.
                    if(!checkForCheckedKing(chessboardTemp, player, 0)) {
                        // Not checkmate
                        // Free allocated memory and return 0
                        chessboardTemp = freeChessboardMemory(chessboardTemp);
                        return 0;
                    }
                    else {
                        // else copy original state if we happened to capture a chess piece.
                        copyChessboard(chessboard, chessboardTemp);
                    }
                }
            }
        }
    }

    // If king cannot resolve the check AND more than one opponent piece is checking the king, we have a checkmate.
    // If only one opponent is checking the king we keep checking.
    if(!checkForCheckedKing(chessboardTemp, player, 1)) {
        // Find attacker
        int attackerRow = -1;
        int attackerColumn = -1;

        // Check that we find an attacker, if not we have a bug in the system.
        if(!findAttackerCoordinates(chessboardTemp, player, kingCoordinates[0], kingCoordinates[1],
                &attackerRow, &attackerColumn)) {
            printf("checkForCheckmate: Single attacker check failed.\n");
            printf("Failed to find attacker.\n");
            promptReturnToContinue();
            // Free memory allocated to chessboard
            chessboardTemp = freeChessboardMemory(chessboardTemp);
            return -1;
        }

        // Check if the attacker can be captured.
        int * playerPieces = NULL;
        int pieceCount = 0;
        playerPieces = getPlayerChessPieces(chessboardTemp, player, 0, &pieceCount);

        // Check that memory allocation didn't fail.
        // If pieceCount is 0, let's assume that the player only has a king left.
        if(!playerPieces && pieceCount != 0) {
            printf("checkForCheckmate: Single attacker check failed.\n");
            printf("Failed to allocate memory for player pieces.\n");
            promptReturnToContinue();
            // Free memory allocated to chessboard
            chessboardTemp = freeChessboardMemory(chessboardTemp);
            return -1;
        }

        // For each piece try to capture the attacker
        for(int i = 0; i < pieceCount; i++) {

            if(validateAndMakeMove(chessboardTemp,
                    playerPieces[i * 2], playerPieces[(i * 2) + 1],
                    attackerRow, attackerColumn, player, 1, 1)) {

                // Free memory allocated to player pieces
                playerPieces = freeIntArrayMemory(playerPieces);
                // Free memory allocated to chessboard
                chessboardTemp = freeChessboardMemory(chessboardTemp);

                // Capture succeeded, player can save the king.
                return 0;
            }
        }

        /* So far we know that:
         * The king cannot resolve the check.
         * The attacker cannot be captured.
         *
         * We still need to check that no piece can get in the middle of the attacker and the king.
         * For this we only need to check rooks, bishops and queens.
         * Note that if at this point the attacker is a knight, we have a checkmate.
         * If the attacker is a single pawn, all adjacent squares must be under attack by other pieces (else the king
         * or other capture attempt could have resolved this check).
        */

        // First check that opponent is not a knight or pawn
        if(chessboardTemp[(8 * attackerRow) + attackerColumn].rank != 1 &&
            chessboardTemp[(8 * attackerRow) + attackerColumn].rank != 3) {

            // Now we need the attack path
            int attackPathLength = 0;
            int * attackPath = NULL;
            attackPath = getAttackPathToKing(attackerRow, attackerColumn,
                    kingCoordinates[0], kingCoordinates[1], &attackPathLength);

            // If allocating memory failed.
            if(!attackPath && attackPathLength != 0) {
                printf("checkForCheckmate: Single attacker check failed.\n");
                printf("Failed to allocate memory for attack path.\n");
                promptReturnToContinue();
                // Free allocated memory
                playerPieces = freeIntArrayMemory(playerPieces);
                chessboardTemp = freeChessboardMemory(chessboardTemp);
                return -1;
            }

            // Now let's validate some moves.
            // The function is slightly brute forcing here.
            // TODO: See if this following part can be made more efficient.

            // For each player piece.
            for(int piece = 0; piece < pieceCount; piece++) {
                // For each square in attack path
                for(int square = 0; square < attackPathLength; square++) {
                    // Check if piece can
                    if(validateAndMakeMove(chessboardTemp,
                                           playerPieces[piece * 2], playerPieces[(piece * 2) + 1],
                                           attackPath[square * 2], attackPath[(square * 2) + 1],
                                           player, 1, 1)) {

                        // Free memory allocated to attack path
                        attackPath = freeIntArrayMemory(attackPath);
                        // Free memory allocated to player pieces
                        playerPieces = freeIntArrayMemory(playerPieces);
                        // Free memory allocated to chessboard
                        chessboardTemp = freeChessboardMemory(chessboardTemp);

                        // Interrupting attack path succeeded!
                        return 0;
                    }
                }
            }

            // Free memory allocated to attack path
            attackPath = freeIntArrayMemory(attackPath);
        }

        // Free memory allocated to player pieces
        playerPieces = freeIntArrayMemory(playerPieces);
    }


    // Free allocated memory
    chessboardTemp = freeChessboardMemory(chessboardTemp);

    // No move was found, we are in a checkmate
    return 1;

}

