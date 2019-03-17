/*
 * Move.h
 *
 *  Created on: 11-Dec-2011
 *      Author: raghav
 */

#ifndef MOVE_H_
#define MOVE_H_

#include "defs.h"
#include<iostream>

enum SpecialMove {
	NORMAL_MOVE, //00
	PROMOTION_MOVE, //01
	ENPASSANT_MOVE, //10
	CASTLE_MOVE	//11
};

struct Move {
	U32 state;

	inline Move() :
			state(0) {
	}

	inline Move(int move) :
			state(move) {
	}

	inline Move(Square from, Square to, SpecialMove sm, Piece pt);

	inline void sset(Square from, Square to);
	inline void set(Square from, Square to, SpecialMove sm, Piece pt);
	inline void promotion(Piece p);

	inline Square from() const;
	inline Square to() const;
	inline SpecialMove special() const;
	inline Piece piece() const;
	inline Piece promotion() const;

	inline U32 key() const;

	inline operator int();
	friend inline std::ostream& operator<<(std::ostream& out, Move& m);
	inline bool operator==(const Move& s);

};

inline U32 Move::key() const{
	return state & 0xFFF;
}

inline std::ostream& operator<<(std::ostream& out, Move& m) {
    return out << SQCHAR[m.from()] << SQCHAR[m.to()]<<":"<<m.special()<<"-"<<m.promotion();
}

inline void Move::sset(Square from, Square to) {
	state = ((to) | (from << 6)) & 0xfff;
}

inline void Move::set(Square from, Square to, SpecialMove sm, Piece pt) {
	state = (to) | (from << 6) | (sm << 12) | (pt << 14);
}

inline void Move::promotion(Piece p) {
	state |= (p << 18);
}

inline Move::Move(Square from, Square to, SpecialMove sm = NORMAL_MOVE,
		Piece pt = PIECE_NONE) {
	state = (to) | (from << 6) | (sm << 12) | (pt << 14);
}

inline Square Move::from() const {
	return Square((state >> 6) & 0x3F);
}

inline Square Move::to() const {
	return Square(state & 0x3F);
}

inline SpecialMove Move::special() const {
	return SpecialMove((state >> 12) & 3);
}

inline Piece Move::piece() const {
	return Piece((state >> 14) & 15);
}

inline Piece Move::promotion() const {
	return Piece((state >> 18) & 15);
}

inline Move::operator int() {
	return state;
}

inline bool Move::operator==(const Move& s) {
	return state == s.state;
}

struct ScoredMove: Move {
	int score;

	inline bool operator<(const ScoredMove& s) {
		return score < s.score;
	}

};

#endif /* MOVE_H_ */
