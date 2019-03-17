/*
 * search.cpp
 *
 *  Created on: 11-Dec-2011
 *      Author: raghav
 */

#include "board.h"

int Board::think(int searchDepth) {

	//  iterative deepening
	int score = 0;
	int bestValues[MAX_PLY];
    bestValues[0] = 0;

	searchDepth = searchDepth < PLY?searchDepth : PLY - 1;

#ifndef USE_TT
	memset(pv, 0, sizeof(pv));
	memset(pv_length, 0, sizeof(pv_length));
#endif
	inodes = 0;

#ifdef USE_KM
	memset(mtable, 0, sizeof(mtable));
#endif

#ifdef USE_TT
	ttable.reset();
#endif

	int alpha = -LARGE_NUMBER, beta = LARGE_NUMBER;
	int aspDelta = 0;

	for (int depth = 1; depth <= searchDepth; depth++) {
		//  clear the buffers:
		t.tic();
        
        int n_aspirations = 0;

		do {
			memset(moveBufLen, 0, sizeof(moveBufLen));
			memset(moveBuffer, 0, sizeof(moveBuffer));
			moveBufLen[0] = moveBuffer;

			score = search(0, depth, alpha, beta);
            n_aspirations++;
            
            std::cout << "info" << "~asp d:" << depth
            << " i:" << n_aspirations << " del: " << aspDelta
            << " a: " << alpha << " s: " << score << " b: " <<beta << std::endl;

			if (score >= beta) {
				beta = Min(beta + aspDelta, LARGE_NUMBER);
				aspDelta += aspDelta / 2;
			} else if (score <= alpha) {

				alpha = Max(alpha - aspDelta, -LARGE_NUMBER);
				aspDelta += aspDelta / 2;
			} else
				break;
            
            

		} while (abs(score) < LARGE_NUMBER);

		bestValues[depth] = score;
        int prevDelta1 = (depth>2) ? bestValues[depth - 1] - bestValues[depth - 2] : 0;
        int prevDelta2 = (depth>3) ? bestValues[depth - 2] - bestValues[depth - 3] : 0;

		if (depth <= (searchDepth)) {
            aspDelta = Min(Max(int((abs(prevDelta1) + abs(prevDelta2)) / 2), 10),100);

			alpha = Max(bestValues[depth - 1] - aspDelta, -LARGE_NUMBER);
			beta = Min(bestValues[depth - 1] + aspDelta, LARGE_NUMBER);
		} else {
			alpha = -LARGE_NUMBER, beta = LARGE_NUMBER;
		}

        std::cout << "info" << " s: " << score << " a: " << alpha << " b: " << beta << std::endl;
        std::cout << "info" << " d: " << depth << " naw: " << n_aspirations <<" nc: " << inodes << " t: " << t.toc() << std::endl;
#ifdef USE_TT
		std::cout << "info " << ttable << std::endl;
		updatePvFromTt(depth);
#endif

		std::cout << "info PV: ";
		for (int i = 0; i < pv_length[0]; i++) {
			std::cout << pv[0][i];
			std::cout << " ";
		}
		std::cout << std::endl;

	}
	return (score);
}

