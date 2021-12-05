#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include "ass4.h"

// PGN characters
#define PAWN 'P'
#define ROOK 'R'
#define KNIGHT 'N'
#define BISHOP 'B'
#define QUEEN 'Q'
#define KING 'K'

// Move logical representation
typedef struct {
    char srcPiece, srcRow, srcCol, destPiece, destRow, destCol, promotionPiece;
    int iSrc, jSrc, iDest, jDest;
    int isWhite, isCapture, isPromotion, isCheck, isMate, isLegal;
} Move;

const char UNDEFINED = '\0';
const char FIRST_COL = 'a';

// FEN & Board characters
const char WHITE_PAWN = 'P';
const char WHITE_ROOK = 'R';
const char WHITE_KNIGHT = 'N';
const char WHITE_BISHOP = 'B';
const char WHITE_QUEEN = 'Q';
const char WHITE_KING = 'K';

const char BLACK_PAWN = 'p';
const char BLACK_ROOK = 'r';
const char BLACK_KNIGHT = 'n';
const char BLACK_BISHOP = 'b';
const char BLACK_QUEEN = 'q';
const char BLACK_KING = 'k';

// FEN separator for strtok()
const char SEP[] = "/";

// Board characters
const char EMPTY = ' ';

void initMove(Move* move) {
    move->isWhite = -1;
    move->isCapture = 0; // default value
    move->isPromotion = 0; // default value
    move->isCheck = 0; // default value
    move->isMate = 0; // default value
    move->isLegal = -1;
    move->srcPiece = UNDEFINED;
    move->srcRow = UNDEFINED;
    move->srcCol = UNDEFINED;
    move->destPiece = UNDEFINED;
    move->destCol = UNDEFINED;
    move->destRow = UNDEFINED;
    move->promotionPiece = UNDEFINED;
    move->iSrc = -1;
    move->jSrc = -1;
    move->iDest = -1;
    move->jDest = -1;
}

void printMove(Move move) {
    if (move.isWhite) {
        printf("White\n");
    } else {
        printf("Black\n");
    }
    printf("Src piece: %c\n", move.srcPiece);
    printf("Src: %c%c (%d,%d)\n", move.srcCol, move.srcRow, move.iSrc, move.jSrc);
    printf("Capture: %d\n", move.isCapture);
    printf("Dest piece: %c\n", move.destPiece);
    printf("Dest: %c%c (%d,%d)\n", move.destCol, move.destRow, move.iDest, move.jDest);
    printf("Promotion: %d (%c)\n", move.isPromotion, move.promotionPiece);
    printf("Check: %d\n", move.isCheck);
    printf("Mate: %d\n", move.isMate);
}

int isPiece(char c) {
    return c == KING || c == QUEEN || c == BISHOP || c == KNIGHT || c == ROOK;
}

int isBlack(char c) {
    return c == BLACK_KING || c == BLACK_QUEEN || c == BLACK_BISHOP || c == BLACK_KNIGHT || c == BLACK_ROOK || c == BLACK_PAWN;
}

int isWhite(char c) {
    return c == WHITE_KING || c == WHITE_QUEEN || c == WHITE_BISHOP || c == WHITE_KNIGHT || c == WHITE_ROOK || c == WHITE_PAWN;
}

int toDigit(char c) {
    assert('0' <= c && c <= '9');
    return c - '0';
}

char toChar(int d) {
    assert(0 <= d && d <= 9);
    return d + '0';
}

int isColumn(char c) {
    char colIdx = c - FIRST_COL;
    return 0 <= colIdx && colIdx < SIZE;
}

int colToIdx(char col) {
    assert(isColumn(col));
    // char maxCol = FIRST_COL + SIZE - 1;
    // assert(FIRST_COL <= col && col <= maxCol);
    return col - FIRST_COL;
}

char idxToCol(int index) {
    assert(0 <= index && index < SIZE);
    return index + 'a';
}

int rowToIdx(int row) {
    assert(0 < row && row <= SIZE);
    return SIZE - row;
}

