/*
 * board.cpp
 *
 *  Created on: 11-Dec-2011
 *      Author: raghav
 */

#include "board.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include "feature_weights.h"


Board::Board(std::string model_folder): evaluationFeatureWeights{new EvaluationFeatureWeights(model_folder+"eval_weights.csv")} {
	gameStateIndex = -1;
	maxGameStateIndex = INIT_MAX_GAME_STACK;
	gameState = new GameState[maxGameStateIndex];

#ifdef USE_TT
    ttable.setsize(200);
#endif

    eval_weights = file_to_feature_map(model_folder+"eval_weights.csv");

    evaluationFeatureWeights_log2_level0 = new EvaluationFeatureWeights(model_folder+"eval_weights_l2.csv") ;

    std::map<int,map_str_float_t> eval_weights_log2_level1;
    file_to_feature_maps(model_folder+"eval_weights_l2c.csv", eval_weights_log2_level1);

    for(auto& pair: eval_weights_log2_level1){
        evaluationFeatureWeights_log2_level1[pair.first] = new EvaluationFeatureWeights(pair.second);
    }

    dnn_model  = new EvaluationDnnModel();
    std::ifstream model_file(model_folder+"dnn_relu.txt");
    model_file>>(*dnn_model);


}

Board::~Board() {
	delete[] gameState;
    delete evaluationFeatureWeights;
    delete evaluationFeatureWeights_log2_level0;
    for(auto& pair: evaluationFeatureWeights_log2_level1){
        delete pair.second;
    }

    delete dnn_model;

}

void Board::init() {

	for (int i = 0; i < 64; i++)
		board[i] = PIECE_NONE;

	board[SQ_E1] = WK;
	board[SQ_D1] = WQ;
	board[SQ_A1] = WR;
	board[SQ_H1] = WR;
	board[SQ_B1] = WN;
	board[SQ_G1] = WN;
	board[SQ_C1] = WB;
	board[SQ_F1] = WB;
	board[SQ_A2] = WP;
	board[SQ_B2] = WP;
	board[SQ_C2] = WP;
	board[SQ_D2] = WP;
	board[SQ_E2] = WP;
	board[SQ_F2] = WP;
	board[SQ_G2] = WP;
	board[SQ_H2] = WP;

	board[SQ_E8] = BK;
	board[SQ_D8] = BQ;
	board[SQ_A8] = BR;
	board[SQ_H8] = BR;
	board[SQ_B8] = BN;
	board[SQ_G8] = BN;
	board[SQ_C8] = BB;
	board[SQ_F8] = BB;
	board[SQ_A7] = BP;
	board[SQ_B7] = BP;
	board[SQ_C7] = BP;
	board[SQ_D7] = BP;
	board[SQ_E7] = BP;
	board[SQ_F7] = BP;
	board[SQ_G7] = BP;
	board[SQ_H7] = BP;

	initFromSquares(board, WHITE, 0, CastlingStart[0], CastlingStart[1],
			Square(SQ_NONE));

}

void Board::initFromSquares(Piece input[64], Color next, int fiftyM,
		Castling wcastle, Castling bcastle, Square epSq) {

	PieceType p;
	Color c;
	Bitboard bit = 1ULL;

	for (int x = 0; x < 8; x++)
		PieceBB[x] = 0ULL;

	for (int x = 0; x < 2; x++)
		ColorBB[x] = 0ULL;

	for (Square i = SQ_A1; i <= SQ_H8; i++) {
		board[i] = input[i];

		p = pieceType(input[i]);
		c = pieceColor(input[i]);

		if (p != PIECE_TYPE_NONE)
			PieceBB[p] |= bit;
		if (c == WHITE || c == BLACK)
			ColorBB[c] |= bit;

		bit <<= 1;

	}
	PieceBB[0] = ColorBB[0] | ColorBB[1]; // Occupied squares

	fiftyMoves = fiftyM;
	player = next;
	enpassant = epSq;
	canCastle[0] = wcastle;
	canCastle[1] = bcastle;

	pushGameState(0);

#ifdef USE_TT
    ttable.reset();
#endif

	lastMove = Move();
	hash = boardHash(this);

}


Move Board::bestMove() {
	return pv[0][0];
}

Move Board::createMove(const char* anm) {

	Square from = an_square(anm);
	Square to = an_square(anm + 2);
	Move m(0);
	SpecialMove sm = NORMAL_MOVE;
    Piece promotion(PIECE_NONE_DARK_SQ);// = Piece(0);
	switch (toupper(anm[4])) {
	case 'C':
		sm = CASTLE_MOVE;
		break;
	case 'E':
		sm = ENPASSANT_MOVE;
		break;
	case 'Q':
        sm = PROMOTION_MOVE;
		promotion = piece(QUEEN, player);
		break;
	case 'R':
        sm = PROMOTION_MOVE;
		promotion = piece(ROOK, player);
		break;
	case 'B':
        sm = PROMOTION_MOVE;
		promotion = piece(BISHOP, player);
		break;
	case 'N':
        sm = PROMOTION_MOVE;
		promotion = piece(KNIGHT, player);
		break;
	}
	m.set(from, to, sm, board[to]);
    if(promotion)
        m.promotion(promotion);
	return m;
}

bool Board::hasMoves() {

	ScoredMove sm[1000];
	ScoredMove* smend = generateMoves<MOVETYPE_NORMAL>(sm);

	bool haslegal = false;
	for (ScoredMove* m = sm; m < smend; m++) {
		if (makeMove(*m)) {
			haslegal = true;
			unmakeMove(*m);
		}
	}

	return haslegal;
}

bool Board::updateMove(const char* am) {

	Move targetm = createMove(am);
	ScoredMove sm[1000];
	ScoredMove* smend = generateMoves<MOVETYPE_NORMAL>(sm);

	for (ScoredMove* m = sm; m < smend; m++) {


        if ( (m->from() == targetm.from()) &&
            (m->to() == targetm.to()) &&
            (m->promotion() == targetm.promotion())) {

            bool moveRes = makeMove(*m);

			return moveRes;
		}
	}
	return false;
}

void Board::display(std::ostream& out)  const {
	Rank rank;
	File file;

	out << std::endl;
	{

		for (rank = RANK_8; rank >= RANK_1; rank--) {
			out << "    +---+---+---+---+---+---+---+---+" << std::endl;
			out << std::setw(3) << rank + 1 << " |";
			for (file = FILE_A; file <= FILE_H; file++) {
				out << " " << PIECENAMES[board[boardIndex(rank, file)]]
						<< " |";
			}
            out << std::endl;
		}
		out << "    +---+---+---+---+---+---+---+---+" << std::endl;
		out << "      A   B   C   D   E   F   G   H" << std::endl
				<< std::endl;

	}

}

void Board::displayBB() const {
	for (int x = 0; x < 8; x++)
		print_bitboard(PieceBB[x]);

	for (int x = 0; x < 2; x++)
		print_bitboard(ColorBB[x]);

}
