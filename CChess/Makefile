CC = gcc
CFLAGS = -Wall -Wextra -std=c11

default: CChess

CChess: Main.o Gameplay.o UserInput.o Menu.o OSSpecific.o ChessPiece.o Chessboard.o
	$(CC) $(CFLAGS) -o CChess Main.o Gameplay.o UserInput.o Menu.o OSSpecific.o ChessPiece.o Chessboard.o -lm

Main.o: Main.c Gameplay.h
	$(CC) $(CFLAGS) -c Main.c

Gameplay.o: Gameplay.c Gameplay.h Menu.h OSSpecific.h UserInput.h ChessPiece.h Chessboard.h
	$(CC) $(CFLAGS) -c Gameplay.c

Menu.o: Menu.c Menu.h UserInput.h
	$(CC) $(CFLAGS) -c Menu.c

UserInput.o: UserInput.c UserInput.h
	$(CC) $(CFLAGS) -c UserInput.c

ChessPiece.o: ChessPiece.c ChessPiece.h UserInput.h
	$(CC) $(CFLAGS) -c ChessPiece.c

Chessboard.o: Chessboard.c Chessboard.h ChessPiece.h UserInput.h
	$(CC) $(CFLAGS) -c Chessboard.c

OSSpecific.o: OSSpecific.c OSSpecific.h
	$(CC) $(CFLAGS) -c OSSpecific.c

clean:
	$(RM) Exercise9_CChess *.o *-