int idxToRow(int index) {
    assert(0 <= index && index < SIZE);
    return SIZE - index;
}

void printColumns() {
    char column = toupper(FIRST_COL);
    printf("* |");
    for (int i = 0; i < SIZE; i++) {
        if (i) {
            printf(" ");
        }
        printf("%c", column);
        column++;
    }
    printf("| *\n");
}

void printSpacers() {
    printf("* -");
    for (int i = 0; i < SIZE; i++) {
        printf("--");
    }
    printf(" *\n");
}

void createRow(char row[], char fenRow[]) {
    int rowIdx = 0;
    for (int i = 0; fenRow[i]; i++) {
        if (isdigit(fenRow[i])) {
            int spaces = toDigit(fenRow[i]);
            for (int j = 0; j < spaces; j++) {
                row[rowIdx++] = EMPTY;
            }
        } else {
            row[rowIdx++] = fenRow[i];
        }
    }
}

void createBoard(char board[][SIZE], char fen[]) {
    int i = 0;
    char* fenRow = strtok(fen, SEP);
    while (fenRow != NULL) {
        createRow(board[i++] ,fenRow);
        fenRow = strtok(NULL, SEP);
    }
}

void printBoard(char board[][SIZE]) {
    printColumns();
    printSpacers();
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", SIZE - i);
        for (int j = 0; j < SIZE; j++) {
            printf("|%c", board[i][j]);
        }
        printf("| %d\n", SIZE - i);
    }
    printSpacers();
    printColumns();
}

void parsePgn(char board[][SIZE], char pgn[], Move* move) {
    const char CAPTURE = 'x';
    const char PROMOTION = '=';
    const char CHECK = '+';
    const char MATE = '#';

    int moveLength = strlen(pgn);
    assert(moveLength > 0);

    if (isPiece(pgn[0])) {
        move->srcPiece = pgn[0];
    } else {
        move->srcPiece = PAWN;
    }

    if (!move->isWhite) {
        move->srcPiece = tolower(move->srcPiece);
    }

    int seenDigit = 0;
    int seenColumn = 0;
    for (int i = moveLength-1; i >= 0; i--) {
        if (isdigit(pgn[i]) && !seenDigit) {
            move->destRow = pgn[i];
            seenDigit = 1;
        } else if (isColumn(pgn[i]) && !seenColumn) {
            move->destCol = pgn[i];
            seenColumn = 1;
        } else if (isdigit(pgn[i])) {
            move->srcRow = pgn[i];
        } else if (isColumn(pgn[i])) {
            move->srcCol = pgn[i];
        } else if (pgn[i] == CAPTURE) {
            move->isCapture = 1;
        } else if (pgn[i] == PROMOTION) {
            move->isPromotion = 1;
            assert(i < moveLength-1);
            move->promotionPiece = move->isWhite ? pgn[i+1] : tolower(pgn[i+1]);
        } else if (pgn[i] == CHECK) {
            move->isCheck = 1;
        } else if (pgn[i] == MATE) {
            move->isMate = 1;
        }
    }

    assert(move->destCol != UNDEFINED && move->destCol != UNDEFINED);

    move->jDest = colToIdx(move->destCol);
    move->iDest = rowToIdx(toDigit(move->destRow));
    assert(0 <= move->iDest && move->iDest < SIZE && 0 <= move->jDest && move->jDest < SIZE);
    move->destPiece = board[move->iDest][move->jDest];
}

int canMoveLikePawn(const Move* move) {
    int iDiff = move->iDest - move->iSrc;
    int isSecondRowForBlack = move->iSrc == rowToIdx(SIZE-1);
    int isSecondRowForWhite = move->iSrc == rowToIdx(2);
    int isRowLegal = move->isWhite && isSecondRowForWhite && (iDiff == -2 || iDiff == -1) ||
                     move->isWhite && iDiff == -1 ||
                     !move->isWhite && isSecondRowForBlack && (iDiff == 2 || iDiff == 1) ||
                     !move->isWhite && iDiff == 1;
    int jDiff = abs(move->jDest - move->jSrc);
    int isColLegal = move->isCapture && jDiff == 1 || !move->isCapture && jDiff == 0;
    return isRowLegal && isColLegal;
}

