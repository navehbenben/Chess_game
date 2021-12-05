#ifndef __ASS4_H
#define __ASS4_H

void createBoard(char board[][SIZE], char fen[]);
void printBoard(char board[][SIZE]);
int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn);

//copy from public code
int toDigit(char c);
void printColumns();
void printSpacers();

//public code
// void printRow(char row[], int rowIdx);
void printBoardFromFEN(char fen[]);

#endif
