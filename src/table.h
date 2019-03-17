/*
 * table.h
 *
 *  Created on: 25-Dec-2011
 *      Author: raghav
 */

#ifndef TABLE_H_
#define TABLE_H_

#include "defs.h"
#include <ostream>

class record {
private:
	U64 key_;
	U32 move_;
	I16 score_;


public:

	record() :
	 key_(0), move_(0) , score_(0){
	 }


	record(U64 hash, int score, U32 move, int depth, HashType rt) {
		key_ = hash;
		move_ = move;
		score_ = score;

		move_ = move_& 0x3FFFFF;

		move_ |= ((rt & 0x3)<<22);
		move_ |= ((depth&0xFF)<<24) ;

	}

	inline U64 key() const {
		return key_;
	}

	inline bool operator==(const U64 cmp) const {
		return key_ == cmp; //(cmp & 0xFFFFFFFF);
	}

	inline int score() const {
		return score_;
	}

	inline U32 move() const {
		return move_& 0x3FFFFF;
	}

	inline int depth() const {
		return (move_>>24)&0xFF;
	}

	inline const HashType type() const {
		return HashType((move_>>22)&0x3) ;
	}

};

class Ttable {
private:
	record* table;
	U32 size;
	U32 occupied;
	U32 collisions;
	U32 hits;

public:

	Ttable(U32 sizemb);
	Ttable();
	~Ttable();
	void setsize(U32 size);
	void resize(U32 size);
	void reset();
	record* operator[](U64 hash);
	record* operator[](U64 hash) const;
	bool put(record& in);
	double fillratio() const;
	U32 collision() const;

	friend std::ostream& operator<<(std::ostream& out, Ttable& tt);

};

#endif /* TABLE_H_ */