int canMoveLikeKing(const Move* move) {
    return (abs(move->iDest - move->iSrc) == 1 && abs(move->jDest - move->jSrc) == 1) ||
            abs(move->iDest - move->iSrc) == 1 && move->jDest == move->jSrc ||
            abs(move->jDest - move->jSrc) == 1 && move->iDest == move->iSrc;
}

int canMoveLikeQueen(const Move* move) {
    return move->iDest == move->iSrc || move->jDest == move->jSrc ||
           abs(move->iDest - move->iSrc) == abs(move->jDest - move->jSrc);
}

int canMoveLikeBishop(const Move* move) {
    return abs(move->iDest - move->iSrc) == abs(move->jDest - move->jSrc);
}

int canMoveLikeKnight(const Move* move) {
    return (abs(move->iDest - move->iSrc) == 1 && abs(move->jDest - move->jSrc) == 2) ||
           (abs(move->iDest - move->iSrc) == 2 && abs(move->jDest - move->jSrc) == 1);
}

int canMoveLikeRook(const Move* move) {
    return move->iDest == move->iSrc || move->jDest == move->jSrc;
}

int canMoveLikeThis(const Move* move) {
    assert(0 <= move->iDest && move->iDest < SIZE && 0 <= move->jDest && move->jDest < SIZE);
    char piece = toupper(move->srcPiece);
    switch (piece) {
        case KING:      return canMoveLikeKing(move);
        case QUEEN:     return canMoveLikeQueen(move);
        case BISHOP:    return canMoveLikeBishop(move);
        case KNIGHT:    return canMoveLikeKnight(move);
        case ROOK:      return canMoveLikeRook(move);
        case PAWN:      return canMoveLikePawn(move);
    }
}

int isDiagonalClear(char board[][SIZE], int iFrom, int jFrom, int iTo, int jTo) {
    // not a diagonal
    if (abs(iTo - iFrom) != abs(jTo - jFrom)) {
        return 0;
    }

    int iSign = iTo > iFrom ? 1 : -1;
    int jSign = jTo > jFrom ? 1 : -1;
    int distance = abs(iTo - iFrom);
    for (int d = 1; d < distance; d++) {
        int i = iFrom + iSign * d;
        int j = jFrom + jSign * d;
        if (board[i][j] != EMPTY) {
            return 0;
        }
    }
    return 1;
}

int isLineClear(char board[][SIZE], int iFrom, int jFrom, int iTo, int jTo) {
    // not a line
    if (!((iFrom != iTo && jFrom == jTo) || (iFrom == iTo && jFrom != jTo))) {
        return 0;
    }

    int iMin = iFrom < iTo ? iFrom : iTo;
    int iMax = iFrom > iTo ? iFrom : iTo;
    int jMin = jFrom < jTo ? jFrom : jTo;
    int jMax = jFrom > jTo ? jFrom : jTo;

    if (iMin == iMax) {
        for (int j = jMin+1; j < jMax; j++) {
            if (board[iMin][j] != EMPTY) {
                return 0;
            }
        }
    } else if (jMin == jMax) {
        for (int i = iMin+1; i < iMax; i++) {
            if (board[i][jMin] != EMPTY) {
                return 0;
            }
        }
    }
    return 1;
}

