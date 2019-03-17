/*
 * defs.h
 *
 *  Created on: 13-Dec-2011
 *      Author: raghav
 */

#ifndef END_DEFS_H_
#define END_DEFS_H_

#include <inttypes.h>
#include <climits>
#include <cstdlib>
#include <stdint.h>
#include <iostream>
#include <cassert>
#include <cstring>
#include "timer.h"
#include <map>
#include <unordered_map>

#if UINTPTR_MAX == 0xffffffffffffffff
/* 64-bit */
#define IS_64BIT
/* Fail safe test*/
#elif defined(_WIN64)
#define IS_64BIT
#elif defined(__x86_64__)
#define IS_64BIT
#elif defined(__LP64__)
#define IS_64BIT
#else

#endif

#if defined(IS_64BIT)
const bool CpuIs64Bit = true;
#else
const bool CpuIs64Bit = false;
#endif

#define USE_BSFQ


// Limits
#define MAX_MOVE_BUFFER 1000000
#define MAX_PLY 100
#define PLY 6
#define INIT_MAX_GAME_STACK 300
#define INCR_MAX_GAME_STACK 100
//#define MAX_DEPTH 7
#define LARGE_NUMBER 128000 //INT_MAX

#define Min(x, y) (((x) < (y)) ? (x) : (y))
#define Max(x, y) (((x) < (y)) ? (y) : (x))
//#define Sign(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 0))

typedef uint64_t U64;
typedef uint64_t Bitboard;
typedef uint32_t U32;
typedef int16_t I16;

typedef std::unordered_map<std::string, int> map_str_int_t;
typedef std::unordered_map<std::string, float> map_str_float_t;


enum PieceType {
	PIECE_TYPE_NONE = 0,
	PAWN = 1,
	KING = 2,
	KNIGHT = 3,
	BISHOP = 5,
	ROOK = 6,
	QUEEN = 7,

};

extern const PieceType orderedPt[6];

enum Piece {
	PIECE_NONE_DARK_SQ = 0,
	WP = 1,
	WK = 2,
	WN = 3,
	WB = 5,
	WR = 6,
	WQ = 7,
	PIECE_NONE = 8,
	BP = 9,
	BK = 10,
	BN = 11,
	BB = 13,
	BR = 14,
	BQ = 15,

};

extern const char PIECENAMES[16];

extern const int material_score[16];

enum Color {
	WHITE, BLACK, COLOR_NONE
};

extern const int ColorWeight[3];

enum PLYMAX {
	PLY_MAX = 100, PLY_MAX_PLUS_2 = PLY_MAX + 2,
};

enum ValueType {
	VALUE_TYPE_NONE = 0,
	VALUE_TYPE_UPPER = 1,
	VALUE_TYPE_LOWER = 2,
	VALUE_TYPE_EXACT = VALUE_TYPE_UPPER | VALUE_TYPE_LOWER
};

enum Value {
	VALUE_ZERO = 0,
	VALUE_DRAW = 0,
	VALUE_KNOWN_WIN = 15000,
	VALUE_MATE = 30000,
	VALUE_INFINITE = 30001,
	VALUE_NONE = 30002,

	VALUE_MATE_IN_PLY_MAX = VALUE_MATE - PLY_MAX,
	VALUE_MATED_IN_PLY_MAX = -VALUE_MATE + PLY_MAX,

	VALUE_ENSURE_INTEGER_SIZE_P = INT_MAX,
	VALUE_ENSURE_INTEGER_SIZE_N = INT_MIN
};

enum Depth {

	ONE_PLY = 2,

	DEPTH_ZERO = 0 * ONE_PLY,
	DEPTH_QS_CHECKS = -1 * ONE_PLY,
	DEPTH_QS_NO_CHECKS = -2 * ONE_PLY,

	DEPTH_NONE = -127 * ONE_PLY
};

enum Square {
	SQ_A1,
	SQ_B1,
	SQ_C1,
	SQ_D1,
	SQ_E1,
	SQ_F1,
	SQ_G1,
	SQ_H1,
	SQ_A2,
	SQ_B2,
	SQ_C2,
	SQ_D2,
	SQ_E2,
	SQ_F2,
	SQ_G2,
	SQ_H2,
	SQ_A3,
	SQ_B3,
	SQ_C3,
	SQ_D3,
	SQ_E3,
	SQ_F3,
	SQ_G3,
	SQ_H3,
	SQ_A4,
	SQ_B4,
	SQ_C4,
	SQ_D4,
	SQ_E4,
	SQ_F4,
	SQ_G4,
	SQ_H4,
	SQ_A5,
	SQ_B5,
	SQ_C5,
	SQ_D5,
	SQ_E5,
	SQ_F5,
	SQ_G5,
	SQ_H5,
	SQ_A6,
	SQ_B6,
	SQ_C6,
	SQ_D6,
	SQ_E6,
	SQ_F6,
	SQ_G6,
	SQ_H6,
	SQ_A7,
	SQ_B7,
	SQ_C7,
	SQ_D7,
	SQ_E7,
	SQ_F7,
	SQ_G7,
	SQ_H7,
	SQ_A8,
	SQ_B8,
	SQ_C8,
	SQ_D8,
	SQ_E8,
	SQ_F8,
	SQ_G8,
	SQ_H8,
	SQ_NONE,

