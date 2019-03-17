/*
 * hash.h
 *
 *  Created on: 24-Dec-2011
 *      Author: raghav
 */

#ifndef HASH_H_
#define HASH_H_



#include "defs.h"
#include "board.h"

extern const U64 Random64[781];

extern const U64 *RandomPiece;
extern const U64 *RandomCastle;
extern const U64 *RandomEnPassant;
extern const U64 *RandomTurn;

extern const int PieceIndex[16];

class Board;

U64 boardHash(const Board* board_);

inline U64 pieceHash(Piece piece, Square sq) {
	return (pieceType(piece) != PIECE_TYPE_NONE) ?
			RandomPiece[64 * PieceIndex[piece] + sq] : 0ULL;
}

inline U64 moveHash(Piece piece, Square from, Square to) {
	return pieceHash(piece, from) ^ pieceHash(piece, to);
}

inline U64 castlingHash(Castling castling, Color c) {
	switch (castling) {
	case CASTLING_OO:
		return RandomCastle[c << 1];
		break;
	case CASTLING_OOO:
		return RandomCastle[(c << 1) + 1];
		break;
	case CASTLING_OO_OOO:
		return RandomCastle[c << 1] ^ RandomCastle[(c << 1) + 1];
		break;
	default :
		return 0ULL;
	}

}

inline U64 enpassantHash(Square sq) {
	return RandomEnPassant[square_file(sq)];
}

inline U64 turnHash(Color c) {
	return (c == WHITE) ? RandomTurn[0] : 0ULL;
}

inline U64 turnHash() {
	return RandomTurn[0];
}

#endif /* HASH_H_ */
