/*
 * movegen.cpp
 *
 *  Created on: 11-Dec-2011
 *      Author: raghav
 */

#include "board.h"

int ttscore;
U32 ttmove;
U32 pvmove;

template<MoveType mt> ScoredMove* Board::generateMoves(ScoredMove* moves,
		int ply) const {
	ttscore = 0;
	ttmove = 0;
	pvmove = pv[0][ply].state;
#ifdef USE_TT
	record* rec;
	if (mt == MOVETYPE_NORMAL) {
		rec = ttable[hash];
		if (rec) {
			ttmove = rec->move();
			ttscore = (rec->type() - 1) * 10000;

		}
	}
#endif

	Bitboard target = EmptyBoardBB;
	if (mt == MOVETYPE_CAPTURE) {
		target = ColorBB[opposite_color(player)];
	} else {
		target = ~ColorBB[player];
	}

	if (player == WHITE) {
		moves = generatePawnMoves<mt, WHITE>(moves);
	} else {
		moves = generatePawnMoves<mt, BLACK>(moves);
	}
	moves = generatePieceMoves<mt, KING>(moves, target, player);
	moves = generatePieceMoves<mt, KNIGHT>(moves, target, player);
	moves = generatePieceMoves<mt, BISHOP>(moves, target, player);
	moves = generatePieceMoves<mt, ROOK>(moves, target, player);
	moves = generatePieceMoves<mt, QUEEN>(moves, target, player);

	if (mt == MOVETYPE_NORMAL)
		moves = generateCastlingMoves(moves, player);

	return moves;
}

template<MoveType mt, PieceType pt> inline ScoredMove* Board::generatePieceMoves(
		ScoredMove* moves, Bitboard target, Color c) const {

	Bitboard pos = PieceBB[pt] & ColorBB[c];
	Square from;
	Bitboard attacks = EmptyBoardBB;
	while (pos) {
		from = pop_1st_bit(&pos);
		attacks = attacksFrom<pt>(from) & target;
		moves = addMoves<mt>(moves, from, attacks);
	}

	return moves;
}

template<MoveType mt> inline ScoredMove* Board::addMoves(ScoredMove* moves,
		Square from, Bitboard attacks, SpecialMove sm) const {

	Square to;
	while (attacks) {
		to = pop_1st_bit(&attacks);
		moves->set(from, to, sm, board[to]);
		if (scoreMove < mt > (moves, from, to))
			moves++;
	}

	return moves;
}

template<MoveType mt> inline bool Board::scoreMove(ScoredMove* moves,
		Square from, Square to) const {
	moves->score = 0;

	//Scoring
		if (moves->state == pvmove) {
			moves->score += 10000;
			return true;
		}

	#ifdef USE_TT
		if (mt == MOVETYPE_NORMAL) {
			if (ttmove == moves->state) {
				moves->score += ttscore;
				return true;
			}
		}
	#endif

	#ifdef USE_KM
		if (mt == MOVETYPE_NORMAL) {
			moves->score += mtable[moves->key()];
		}
	#endif

	if (mt == MOVETYPE_CAPTURE) {

		int score = see(from, to);

		if (score <= 0)
			return false;

		moves->score = score;
	} else {
		moves->score +=
				(material_score[board[to]] - material_score[board[from]]) * 10;
	}

	return true;

}

template<MoveType mt, Color c> inline ScoredMove* Board::generatePawnMoves(
		ScoredMove* moves) const {
	Bitboard pos = PieceBB[PAWN] & ColorBB[c];
	Bitboard opp = ColorBB[opposite_color(c)];

	Square from;
	while (pos) {
		Bitboard attacks = EmptyBoardBB;
		from = pop_1st_bit(&pos);
		Rank r = square_rank(from);
		const Rank pRank[2] = { RANK_7, RANK_2 };
		const Rank sRank[2] = { RANK_2, RANK_7 };
		const int delta[4] = { 8, -8, 16, -16 };

		attacks = attacksFrom<PAWN, c>(from);

        if (enpassant != SQ_NONE && ((SetMaskBB[enpassant] & attacks))){
			moves = addMoves<mt>(moves, from, SetMaskBB[enpassant]);
        }

		attacks = attacks & (opp);

		if (mt == MOVETYPE_NORMAL) {
			Bitboard push = (SetMaskBB[from + delta[c]] & emptySquares());
			if ((r == sRank[c]) && push) {
				push = push | (SetMaskBB[from + delta[c + 2]] & emptySquares());
			}
			attacks = attacks | push;
		}

		if (r == pRank[c]) {
			moves = generatePromotionMoves(moves, from, attacks, c);
			continue;
		} else {
			moves = addMoves<mt>(moves, from, attacks);
		}
	}

	return moves;
}

inline ScoredMove* Board::generatePromotionMoves(ScoredMove* moves, Square from,
		Bitboard attacks, Color c) const {
	Square to;
	while (attacks) {
		to = pop_1st_bit(&attacks);

		moves->set(from, to, PROMOTION_MOVE, board[to]);
		moves->promotion(piece(KNIGHT, c));
		moves->score += material_score[KNIGHT];
		moves++;

		for (PieceType pt = BISHOP; pt <= QUEEN; pt++) {
			moves->set(from, to, PROMOTION_MOVE, board[to]);
			moves->promotion(piece(pt, c));
			moves->score += material_score[pt];
			moves++;
		}
	}

	return moves;
}

