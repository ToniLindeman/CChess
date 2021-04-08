/*
File:           ChessPiece.h
Author:         Toni Lindeman
Description:    Functionality for chess pieces.
*/

#ifndef CHESSPIECE_H
#define CHESSPIECE_H

struct chessPiece{
	int rank;
	int owner;
};

int validateAndMakeMove(struct chessPiece * chessboard, int selectedRow, int selectedColumn, int moveRow, int moveColumn,
        int player, int validateOnly, int checkingRun);
char * getRankStr(int rank);

#endif /* CHESSPIECE_H */