int Board::search(int ply, int depth, int alpha, int beta) {

	int val;
	ScoredMove* i;

#ifndef USE_TT
	pv_length[ply] = 0;
#endif

	if (repeatCount() >= 3)
		return VALUE_DRAW;

	if (fiftyMoves >= 100)
		return VALUE_DRAW;

#ifdef USE_TT
	record* rec = ttable[hash];

	if (rec && rec->depth() >= depth) {

		HashType rt = rec->type();
		val = rec->score();
		switch (rt) {
		case HASH_EXACT:
			return val;
			break;
		case HASH_ALPHA:
			if (val <= alpha)
				return alpha;
			break;
		case HASH_BETA:
			if (val >= beta)
				return beta;
			break;
		}

	}
#endif


	if (depth <= 0)
		return qsearch(ply, alpha, beta);

	moveBufLen[ply + 1] = generateMoves<MOVETYPE_NORMAL>(moveBufLen[ply], ply);

	unsigned int lastNc = inodes;
	sort(moveBufLen[ply], moveBufLen[ply + 1]);

	U64 movehash = hash;
	U32 pvmove = 0;
#ifdef USE_TT
	HashType rt = HASH_ALPHA;
#endif
	for (i = moveBufLen[ply]; i < moveBufLen[ply + 1]; i++) {

		if (!makeMove(*i)) {
#ifdef USE_KM
			mtable[i->key()] -= depth * depth;
#endif
			continue;
		}
		{
			inodes++;

			if (pvmove && depth <= 2) {

				val = -search(ply + 1, depth - 1, -(alpha + 1), -alpha);
				if (val > alpha && val < beta)
					val = -search(ply + 1, depth - 1, -beta, -alpha);

			} else {
				val = -search(ply + 1, depth - 1, -beta, -alpha);
			}

			assert(hash!=movehash);
			unmakeMove(*i);

			assert(hash==movehash);

			if (val > alpha) {

				if (val >= beta) {

#ifdef USE_KM
					mtable[i->key()] += 4 * depth * depth;
#endif

#ifdef USE_TT
					record rec_ = record(hash, beta, i->state, depth,
							HASH_BETA);
					ttable.put(rec_);
#endif
					return beta;
				}

				alpha = val;
				pvmove = i->state;
#ifdef USE_KM
				mtable[i->key()] += depth * depth;
#endif

#ifndef USE_TT
				pv[ply][ply].state = i->state;

				memcpy(&pv[ply][ply + 1], &pv[ply + 1][ply + 1],
						(pv_length[ply + 1]) * sizeof(Move));
				pv_length[ply] = pv_length[ply + 1] + 1;
#endif

#ifdef USE_TT
				rt = HASH_EXACT;
#endif

			}

#ifdef USE_KM
			mtable[i->key()] -= depth;
#endif
		}

		sort(i + 1, moveBufLen[ply + 1]);

	}

#ifdef USE_TT
	record rec_ = record(hash, alpha, pvmove, depth, rt);
	ttable.put(rec_);
#endif

	if (lastNc == inodes) {
		if (inCheck(player)) {
			return -VALUE_MATE;
		} else {
			return VALUE_DRAW;
		}
	}

	return alpha;
}

#ifdef USE_TT
void Board::updatePvFromTt(int mply) {
	memset(pv, 0, sizeof(pv));
	pv_length[0] = 0;
	updatePvFromTt(0, mply);
}

void Board::updatePvFromTt(int ply, int mply) {
	record* rec = ttable[hash];


	assert(ply<PLY_MAX);
	if (rec && rec->type() == HASH_EXACT) {

		assert(rec->type()==HASH_EXACT);
		pv_length[0] = ply + 1;
		Move sm(rec->move());
		if (sm.state) {
			pv[0][ply] = sm;
			if (ply <= mply && makeMove(sm)) {
				updatePvFromTt(ply + 1, mply);
				unmakeMove(sm);
			}
		}
	}
}
#endif

int Board::qsearch(int ply, int alpha, int beta) {

	int val;
	ScoredMove* i;

#ifdef USE_TT
	record* rec = ttable[hash];

	if (rec) {

		HashType rt = rec->type();
		switch (rt) {
		case HASH_EXACT:
			return rec->score();
		case HASH_ALPHA:
			if (rec->score() <= alpha)
				return alpha;
			break;
		case HASH_BETA:
			if (rec->score() > beta)
				return beta;
			break;
		}

	}
#endif

	if (ply >= PLY_MAX - 5)
		return VALUE_DRAW;

	if (inCheck(player))
		return search(ply, 1, alpha, beta);

	val = (this->*eval)();

	if (val > beta)
		return beta;

	if (val > alpha)
		alpha = val;

	moveBufLen[ply + 1] = generateMoves<MOVETYPE_CAPTURE>(moveBufLen[ply], ply);

	sort(moveBufLen[ply], moveBufLen[ply + 1]);

	for (i = moveBufLen[ply]; i < moveBufLen[ply + 1]; i++) {
		if (!makeMove(*i)) {
			continue;
		}
		{
			inodes++;
			val = -qsearch(ply + 1, -beta, -alpha);
			unmakeMove(*i);
			if (val > alpha) {

				if (val >= beta) {

					return beta;
				}
				alpha = val;
			}

		}

		sort(i + 1, moveBufLen[ply + 1]);

	}
	return alpha;
}

void Board::sort(ScoredMove* from, ScoredMove* to) {
	ScoredMove* i;
	ScoredMove* bm = from; /* best i */

	ScoredMove temp;

	for (i = from; i < to; ++i)
		if (*i > *bm) {
			bm = i;
		}

	temp = *from;
	*from = *bm;
	*bm = temp;
}

U64 Board::perft(int depth) {

	ScoredMove mlist[1000];

	Move m;
	U64 sum = 0;

	ScoredMove* last = generateMoves<MOVETYPE_NORMAL>(mlist);

	if (depth <= 1)
		return int(last - mlist);

	for (ScoredMove* cur = mlist; cur != last; cur++) {
		if (!makeMove(*cur))
			continue;
		sum += perft(depth - 1);
		unmakeMove(*cur);
	}
	return sum;
}