inline ScoredMove* Board::generateCastlingMoves(ScoredMove* moves,
		Color c) const {

	if (canCastle[c]) {
		if ((canCastle[c] & CASTLING_OO) && (!(maskFG[c] & PieceBB[0]))
				&& (!(attackersTo(maskEG[c]) & ColorBB[opposite_color(c)]))) {
			moves->state = CASTLE_MOVES[(c << 1)];
			moves->score+=1000;
			moves++;
		}
		if ((canCastle[c] & CASTLING_OOO) && (!(maskBD[c] & PieceBB[0]))
				&& (!(attackersTo(maskCE[c]) & ColorBB[opposite_color(c)]))) {
			moves->state = CASTLE_MOVES[((c << 1) + 1)];
			moves->score+=1000;
			moves++;
		}
	}
	return moves;
}

template ScoredMove* Board::generateMoves<MOVETYPE_NORMAL>(ScoredMove* moves,
		int ply) const;
template ScoredMove* Board::generateMoves<MOVETYPE_CAPTURE>(ScoredMove* moves,
		int ply) const;

int Board::see(Move m) const {
	return see(m.from(), m.to());
}

int Board::see_sign(Move m) const {
	return see_sign(m.from(), m.to());
}
int Board::see_sign(Square from, Square to) const {

	if (material_score[board[to]] >= material_score[board[from]])
		return 1;

	return see(from, to);
}

/* Code copied from another chess engine, source unknown! */
int Board::see(Square from, Square to) const {

	Bitboard occupied, attackers, stmAttackers, b;
	int swapList[32], slIndex = 1;
	PieceType capturedType, pt;
	Color stm;

	assert(square_is_ok(from));
	assert(square_is_ok(to));

	capturedType = pieceTypeOn(to);

	// King cannot be recaptured
	if (capturedType == KING)
		return material_score[capturedType];

	occupied = occupiedSquares();

	// Handle en passant moves
	if (enpassant == to && pieceTypeOn(from) == PAWN) {
		Square capQq = (turn() == WHITE ? to - DELTA_N : to - DELTA_S);

		assert(capturedType == PIECE_TYPE_NONE);
		assert(pieceTypeOn(capQq) == PAWN);

		// Remove the captured pawn
		clear_bit(&occupied, capQq);
		capturedType = PAWN;
	}

	// Find all attackers to the destination square, with the moving piece
	// removed, but possibly an X-ray attacker added behind it.
	clear_bit(&occupied, from);
	attackers = (rook_attacks_bb(to, occupied) & piecebb(ROOK, QUEEN))
			| (bishop_attacks_bb(to, occupied) & piecebb(BISHOP, QUEEN))
			| (attacksFrom<KNIGHT>(to) & piecebb(KNIGHT))
			| (attacksFrom<KING>(to) & piecebb(KING))
			| (attacksFrom<PAWN, WHITE>(to) & piecebb(PAWN, BLACK))
			| (attacksFrom<PAWN, BLACK>(to) & piecebb(PAWN, WHITE));

	// If the opponent has no attackers we are finished
	stm = opposite_color(colorOn(from));
	stmAttackers = attackers & colorbb(stm);
	if (!stmAttackers)
		return material_score[capturedType];

	// The destination square is defended, which makes things rather more
	// difficult to compute. We proceed by building up a "swap list" containing
	// the material gain or loss at each stop in a sequence of captures to the
	// destination square, where the sides alternately capture, and always
	// capture with the least valuable piece. After each capture, we look for
	// new X-ray attacks from behind the capturing piece.
	swapList[0] = material_score[capturedType];
	capturedType = pieceTypeOn(from);

	do {
		// Locate the least valuable attacker for the side to move. The loop
		// below looks like it is potentially infinite, but it isn't. We know
		// that the side to move still has at least one attacker left.
		int i = 0;
		for (pt = orderedPt[i]; i < 6 && !(stmAttackers & piecebb(pt));
				i++, pt = orderedPt[i])
			;

		/*{
		 //if(pt==KING)
		 //	continue;
		 //	assert(pt < KING);
		 }*/

		// Remove the attacker we just found from the 'occupied' bitboard,
		// and scan for new X-ray attacks behind the attacker.
		b = stmAttackers & piecebb(pt);
		occupied ^= (b & (~b + 1));
		attackers |= (rook_attacks_bb(to, occupied) & piecebb(ROOK, QUEEN))
				| (bishop_attacks_bb(to, occupied) & piecebb(BISHOP, QUEEN));

		attackers &= occupied; // Cut out pieces we've already done

		// Add the new entry to the swap list
		assert(slIndex < 32);
		swapList[slIndex] = -swapList[slIndex - 1]
				+ material_score[capturedType];
		slIndex++;

		// Remember the value of the capturing piece, and change the side to
		// move before beginning the next iteration.
		capturedType = pt;
		stm = opposite_color(stm);
		stmAttackers = attackers & colorbb(stm);

		// Stop before processing a king capture
		if (capturedType == KING && stmAttackers) {
			assert(slIndex < 32);
			swapList[slIndex++] = material_score[QUEEN] * 10;
			break;
		}
	} while (stmAttackers);

	// Having built the swap list, we negamax through it to find the best
	// achievable score from the point of view of the side to move.
	while (--slIndex)
		swapList[slIndex - 1] = Min(-swapList[slIndex], swapList[slIndex-1]);

	return swapList[0];
}
