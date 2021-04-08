/*
File:           ChessBoard.h
Author:         Toni Lindeman
Description:    Functionality for the chessboard.
*/

#ifndef CHESSBOARD_H
#define CHESSBOARD_H

void printChessboard(struct chessPiece * chessboard);
struct chessPiece * getInitChessboard();
struct chessPiece * getEmptyChessboard();
struct chessPiece * freeChessboardMemory(struct chessPiece * pointer);
int validateSelect(struct chessPiece * chessboard, int row, int column, int player);
int save(struct chessPiece * chessboard, int playerTurn, int saveTo);
int loadGameState(struct chessPiece * chessboard, int * outPlayerTurn, int loadFrom);
int countChessPieces(struct chessPiece * chessboard, int * countArray);
int checkForCheckedKing(struct chessPiece * chessboard, int player, int offset);
void copyChessboard(struct chessPiece * copyFrom, struct chessPiece * copyTo);
int checkForCheckmate(struct chessPiece * chessboard, int player);

#endif /* CHESSBOARD_H */
