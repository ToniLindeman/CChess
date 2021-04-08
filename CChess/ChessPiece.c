/*
File:           ChessPiece.c
Author:         Toni Lindeman
Description:    Functionality for chess pieces.
*/

#include <stdio.h>
#include <stdlib.h>		// Apparently abs prototype is in stdlib.
#include <math.h>
#include "UserInput.h"
#include "ChessPiece.h"

int validateAndMakeMove(struct chessPiece * chessboard, int selectedRow, int selectedColumn, int moveRow, int moveColumn,
        int player, int validateOnly, int checkingRun) {
    // Validates move and if valid, moves the piece.
    // validateOnly flag can be set, e.g. when using this function to check if king is checked.
    // checkingRun flag is set when move checking is done.

    // Selected piece and move square
    struct chessPiece selectedPiece = chessboard[(8 * selectedRow) + selectedColumn];
    struct chessPiece moveToPiece = chessboard[(8 * moveRow) + moveColumn];

    // Check that player is actually moving.
    if (selectedRow == moveRow && selectedColumn == moveColumn) {
        if(!validateOnly && !checkingRun) {
            printf("Move square equals selected square, please select another square to move to.\n");
        }

        return 0;
    }

    // Movement variables to help detect kind of motion
    int movementX = abs(selectedColumn - moveColumn);
    int movementY = abs(selectedRow - moveRow);

    // Some movement types are shared, ex. queen can move like a rook.
    // Use flags to avoid duplicate code.
    int checkRookTypeMovement = 0;
    int checkBishopTypeMovement = 0;

    // Pawn ------------------------------------------------------------------------------------------------------------
    if(selectedPiece.rank == 1) {

        // First move the pawn is allowed to move two steps forward.
        // Variable to let the vertical move condition know if two steps is allowed.
        int firstMoveModifier = 1;
        if((selectedPiece.owner == 1 && selectedRow == 1) || (selectedPiece.owner == 2 && selectedRow == 6)) {
            firstMoveModifier = 0;
        }

        // Check horizontal movement. Not yet checking if valid diagonal move.
        if(abs(selectedColumn - moveColumn) > 1) {
            if(!validateOnly && !checkingRun) {
                printf("Pawn moves normally straight forward, except when capturing "
                       "opponent.\n");
            }
            return 0;
        }

        // player 1 vertical check (pawns can only move forward so players must be checked separately).
        if(selectedPiece.owner == 1 && ((moveRow - selectedRow) < 1 || (moveRow - selectedRow) > 2 - firstMoveModifier)) {
            if(!validateOnly && !checkingRun) {
                printf("Pawn can move one step vertically (max 2 in the first move).\n");
            }
            return 0;
        }
        // player 2 vertical check
        if(selectedPiece.owner == 2 && ((selectedRow - moveRow) < 1 || (selectedRow - moveRow) > 2 - firstMoveModifier)) {
            if(!validateOnly && !checkingRun) {
                printf("Pawn can move one step vertically (max 2 in the first move).\n");
            }
            return 0;
        }

        // If diagonal move.
        if(abs(selectedColumn - moveColumn) == 1 && abs(selectedRow - moveRow) == 1) {
            // If empty, then prevent move.
            if(moveToPiece.rank == 0) {
                if(!validateOnly && !checkingRun) {
                    printf("Pawn can only move diagonally if capturing opponent.\n");
                }
                return 0;
            }
        }

        // Check if forward movement is blocked.
        if(abs(selectedColumn - moveColumn) == 0) {
            // moving one step
            if(abs(selectedRow - moveRow) == 1) {
                struct chessPiece squareAhead = chessboard[(8 * moveRow) + moveColumn];
                if(squareAhead.rank != 0) {
                    if(!validateOnly && !checkingRun) {
                        printf("Your move is blocked.\n");
                    }
                    return 0;
                }
            }
            // moving two step
            if(abs(selectedRow - moveRow) == 2) {
                int playerModifier = -1;
                if(player == 2) {
                    playerModifier = 1;
                }
                struct chessPiece squareTwoAhead = chessboard[(8 * moveRow) + moveColumn];
                struct chessPiece squareOneAhead = chessboard[(8 * (moveRow + playerModifier)) + moveColumn];
                if(squareTwoAhead.rank != 0 || squareOneAhead.rank != 0) {
                    if(!validateOnly && !checkingRun) {
                        printf("Your move is blocked.\n");
                    }
                    return 0;
                }
            }
        }
    }

    // Rook ------------------------------------------------------------------------------------------------------------
    else if(selectedPiece.rank == 2) {
        // Rooks move either vertically or horizontally.
        // They can move any distance as long as the path is not blocked by another chess piece.

        // Check that rook is moving only vertically or horizontally
        if(movementX && movementY) {
            if(!validateOnly && !checkingRun) {
                printf("Rook can only move straight vertically or horizontally.\n");
            }
            return 0;
        }

        // Set flag to check that path is clear.
        checkRookTypeMovement = 1;

    }

    // Knight ----------------------------------------------------------------------------------------------------------
    else if(selectedPiece.rank == 3) {
        // Knights can move (2v + 1h) or (1v + 2h)

        // First check that there is movement in both axis.
        if(movementX > 0 && movementY > 0) {
            // Then we only need to check if the sum is 3.
            if((movementX + movementY) != 3) {
                if(!validateOnly && !checkingRun) {
                    printf("Knight must move two vertical + one horizontal or vice versa.\n");
                }
                return 0;
            }
        }
        else {
            if(!validateOnly && !checkingRun) {
                printf("Knight must move two vertical + one horizontal or vice versa.\n");
            }
            return 0;
        }
    }

    // Bishop ----------------------------------------------------------------------------------------------------------
    else if(selectedPiece.rank == 4) {
        // Bishops can only move diagonally.

        // Movement in x and y must be equal.
        if(movementX != movementY) {
            if(!validateOnly && !checkingRun) {
                printf("Bishop can only move diagonally.\n");
            }
            return 0;
        }

        // Direction modifiers (bishops can move ne, nw, se and sw. So we need two direction modifiers).
        int directionX = 1;
        if(selectedColumn > moveColumn) {
            directionX = -1;
        }
        int directionY = 1;
        if(selectedRow > moveRow) {
            directionY = -1;
        }
        // Check path for blocks.
        for(int i = 1; i < movementX; i++) {
            // Using direction modifiers, find the next square on the path.
            if(chessboard[(8 * (selectedRow + (i * directionY))) + selectedColumn + (i * directionX)].rank != 0) {
                if(!validateOnly && !checkingRun) {
                    printf("Your move is blocked.\n");
                }
                return 0;
            }
        }
    }

    // Queen -----------------------------------------------------------------------------------------------------------
    else if(selectedPiece.rank == 5) {
        // The queen can move 0x + ny, nx + 0y and nx + ny. So basically a moveset of rook + bishop.

        // Check if rook type movement
        if( movementX == 0 || movementY == 0) {
            checkRookTypeMovement = 1;
        }

        // Check if bishop type movement
        else if(movementX == movementY) {
            checkBishopTypeMovement = 1;
        }

        else {
            if(!validateOnly && !checkingRun) {
                printf("Invalid move for a queen.\n");
                printf("Queens can move N, E, S, W, NE, NW, SE or SW.\n");
            }
            return 0;
        }

    }

    // King ------------------------------------------------------------------------------------------------------------
    else if(selectedPiece.rank == 6) {
        // Kings can move to any adjacent square (if not occupied by the player's own piece).
        // Here, we only need to check that x and y movement do not exceed 1.
        if(movementX > 1 || movementY > 1) {
            if(!validateOnly && !checkingRun) {
                printf("Kings can only move to an adjacent square.\n");
            }
            return 0;
        }
    }

    // Shared movement path checks -------------------------------------------------------------------------------------
    // Rook path check (rook and queen)
    if(checkRookTypeMovement) {
        // If vertical movement
        if(movementY) {
            // Check that path between select and move is clear.
            for(int y = 1; y < movementY; y++) {
                if(selectedRow < moveRow && chessboard[(8 * (selectedRow + y)) + selectedColumn].rank != 0) {
                    if(!validateOnly && !checkingRun) {
                        printf("Your move is blocked.\n");
                    }
                    return 0;
                }
                else if (selectedRow > moveRow && chessboard[(8 * (selectedRow - y)) + selectedColumn].rank != 0) {
                    if(!validateOnly && !checkingRun) {
                        printf("Your move is blocked.\n");
                    }
                    return 0;
                }
            }
        }
        // If horizontal movement
        else if(movementX) {
            // Check that path between select and move is clear.
            for(int x = 1; x < movementX; x++) {
                if(selectedColumn < moveColumn && chessboard[(8 * selectedRow) + selectedColumn + x].rank != 0) {
                    if(!validateOnly && !checkingRun) {
                        printf("Your move is blocked.\n");
                    }
                    return 0;
                }
                else if (selectedColumn > moveColumn && chessboard[(8 * selectedRow) + selectedColumn - x].rank != 0) {
                    if(!validateOnly && !checkingRun) {
                        printf("Your move is blocked.\n");
                    }
                    return 0;
                }
            }
        }
    }

    // Bishop path check (bishop and queen)
    if(checkBishopTypeMovement) {
        // Direction modifiers (bishops can move ne, nw, se and sw. So we need two direction modifiers).
        int directionX = 1;
        if(selectedColumn > moveColumn) {
            directionX = -1;
        }
        int directionY = 1;
        if(selectedRow > moveRow) {
            directionY = -1;
        }
        // Check path for blocks.
        for(int i = 1; i < movementX; i++) {
            // Using direction modifiers, find the next square on the path.
            if(chessboard[(8 * (selectedRow + (i * directionY))) + selectedColumn + (i * directionX)].rank != 0) {
                if(!validateOnly && !checkingRun) {
                    printf("Your move is blocked.\n");
                }
                return 0;
            }
        }
    }

    // Check that player is not moving on top of their own piece.
    if(moveToPiece.owner == player) {
        if(!validateOnly && !checkingRun) {
            printf("You cannot capture your own piece ;)\n");
        }
        return 0;
    }

    // Unless validateOnly flag is set.
    if(!validateOnly) {

        if(!checkingRun) {
            // If piece moving is a pawn, check if we can promote it.
            if(player == 1 && chessboard[(8 * selectedRow) + selectedColumn].rank == 1 && moveRow == 7) {
                // Player 1 can (and must) promote pawn.
                selectedPiece.rank = promptPromotePawn();

            }
            else if(player == 2 && chessboard[(8 * selectedRow) + selectedColumn].rank == 1 && moveRow == 0) {
                // Player 2 can (and must) promote pawn.
                selectedPiece.rank = promptPromotePawn();
            }
        }

        // All tests past, move piece.
        chessboard[(8 * moveRow) + moveColumn].rank = selectedPiece.rank;
        chessboard[(8 * moveRow) + moveColumn].owner = selectedPiece.owner;
        chessboard[(8 * selectedRow) + selectedColumn].rank = 0;
        chessboard[(8 * selectedRow) + selectedColumn].owner = 0;

    }

    return 1;
}

char * getRankStr(int rank) {
    // Get rank string from rank int.
    switch(rank) {
        case 1:
            return "pawn";
        case 2:
            return "rook";
        case 3:
            return "knight";
        case 4:
            return "bishop";
        case 5:
            return "queen";
        case 6:
            return "king";
        default:
            return "unknown rank";
    }

}

