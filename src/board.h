/*
 * Board.h
 *
 *  Created on: 10-Dec-2011
 *      Author: raghav
 */

#ifndef BOARD_H_
#define BOARD_H_

#define USE_TT
#define USE_KM

#include "move.h"
#include "bitboard.h"
#include "defs.h"
#include "gamestack.h"
#include "hash.h"
#include "table.h"
#include <string>
#include <map>
#include "eval_features.h"
//#include "feature_weights.h"

class EvaluationFeatureWeights;
class EvaluationDnnModel;


class Board {

	Piece board[64];

	Bitboard PieceBB[8];
	Bitboard ColorBB[2];

	Color player;

	Square enpassant;
	Castling canCastle[2];
	int fiftyMoves;

	Move lastMove;

	uint64_t inodes;
	U64 hash;

	timer t;
	ScoredMove moveBuffer[MAX_MOVE_BUFFER];
	ScoredMove* moveBufLen[MAX_PLY];

	GameState *gameState;
	int gameStateIndex;
	int maxGameStateIndex;

    map_str_float_t eval_weights;

    EvaluationFeatures evaluationFeatures;
    EvaluationFeatureWeights* evaluationFeatureWeights;

    EvaluationFeatureWeights* evaluationFeatureWeights_log2_level0;
    std::map<int,EvaluationFeatureWeights*> evaluationFeatureWeights_log2_level1;

    EvaluationDnnModel* dnn_model;


#ifdef USE_TT
	Ttable ttable;
#endif
#ifdef USE_KM
	int mtable[64 * 64];
#endif

//bool	viewRotated;

#ifdef USE_TT
	Move pv[1][MAX_PLY];
	int pv_length[1];
#else
	Move pv[MAX_PLY][MAX_PLY];
	int pv_length[MAX_PLY];
#endif

public:
	//Board(std::string model_folder);//"/home/raghav/wrk/endhiran/model/");
	Board(std::string model_folder);//"/home/raghav/wrk/endhiran/model/");
	~Board();
	void init();
	void initFromSquares(Piece input[64], Color next, int fiftyM,
			Castling wcastle, Castling bcastle, Square epSq);

    void init_eval_weights(std::istream&);

	bool makeMove(Move move);
	bool unmakeMove(Move move);
	Move createMove(const char* anm);
	bool hasMoves();

	bool updateMove(const char* am);

	//inline Bitboard attacksFromPiece(PieceType pt, Square s) const;
	template<PieceType> inline Bitboard attacksFrom(Square s) const;
	template<PieceType, Color c> inline Bitboard attacksFrom(Square s) const;
	inline Bitboard attackersTo(Square s) const;
	inline Bitboard attackersTo(Bitboard squares) const;
	inline bool inCheck(Color c) const;

	int think(int searchDepth=PLY);
	int search(int ply, int depth, int alpha, int beta);
	int qsearch(int ply, int alpha, int beta);
	void sort(ScoredMove* from, ScoredMove* to);

	U64 perft(int depth);

	int eval_simple();
	int eval_linear();
	int eval_l2();
    int eval_dnn();
  	int (Board::*eval)() = &Board::eval_linear;
  map_str_int_t eval_features();

	Move bestMove();
    inline bool makeBestMove(){
        Move move = bestMove();
        return makeMove(move);
    }
#ifdef USE_TT
	void updatePvFromTt(int mply);
	void updatePvFromTt(int ply,int mply);
#endif

	inline Piece pieceOn(Square sq) const;
	inline PieceType pieceTypeOn(Square sq) const;
	inline Color colorOn(Square sq) const;
	inline Bitboard piecebb(PieceType pt) const;
	inline Bitboard piecebb(PieceType pt,Color c) const;
	inline Bitboard piecebb(PieceType pt1,PieceType pt2) const;
	inline Bitboard colorbb(Color c) const;

	inline Color turn() const;
	inline Color opponent() const;
	inline Castling castles(Color c) const;
	inline Square epsquare() const;
	inline int fiftymoves() const;

	inline U64 hashkey() const;

	inline Bitboard occupiedSquares() const;
	inline void updateOccupied();
	inline Bitboard emptySquares() const;
    void display(std::ostream& out = std::cout) const;
	void displayBB() const;
	inline void pushGameState(int m);
	inline int popGameState();
	inline int repeatCount() const;

	U64 nodes() {
		return inodes;
	}

	void readFen(const std::string& fen);

	template<MoveType mt> ScoredMove* generateMoves(ScoredMove* moves, int ply = 0) const;

	template<MoveType mt,PieceType pt> inline ScoredMove* generatePieceMoves(ScoredMove* moves, Bitboard target,Color c) const;
	template<MoveType mt,Color c> inline ScoredMove* generatePawnMoves(ScoredMove* moves) const;

	inline ScoredMove* generateCastlingMoves(ScoredMove* moves,Color c) const;

	inline ScoredMove* generatePromotionMoves(ScoredMove* moves, Square from,
			Bitboard attacks, Color c) const;

	template<MoveType mt> inline ScoredMove* addMoves(ScoredMove* moves,Square from, Bitboard attacks, SpecialMove sm = NORMAL_MOVE) const;
	template<MoveType mt> inline bool scoreMove(ScoredMove* moves, Square from, Square to) const;

	int see(Move m) const;
	int see_sign(Move m) const;
	int see_sign(Square from, Square to) const;
	int see(Square from, Square to) const;

