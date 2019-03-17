/*
 * move.cpp
 *
 *  Created on: 11-Dec-2011
 *      Author: raghav
 */
#include "board.h"
#include "hash.h"

bool Board::makeMove(Move move) {

	pushGameState(move);
	Square from = move.from();
	Square to = move.to();
	SpecialMove sm = move.special();

	Piece fromPiece = board[from];
	Piece toPiece = board[to];

	Color oppositeColor = opposite_color(player);

	PieceType fromPieceType = pieceType(fromPiece);
	Color fromPieceColor = player;
	PieceType toPieceType = pieceType(toPiece);
	Color toPieceColor = pieceColor(toPiece);

	board[to] = fromPiece;
	board[from] = PIECE_NONE;
	Bitboard mb = make_move_bb(from, to);
	fiftyMoves++;

	assert(pieceColor(fromPiece)==player);

	// enpassant move
	if(enpassant != SQ_NONE)
		hash ^= enpassantHash(enpassant);

	if (fromPieceType == PAWN) {
		fiftyMoves = 0;
		if (to == (from + 16 * ColorWeight[fromPieceColor])) {
			enpassant = Square(from + 8 * ColorWeight[fromPieceColor]);
			hash ^= enpassantHash(enpassant);
		} else {
			enpassant = SQ_NONE;
		}
	} else {
		enpassant = SQ_NONE;
	}

	assert(enpassant>=SQ_A1 && enpassant<=SQ_NONE);

	if (sm == ENPASSANT_MOVE) {
		to = Square(to - 8 * ColorWeight[player]);
		toPiece = board[to];
		board[to] = PIECE_NONE;
		toPieceType = pieceType(toPiece);
		toPieceColor = pieceColor(toPiece);
	}
	// enpassant move //

	//Castling move
	if (canCastle[fromPieceColor]) {
		if (fromPieceType == KING) {
			hash ^= canCastle[fromPieceColor];
			canCastle[fromPieceColor] = CASTLING_NONE;
		} else if (fromPieceType == ROOK) {
			if (square_file(from) == FILE_A) {
				hash ^= castlingHash(
						Castling(canCastle[fromPieceColor] & CASTLING_OOO),
						fromPieceColor);
				canCastle[fromPieceColor] = Castling(
						canCastle[fromPieceColor] & CASTLING_OO);
			} else if (square_file(from) == FILE_H) {
				hash ^= castlingHash(
						Castling(canCastle[fromPieceColor] & CASTLING_OO),
						fromPieceColor);
				canCastle[fromPieceColor] = Castling(
						canCastle[fromPieceColor] & CASTLING_OOO);
			}
		}
	}
	if (canCastle[toPieceColor]) {
		if (toPieceType == ROOK) {
			if (square_file(to) == FILE_A) {
				hash ^= castlingHash(
						Castling(canCastle[toPieceColor] & CASTLING_OOO),
						toPieceColor);
				canCastle[toPieceColor] = Castling(
						canCastle[toPieceColor] & CASTLING_OO);
			} else if (square_file(to) == FILE_H) {
				hash ^= castlingHash(
						Castling(canCastle[toPieceColor] & CASTLING_OO),
						toPieceColor);
				canCastle[toPieceColor] = Castling(
						canCastle[toPieceColor] & CASTLING_OOO);
			}
		}
	}

	if (sm == CASTLE_MOVE) {
		Square rfrom, rto;
		assert((fromPieceColor==WHITE) || (fromPieceColor==BLACK));
		if (fromPieceColor == WHITE) {
			assert(from==SQ_E1);
			assert((to==SQ_G1)||(to==SQ_C1));
			if (to == SQ_G1) {
				rfrom = SQ_H1;
				rto = SQ_F1;
			} else if (to == SQ_C1) {
				rfrom = SQ_A1;
				rto = SQ_D1;
			}
		} else if (fromPieceColor == BLACK) {
			assert(from==SQ_E8);
			assert((to==SQ_G8)||(to==SQ_C8));
			if (to == SQ_G8) {
				rfrom = SQ_H8;
				rto = SQ_F8;
			} else if (to == SQ_C8) {
				rfrom = SQ_A8;
				rto = SQ_D8;
			}
		}


		board[rto] = board[rfrom];
		board[rfrom] = PIECE_NONE;

		Bitboard rmb = make_move_bb(rfrom, rto);
		do_move_bb(&PieceBB[ROOK], rmb);
		do_move_bb(&PieceBB[0], rmb);
		do_move_bb(&ColorBB[fromPieceColor], rmb);

		hash ^= moveHash(fromPiece, rfrom, rto);

	}

	if (toPieceType != PIECE_TYPE_NONE) {
		clear_bit(&PieceBB[toPieceType], to);
		clear_bit(&PieceBB[0], to);
		clear_bit(&ColorBB[toPieceColor], to);
		fiftyMoves = 0;
		hash ^= pieceHash(toPiece, to);
	}

	//Bitboard mb = make_move_bb(from, to);
	do_move_bb(&PieceBB[fromPieceType], mb);
	do_move_bb(&PieceBB[0], mb);
	do_move_bb(&ColorBB[fromPieceColor], mb);
	hash ^= moveHash(fromPiece, from, to);

	if (sm == PROMOTION_MOVE) {
		Piece prPiece = move.promotion();
		clear_bit(&PieceBB[fromPieceType], to);
		set_bit(&PieceBB[pieceType(prPiece)], to);
		board[to] = prPiece;
		hash ^= pieceHash(fromPiece, to);
		hash ^= pieceHash(prPiece, to);
	}

	hash ^= turnHash();
	player = oppositeColor;

	assert(PieceBB[0] == (ColorBB[0]|ColorBB[1]));

	if (inCheck(opposite_color(player))) {
		unmakeMove(move);
		return false;
	}

	return true;

}

