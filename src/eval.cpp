/*
 * eval.cpp
 *
 *  Created on: 11-Dec-2011
 *      Author: raghav
 */

#include "board.h"
#include <cmath>
#include "feature_weights.h"


const int material_score[16] = { 0, 100, 20000, 320, 0, 330, 500, 900, 0, 100,
		20000, 320, 0, 330, 500, 900 };

int pawn_pcsq[64] = {  0,  0,  0,  0,  0,  0,  0,  0,
		50, 50, 50, 50, 50, 50, 50, 50,
		10, 10, 20, 30, 30, 20, 10, 10,
		 5,  5, 10, 25, 25, 10,  5,  5,
		 0,  0,  0, 20, 20,  0,  0,  0,
		 5, -5,-10,  0,  0,-10, -5,  5,
		 5, 10, 10,-20,-20, 10, 10,  5,
		 0,  0,  0,  0,  0,  0,  0,  0 };

int knight_pcsq[64] = { -50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50,};

int bishop_pcsq[64] = {-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-10,-10,-10,-10,-10,-20,};

int rook_pcsq[64] = {0,  0,  0,  0,  0,  0,  0,  0,
		  5, 10, 10, 10, 10, 10, 10,  5,
		 -5,  0,  0,  0,  0,  0,  0, -5,
		 -5,  0,  0,  0,  0,  0,  0, -5,
		 -5,  0,  0,  0,  0,  0,  0, -5,
		 -5,  0,  0,  0,  0,  0,  0, -5,
		 -5,  0,  0,  0,  0,  0,  0, -5,
		  0,  0,  0,  5,  5,  0,  0,  0};

int queen_pcsq[64] = {-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
		 -5,  0,  5,  5,  5,  5,  0, -5,
		  0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20};

int king_pcsq[64] = {-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		 20, 20,  0,  0,  0,  0, 20, 20,
		 20, 30, 10,  0,  0, 10, 30, 20 };

int *pcsq[64] = {
		0,
		pawn_pcsq,
		king_pcsq,
		knight_pcsq,
		0,
		bishop_pcsq,
		rook_pcsq,
		queen_pcsq,
};

int Board::eval_simple() {

	int wscore = 0;
	int bscore = 0;
	Square from;
	Square to;
	Bitboard attacks;
	Bitboard pos;
	PieceType pt;
	for (Piece p = WP; p <= WQ; p++) {
		pt = pieceType(p);
		pos = PieceBB[pt] & ColorBB[WHITE];

		while (pos) {
			from = pop_1st_bit(&pos);
			wscore += material_score[pt];

			wscore+=pcsq[pt][flip[from]];
			switch (pt) {
			case PAWN:
				attacks = attacksFrom<PAWN>(from);
				break;
			case KING:
				attacks = EmptyBoardBB;
				break;
			case KNIGHT:
				attacks = attacksFrom<KNIGHT>(from);
				break;
			case BISHOP:
				attacks = attacksFrom<BISHOP>(from);
				break;
			case ROOK:
				attacks = attacksFrom<ROOK>(from);
				break;
			case QUEEN:
				attacks = attacksFrom<QUEEN>(from);
				break;
            default:
                attacks = EmptyBoardBB;
			}

			if (attacks) {
				wscore += bitCnt(attacks & ColorBB[BLACK]) * 5;
				wscore += bitCnt(attacks & ColorBB[WHITE]) * 3;
				wscore += bitCnt(attacks & emptySquares()) * 2;
			}

		}
	}

	for (Piece p = BP; p <= BQ; p++) {
		pt = pieceType(p);
		pos = PieceBB[pt] & ColorBB[BLACK];

		while (pos) {
			from = pop_1st_bit(&pos);
			bscore += material_score[pt];

			bscore+=pcsq[pt][from];
			switch (pt) {
			case PAWN:
				attacks = attacksFrom<PAWN>(from);
				break;
			case KING:
				attacks = EmptyBoardBB;
				break;
			case KNIGHT:
				attacks = attacksFrom<KNIGHT>(from);
				break;
			case BISHOP:
				attacks = attacksFrom<BISHOP>(from);
				break;
			case ROOK:
				attacks = attacksFrom<ROOK>(from);
				break;
			case QUEEN:
				attacks = attacksFrom<QUEEN>(from);
				break;
            default:
                attacks = EmptyBoardBB;

			}

			if (attacks) {
				bscore += bitCnt(attacks & ColorBB[WHITE]) * 5;
				bscore += bitCnt(attacks & ColorBB[BLACK]) * 3;
				bscore += bitCnt(attacks & emptySquares()) * 2;
			}
		}
	}

	return player == WHITE ? (wscore - bscore) : (bscore - wscore);
}

int Board::eval_linear() {

     evaluationFeatures.compute(*this);
    float score = evaluationFeatureWeights->apply(evaluationFeatures);


    int int_score = int(round(score*100));

    return player == WHITE ? int_score: -int_score ;
}

int Board::eval_dnn() {

    evaluationFeatures.compute(*this);
    float score = dnn_model->apply(evaluationFeatures);

    int int_score = int(round(score*100));

    return player == WHITE ? int_score: -int_score ;
}


int Board::eval_l2() {

    evaluationFeatures.compute(*this);
    float l0score = evaluationFeatureWeights_log2_level0->apply(evaluationFeatures);
    const int clip = 5;

    int level = Min(Max(round(l0score), -clip), clip);

    float score=l0score, unclipped_score;
    auto l1_weight = evaluationFeatureWeights_log2_level1.find(level);
    if(l1_weight != evaluationFeatureWeights_log2_level1.end()){
        unclipped_score = l1_weight->second->apply(evaluationFeatures);

        score = Min(
			Max(unclipped_score, EvaluationFeatureWeights::score_ul_bound[level+clip][0]), 
			EvaluationFeatureWeights::score_ul_bound[level+clip][1]);
    }


    int int_score = int(round(score*100));

    return player == WHITE ? int_score: -int_score ;
}

map_str_int_t Board::eval_features() {
    evaluationFeatures.compute(*this);
    const map_str_int_t& features = evaluationFeatures.to_map();

    return features;
}
