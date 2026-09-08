#ifndef TETRIS_H
#define TETRIS_H

typedef enum {
    EMPTY = 0,
    PIECE_I, PIECE_O, PIECE_T, PIECE_S, PIECE_Z, PIECE_J, PIECE_L
} PieceType;

typedef struct {
    int row;
    int col;
} Point;

typedef struct {
    PieceType type;
    Point rotations[4][4];
    char color;
} Piece;

typedef struct {
    PieceType type;
    Point base;
    char rotIndex;
} Fall;

typedef struct {
    char board[20][10];
    int score;
    Piece all_pieces[8];
    Fall current;
} GameState;

#endif
