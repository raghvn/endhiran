/*
 * defs.cpp
 *
 *  Created on: 21-Dec-2011
 *      Author: raghav
 */

#include "defs.h"


const char* SQCHAR[64] = {
		"A1",
		"B1",
		"C1",
		"D1",
		"E1",
		"F1",
		"G1",
		"H1",
		"A2",
		"B2",
		"C2",
		"D2",
		"E2",
		"F2",
		"G2",
		"H2",
		"A3",
		"B3",
		"C3",
		"D3",
		"E3",
		"F3",
		"G3",
		"H3",
		"A4",
		"B4",
		"C4",
		"D4",
		"E4",
		"F4",
		"G4",
		"H4",
		"A5",
		"B5",
		"C5",
		"D5",
		"E5",
		"F5",
		"G5",
		"H5",
		"A6",
		"B6",
		"C6",
		"D6",
		"E6",
		"F6",
		"G6",
		"H6",
		"A7",
		"B7",
		"C7",
		"D7",
		"E7",
		"F7",
		"G7",
		"H7",
		"A8",
		"B8",
		"C8",
		"D8",
		"E8",
		"F8",
		"G8",
		"H8",
};

const char PIECENAMES[16] = { ' ', 'P', 'K', 'N', ' ', 'B', 'R', 'Q', ' ', 'p',
		'k', 'n', ' ', 'b', 'r', 'q' };

const char* COLORNAME[2] = { "White" , "Black"};
char COLORCHARS[2] = {'w','b'};


const int ColorWeight[3] = {1,-1,0};

Bitboard maskEG[2];
Bitboard maskFG[2];
Bitboard maskBD[2];
Bitboard maskCE[2];

unsigned int CASTLE_MOVES[4];

int CastlingMoves[4] = {0, 1, 1, 2};

Castling CastlingStart[2] = {CASTLING_OO_OOO, CASTLING_OO_OOO};

const char* CASTLE_NAMES[4] = {"", "OO","OOO" , "OO_OOO"};

const PieceType orderedPt[6] = {PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING};

const int flip[64] = { 56, 57, 58, 59, 60, 61, 62, 63, 48, 49, 50, 51, 52, 53, 54, 55,
    40, 41, 42, 43, 44, 45, 46, 47, 32, 33, 34, 35, 36, 37, 38, 39, 24, 25,
    26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23, 8, 9, 10, 11,
    12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7 };