int isClearPath(char board[][SIZE], const Move* move) {
    char piece = toupper(move->srcPiece);
    int iFrom = move->iSrc;
    int jFrom = move->jSrc;
    int iTo = move->iDest;
    int jTo = move->jDest;
    assert(0 <= iTo && iTo < SIZE && 0 <= jTo && jTo < SIZE);

    piece = toupper(piece);

    // TODO: add case for pawn-capture? (for example "exe6") -> looks like no need...
    // ADDED! due to this test case: "e4+" for this: 8/8/8/8/5k2/8/PPPPPPPP/7K (which is illegal move).
    /*
    simple explanation: without this, a pawn could threat the king with a regular move.
    simple example: f3+ for this 8/8/8/8/5k2/8/PPPPPPPP/7K would be legal.
    */
    if (piece == PAWN && move->isCapture) {
        return isDiagonalClear(board, iFrom, jFrom, iTo, jTo);
    }

    if (piece == PAWN && !move->isCapture) {
        return isLineClear(board, iFrom, jFrom, iTo, jTo);
    }

    if (piece == KING || piece == KNIGHT) {
        return 1;
    }
    if (piece == QUEEN) {
        return isDiagonalClear(board, iFrom, jFrom, iTo, jTo) || isLineClear(board, iFrom, jFrom, iTo, jTo);
    }
    if (piece == BISHOP) {
        return isDiagonalClear(board, iFrom, jFrom, iTo, jTo);
    }
    if (piece == ROOK) {
        return isLineClear(board, iFrom, jFrom, iTo, jTo);
    }
}

int cloneBoard(char src[][SIZE], char dest[][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

int isCheck(char board[][SIZE], int isKingWhite) {
    char king = isKingWhite ? WHITE_KING : BLACK_KING;
    int iKing, jKing;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == king) {
                iKing = i;
                jKing = j;
                break;
            }
        }
    }
    Move move;
    move.iDest = iKing;
    move.jDest = jKing;
    move.isWhite = !isKingWhite;
    move.isCapture = 1;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char piece = board[i][j];
            if ((isKingWhite && isBlack(piece)) || (!isKingWhite && isWhite(piece))) {
                move.srcPiece = piece;
                move.iSrc = i;
                move.jSrc = j;
                if (canMoveLikeThis(&move) && isClearPath(board, &move)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void performMove(char board[][SIZE], const Move* move);

void inferSrc(char board[][SIZE], char pgn[], Move* move) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (move->srcRow != UNDEFINED && rowToIdx(move->srcRow) != i) {
                continue;
            }
            if (move->srcCol != UNDEFINED && colToIdx(move->srcCol) != j) {
                continue;
            }
            if (board[i][j] != move->srcPiece) {
                continue;
            }
            move->iSrc = i;
            move->jSrc = j;
            if (!canMoveLikeThis(move)) {
                continue;
            }
            if (!isClearPath(board, move)) {
                continue;
            }
            char tmp[SIZE][SIZE];
            cloneBoard(board, tmp);
            performMove(tmp, move);
            if (isCheck(tmp, move->isWhite)) {
                continue;
            }
            int isOppCheck = isCheck(tmp, !move->isWhite);
            if (!isOppCheck && (move->isCheck || move->isMate)) {
                continue;
            }
            if (isOppCheck && !(move->isCheck || move->isMate)) {
                continue;
            }
            move->srcCol = idxToCol(move->jSrc);
            move->srcRow = toChar(idxToRow(move->iSrc));
            move->isLegal = 1;
            return;
        }
    }
    move->isLegal = 0;
}

Move parseMove(char board[][SIZE], char pgn[], int isWhiteTurn) {
    Move move;
    initMove(&move);
    move.isWhite = isWhiteTurn;
    parsePgn(board, pgn, &move);
    if (move.isWhite && isWhite(move.destPiece) || !move.isWhite && isBlack(move.destPiece) ||
        move.isCapture && move.destPiece == EMPTY || !move.isCapture && move.destPiece != EMPTY) {
        move.isLegal = 0;
        return move;
    }
    inferSrc(board, pgn, &move);
    return move;
}

void performMove(char board[][SIZE], const Move* move) {
    board[move->iDest][move->jDest] = move->isPromotion ? move->promotionPiece : move->srcPiece;
    board[move->iSrc][move->jSrc] = EMPTY;
}

int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn) {
    Move move = parseMove(board, pgn, isWhiteTurn);
    if (move.isLegal) {
        performMove(board, &move);
        return 1;
    } else {
        // printMove(move);
        return 0;
    }
}
