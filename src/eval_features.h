/*
 * eval_features.h
 *
 *  Created on: 19-Oct-2018
 *      Author: raghav
 */

#include "board.h"
#include <vector>

#ifndef EVAL_FEATURES_H
#define EVAL_FEATURES_H

class Board;

class EvaluationFeatures{

public:
    EvaluationFeatures();
    void inline reset();
    void compute(const Board& board);
    const map_str_int_t& to_map();

    int n_material[16];
    int n_moves[16];
    int n_supports[16];
    int n_attacks[16];
    int n_fattacks[16];
    int n_rattacks[16];
    int m_attacks[16][16];
    int m_fattacks[16][16];

    Square pieceSquares[16][8];
    int pieceSquaresCount[16];

    Castling castles[2];

    int cm;
    int cs;
    int cx;

private:

    // internal
    Bitboard c_attacks[2];
    Bitboard piece_attacks[16];

    map_str_int_t features;

    //Constant Defs
    const Rank pRank[2] = { RANK_7, RANK_2 };
    const Rank sRank[2] = { RANK_2, RANK_7 };
    const int delta[4] = { DELTA_N, DELTA_S, DELTA_NN, DELTA_SS };


    const int flip[64] = { 56, 57, 58, 59, 60, 61, 62, 63, 48, 49, 50, 51, 52, 53, 54, 55,
        40, 41, 42, 43, 44, 45, 46, 47, 32, 33, 34, 35, 36, 37, 38, 39, 24, 25,
        26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23, 8, 9, 10, 11,
        12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7 };

};

#endif
