/*
 * gamestack.h
 *
 *  Created on: 24-Dec-2011
 *      Author: raghav
 */

#ifndef GAMESTACK_H_
#define GAMESTACK_H_

struct GameState {
	unsigned int move;
	Castling canCastle[2];
	Square enpassant;
	int fiftyMoves;

	U64 hash;

};


#endif /* GAMESTACK_H_ */