bool Board::unmakeMove(Move move) {

	Color oppositeColor = player;
	player = opposite_color(player);

	Square from = move.from();
	Square to = move.to();
	SpecialMove sm = move.special();
	Piece undoPiece = move.piece();
	PieceType undoPieceType = pieceType(undoPiece);
	Color undoPieceColor = oppositeColor; 

	if (undoPieceType != PIECE_TYPE_NONE) {
		assert(pieceColor(undoPiece)==oppositeColor);
	}

	if (sm == PROMOTION_MOVE) {
		Piece prpiece = move.promotion();
		set_bit(&PieceBB[PAWN], to);
		clear_bit(&PieceBB[pieceType(prpiece)], to);
		board[to] = piece(PAWN, player);
	}

	Piece movedPiece = board[to];
	PieceType movedPieceType = pieceType(movedPiece);
	Color movedPieceColor = player;
	Bitboard mb = make_move_bb(to, from);

	assert(pieceColor(movedPiece)==player);

	do_move_bb(&PieceBB[movedPieceType], mb);
	do_move_bb(&PieceBB[0], mb);
	do_move_bb(&ColorBB[movedPieceColor], mb);

	if (sm == CASTLE_MOVE) {
		Square rfrom, rto;

		if (movedPieceColor == WHITE) {
			assert((to==SQ_G1) || (to== SQ_C1));
			if (to == SQ_G1) {
				rfrom = SQ_H1;
				rto = SQ_F1;
			} else if (to == SQ_C1) {
				rfrom = SQ_A1;
				rto = SQ_D1;
			}
		} else {
			assert((to==SQ_G8)|| (to==SQ_C8));
			if (to == SQ_G8) {
				rfrom = SQ_H8;
				rto = SQ_F8;
			} else if (to == SQ_C8) {
				rfrom = SQ_A8;
				rto = SQ_D8;
			}
		}

		Bitboard rmb = make_move_bb(rto, rfrom);
		do_move_bb(&PieceBB[ROOK], rmb);
		do_move_bb(&PieceBB[0], rmb);
		do_move_bb(&ColorBB[player], rmb);
		board[rfrom] = board[rto];
		board[rto] = PIECE_NONE;
	}

	/* enpassant move */

	if (sm == ENPASSANT_MOVE) {
		board[to] = PIECE_NONE;
		to = Square(to - 8 * ColorWeight[player]);
	}
	/* ---enpassant move */

	if (undoPieceType != PIECE_TYPE_NONE) {
		set_bit(&PieceBB[undoPieceType], to);
		set_bit(&PieceBB[0], to);
		set_bit(&ColorBB[undoPieceColor], to);
	}

	board[from] = movedPiece;
	board[to] = undoPiece;

	unsigned int m = popGameState();

	if(move.state!=m){
			std::cout<<move<<std::endl<<Move(m)<<std::endl;
		}
	assert(move.state==m);



	assert(PieceBB[0] == (ColorBB[0]|ColorBB[1]));

	return true;

}