	DELTA_N = 8,
	DELTA_E = 1,
	DELTA_S = -8,
	DELTA_W = -1,

	DELTA_NN = DELTA_N + DELTA_N,
	DELTA_NE = DELTA_N + DELTA_E,
	DELTA_SE = DELTA_S + DELTA_E,
	DELTA_SS = DELTA_S + DELTA_S,
	DELTA_SW = DELTA_S + DELTA_W,
	DELTA_NW = DELTA_N + DELTA_W
};

extern const char* SQCHAR[64];

extern const char* COLORNAME[2];

extern const char* CASTLE_NAMES[4];

enum File {
	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H
};

enum Rank {
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8
};

enum SquareColor {
	DARK, LIGHT
};

enum Castling {
	CASTLING_NONE = 0, CASTLING_OO = 1, CASTLING_OOO = 2, CASTLING_OO_OOO = 3,
};

extern int CastlingMoves[4];

extern Castling CastlingStart[2];

extern Bitboard maskEG[2];
extern Bitboard maskFG[2];
extern Bitboard maskBD[2];
extern Bitboard maskCE[2];

extern unsigned int CASTLE_MOVES[4];



enum HashType {
	HASH_ALPHA = 0,  HASH_BETA = 1,HASH_EXACT = 2,
};

enum MoveType {
	MOVETYPE_NORMAL, MOVETYPE_CAPTURE,
};

#define ENABLE_OPERATORS_ON(T) \
inline T operator+ (const T d1, const T d2) { return T(int(d1) + int(d2)); } \
inline T operator- (const T d1, const T d2) { return T(int(d1) - int(d2)); } \
inline T operator* (int i, const T d) {  return T(i * int(d)); } \
inline T operator* (const T d, int i) {  return T(int(d) * i); } \
inline T operator/ (const T d, int i) { return T(int(d) / i); } \
inline T operator- (const T d) { return T(-int(d)); } \
inline T operator++ (T& d, int) {d = T(int(d) + 1); return d; } \
inline T operator-- (T& d, int) { d = T(int(d) - 1); return d; } \
inline void operator+= (T& d1, const T d2) { d1 = d1 + d2; } \
inline void operator-= (T& d1, const T d2) { d1 = d1 - d2; } \
inline void operator*= (T& d, int i) { d = T(int(d) * i); } \
inline void operator/= (T& d, int i) { d = T(int(d) / i); }

ENABLE_OPERATORS_ON(Value)
ENABLE_OPERATORS_ON(PieceType)
ENABLE_OPERATORS_ON(Piece)
ENABLE_OPERATORS_ON(Color)
ENABLE_OPERATORS_ON(Depth)
ENABLE_OPERATORS_ON(Square)
ENABLE_OPERATORS_ON(File)
ENABLE_OPERATORS_ON(Rank)

ENABLE_OPERATORS_ON(Castling)

#undef ENABLE_OPERATORS_ON

Bitboard inline bitboard(Square sq) {
	return 1ULL << sq;
}

Square inline boardIndex(Rank rank, File file) {
	return Square((rank << 3) + file);
}

PieceType inline pieceType(Piece piece) {
	return PieceType(piece & 7);
}

Piece inline piece(PieceType pt, Color c) {
	return Piece((int(c) << 3) | int(pt));
}

Color inline pieceColor(Piece piece) {
	return pieceType(piece) != PIECE_TYPE_NONE ?
			(piece & 8 ? BLACK : WHITE) : COLOR_NONE;
}

inline File square_file(Square s) {
	return File(int(s) & 7);
}

inline Rank square_rank(Square s) {
	return Rank(int(s) >> 3);
}

inline int file_distance(Square s1, Square s2) {
	return abs(square_file(s1) - square_file(s2));
}

inline int rank_distance(Square s1, Square s2) {
	return abs(square_rank(s1) - square_rank(s2));
}

inline int square_distance(Square s1, Square s2) {
	return Max(file_distance(s1, s2), rank_distance(s1, s2));
}

inline bool file_is_ok(File f) {
	return f >= FILE_A && f <= FILE_H;
}

inline bool rank_is_ok(Rank r) {
	return r >= RANK_1 && r <= RANK_8;
}

inline bool square_is_ok(Square s) {
	return s >= SQ_A1 && s <= SQ_H8;
}

inline Color opposite_color(Color c) {
	return Color(c ^ 1);
}

inline char* to_an(Square s, char* an) {

	an[0] = square_file(s) + 'A';
	an[1] = square_rank(s) + '1';
	an[3] = '\0';
	return an;
}

inline Square an_square(const char* an) {
	return Square((toupper(an[0]) - 'A') + (an[1] - '1') * 8);
}

inline Piece charToPiece(char p) {
	switch (p) {
	case 'P':
		return WP;
	case 'K':
		return WK;
	case 'N':
		return WN;
	case 'B':
		return WB;
	case 'R':
		return WR;
	case 'Q':
		return WQ;
	case 'p':
		return BP;
	case 'k':
		return BK;
	case 'n':
		return BN;
	case 'b':
		return BB;
	case 'r':
		return BR;
	case 'q':
		return BQ;
	default:
		return PIECE_NONE;
	}
}

extern const int flip[64];

#endif /* DEFS_H_ */