    friend inline std::ostream& operator<<(std::ostream& out,const Board& m);
};

inline Bitboard Board::piecebb(PieceType pt) const {
	return PieceBB[pt];
}

inline Bitboard Board::piecebb(PieceType pt,Color c) const{
	return PieceBB[pt]&ColorBB[c];
}

inline Bitboard Board::piecebb(PieceType pt1, PieceType pt2) const {
	return (PieceBB[pt1] | PieceBB[pt2]);
}

inline Bitboard Board::colorbb(Color c) const {
	return ColorBB[c];
}

inline Piece Board::pieceOn(Square sq) const {
	return board[sq];
}

inline PieceType Board::pieceTypeOn(Square sq) const {
	return pieceType(board[sq]);
}

inline Color Board::colorOn(Square sq) const {
	return pieceColor(board[sq]);
}

inline Color Board::turn() const {
	return player;
}

inline Color Board::opponent() const {
	return opposite_color(player);
}

inline Castling Board::castles(Color c) const {
	return canCastle[c];
}

inline Square Board::epsquare() const {
	return enpassant;
}

inline U64 Board::hashkey() const {
	return hash;
}

inline int Board::fiftymoves() const {
	return fiftyMoves;
}

inline Bitboard Board::occupiedSquares() const {
	return PieceBB[0];
}

inline Bitboard Board::emptySquares() const {
	return ~PieceBB[0];
}

inline void Board::updateOccupied() {
	PieceBB[0] = ColorBB[0] | ColorBB[1];
}

inline void Board::pushGameState(int m) {
	gameStateIndex++;

	if (gameStateIndex >= maxGameStateIndex) {
		GameState* ngs = new GameState[maxGameStateIndex + INCR_MAX_GAME_STACK];GameState* oldgs = gameState;
		memcpy(ngs,oldgs,maxGameStateIndex*sizeof(GameState));
		gameState = ngs;
		delete[] oldgs;
		maxGameStateIndex += INCR_MAX_GAME_STACK;
	}
	gameState[gameStateIndex].move = m;
	gameState[gameStateIndex].canCastle[0] = canCastle[0];
	gameState[gameStateIndex].canCastle[1] = canCastle[1];
	gameState[gameStateIndex].enpassant = enpassant;
	gameState[gameStateIndex].fiftyMoves = fiftyMoves;
	gameState[gameStateIndex].hash = hash;
}

inline int Board::popGameState() {

	int m = 0;
	if (gameStateIndex >= 0) {
		m = gameState[gameStateIndex].move;
		canCastle[0] = gameState[gameStateIndex].canCastle[0];
		canCastle[1] = gameState[gameStateIndex].canCastle[1];
		enpassant = gameState[gameStateIndex].enpassant;
		fiftyMoves = gameState[gameStateIndex].fiftyMoves;
		hash = gameState[gameStateIndex].hash;

		gameStateIndex--;
	}
	return m;
}

template<> inline Bitboard Board::attacksFrom<ROOK>(Square s) const {
	return rook_attacks_bb(s, occupiedSquares());
}

template<> inline Bitboard Board::attacksFrom<BISHOP>(Square s) const {
	return bishop_attacks_bb(s, occupiedSquares());
}

template<> inline Bitboard Board::attacksFrom<QUEEN>(Square s) const {
	return queen_attacks_bb(s, occupiedSquares());
}

template<PieceType pt> inline Bitboard Board::attacksFrom(Square s) const {
	return StepAttacksBB[pt][s]; // + (nextToMove << 3)
}

template<PieceType pt, Color c> inline Bitboard Board::attacksFrom(
		Square s) const {
	return StepAttacksBB[piece(pt, c)][s];
}

inline Bitboard Board::attackersTo(Square s) const {

	return     (attacksFrom<PAWN, WHITE>(s) & (PieceBB[PAWN] & ColorBB[BLACK]) )
			| (attacksFrom<PAWN, BLACK>(s) & (PieceBB[PAWN] & ColorBB[WHITE]))
            | (attacksFrom<KNIGHT>(s) & PieceBB[KNIGHT])
			| (attacksFrom<ROOK>(s) & (PieceBB[ROOK] | PieceBB[QUEEN]))
			| (attacksFrom<BISHOP>(s) & (PieceBB[BISHOP] | PieceBB[QUEEN]))
			| (attacksFrom<KING>(s) & PieceBB[KING]);
}

inline Bitboard Board::attackersTo(Bitboard squares) const {
	Square to;
	Bitboard attacks = EmptyBoardBB;
	while (squares) {
		to = pop_1st_bit(&squares);
		attacks |= attackersTo(to);
	}
	return attacks;

}

inline bool Board::inCheck(Color c) const {
	Bitboard kbb = PieceBB[KING] & ColorBB[c];
	Square ksq = first_1(kbb);
	return (attackersTo(ksq) & ColorBB[opposite_color(c)]) != EmptyBoardBB;
}

inline int Board::repeatCount() const {

	int rep = 1;
	int ilast = gameStateIndex - fiftyMoves;
	for (int i = gameStateIndex - 1; i >= ilast; i -= 2) {
		if (gameState[i].hash == hash)
			rep++;
	}
	return rep;
}

std::ostream& operator<<(std::ostream& out,const Board& board){
    board.display(out);
    return out;
}

#endif /* BOARD_H_ */
