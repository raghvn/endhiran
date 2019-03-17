/*
 * eval_features.cpp
 *
 *  Created on: 19-Oct-2018
 *      Author: raghav
 */

#include "eval_features.h"

EvaluationFeatures::EvaluationFeatures(): features(10000){

    reset();
}

void inline EvaluationFeatures::reset(){
    
    memset(n_material, 0, sizeof(n_material));
    memset(n_moves, 0, sizeof(n_moves));
    memset(n_supports, 0, sizeof(n_supports));
    memset(n_attacks, 0, sizeof(n_attacks));
    memset(n_rattacks, 0, sizeof(n_rattacks));
    memset(n_fattacks, 0, sizeof(n_fattacks));
    memset(c_attacks, 0, sizeof(c_attacks));
    memset(m_attacks, 0, sizeof(m_attacks));
    memset(m_fattacks, 0, sizeof(m_fattacks));
    memset(piece_attacks, 0, sizeof(piece_attacks));
    
    memset(pieceSquares, SQ_NONE, sizeof(pieceSquares));
    memset(pieceSquaresCount, 0, sizeof(pieceSquaresCount));

    cm = 0;
    cs = 0;
    cx = 0;
    
    castles[0] = CASTLING_NONE;
    castles[1] = CASTLING_NONE;


    
    features.clear();
}


void EvaluationFeatures::compute(const Board& board){
    
    reset();
    
    Square from, to;
    Bitboard attacks, attacks2;
    PieceType pt ,pt2;
    Bitboard pos, pos2, iter;
    Color color, color2;
    for (Piece p = WP; p <= BQ; p++) {
        pt = pieceType(p);
        color = pieceColor(p);
        iter = board.piecebb(pt,color);
        
        while (iter) {
            from = pop_1st_bit(&iter);
            
            switch (pt) {
                case PAWN:
                    attacks = color== WHITE ?
                    board.attacksFrom<PAWN, WHITE>(from):
                    board.attacksFrom<PAWN, BLACK>(from);
                    
                    {
                        Rank r = square_rank(from);
                        Bitboard push = (SetMaskBB[from + delta[color]] & board.emptySquares());
                        if ((r == sRank[color]) && push) {
                            push = push | (SetMaskBB[from + delta[color + 2]] & board.emptySquares());
                        }
                        n_moves[p] +=  bitCnt(push);
                    }
                    
                    break;
                case KING:
                    attacks = board.attacksFrom<KING>(from);
                    break;
                case KNIGHT:
                    attacks = board.attacksFrom<KNIGHT>(from);
                    break;
                case BISHOP:
                    attacks = board.attacksFrom<BISHOP>(from);
                    break;
                case ROOK:
                    attacks = board.attacksFrom<ROOK>(from);
                    break;
                case QUEEN:
                    attacks = board.attacksFrom<QUEEN>(from);
                    break;
                default:
                    attacks = EmptyBoardBB;
                    break;
            }
            
            piece_attacks[p] |= attacks;
            c_attacks[color] |= attacks;
        }
    }
    
    for (Piece p = WP; p <= BQ; p++) {
        pt = pieceType(p);
        color = pieceColor(p);
        pos = board.piecebb(pt,color);
        
        if(!pos)
            continue;
        attacks = piece_attacks[p];
        
        n_material[p] = bitCnt(pos);
        
        if(attacks){
            
            if(pt != PAWN)
                n_moves[p] += bitCnt(attacks & (~board.colorbb(color)));
            else
                n_moves[p] += bitCnt(attacks & board.colorbb(opposite_color(color)));
            
            n_supports[p] += bitCnt(attacks & board.colorbb(color));
            n_attacks[p] += bitCnt(attacks & board.colorbb(opposite_color(color)));
            
            n_fattacks[p] += bitCnt(attacks & c_attacks[opposite_color(color)] & board.emptySquares());
            
            for (Piece p2 = WP; p2 <= BQ; p2++) {
                pt2 = pieceType(p2);
                color2 = pieceColor(p2);
                pos2 = board.piecebb(pt2,color2);
                
                if(!pos2)
                    continue;
                
                m_attacks[p][p2] += bitCnt(attacks & pos2);
                
                attacks2 = piece_attacks[p2];
                if(attacks2){
                    int count = bitCnt(attacks & attacks2 & board.emptySquares());
                    m_fattacks[p][p2] += count;
                    
                    if(p2!=p)
                        n_rattacks[p] += count;
                }
            }
        }

        iter = board.piecebb(pt, color);
        
        while (iter) {
            from = pop_1st_bit(&iter);
            pieceSquares[p][pieceSquaresCount[p]++] = from;
        }
    }
    
    castles[WHITE] = board.castles(WHITE);
    castles[BLACK] = board.castles(BLACK);

    cm = bitCnt(c_attacks[WHITE]) - bitCnt(c_attacks[BLACK]);
    cs = bitCnt(c_attacks[WHITE] & board.colorbb(WHITE)) - bitCnt(c_attacks[BLACK] & board.colorbb(BLACK));
    cx = bitCnt(c_attacks[WHITE] & board.colorbb(BLACK)) - bitCnt(c_attacks[BLACK] & board.colorbb(WHITE));
    
}

const map_str_int_t& EvaluationFeatures::to_map(){
    
    Square from, to;
    Bitboard attacks, attacks2;
    PieceType pt ,pt2;
    Bitboard pos, pos2, iter;
    Color color, color2;
    
    //map_str_int_t features(1000);
    features.clear();
    
    char ptchar, ptchar2;
    int colorWeight, colorWeight2;
    for (Piece p = WP; p <= BQ; p++) {
        pt = pieceType(p);
        color = pieceColor(p);
        colorWeight = ColorWeight[color];
        
        ptchar = PIECENAMES[pt];
        if(ptchar==' ')
            continue;
        
        if(n_material[p])
            features[std::string({'n','v',ptchar})] += colorWeight*n_material[p];
        
        if(n_moves[p])
            features[std::string({'n','m',ptchar})] += colorWeight*n_moves[p];
        
        if(n_supports[p])
            features[std::string({'n','s',ptchar})] += colorWeight*n_supports[p];
        
        if(n_attacks[p])
            features[std::string({'n','x',ptchar})] += colorWeight*n_attacks[p];
        
        if(n_rattacks[p])
            features[std::string({'n','r',ptchar})] += colorWeight*n_rattacks[p];
        
        if(n_fattacks[p])
            features[std::string({'n','f',ptchar})] += colorWeight*n_fattacks[p];
        
        for (Piece p2 = WP; p2 <= BQ; p2++) {
            pt2 = pieceType(p2);
            color2 = pieceColor(p2);
            colorWeight2 = ColorWeight[color2];
            
            ptchar2 = PIECENAMES[pt2];
            if(ptchar2==' ')
                continue;
            
            if(color2!=color)
                ptchar2 = tolower(ptchar2);
            
            if(m_attacks[p][p2])
                features[std::string({'x',ptchar,ptchar2})] += colorWeight*m_attacks[p][p2];
            
            if(m_fattacks[p][p2])
                features[std::string({'f',ptchar,ptchar2})] += colorWeight*m_fattacks[p][p2];
        }
                
        const int psq_max_index = pieceSquaresCount[p];
        for(int psq_index=0; psq_index<psq_max_index; psq_index++ ){
            from = pieceSquares[p][psq_index];
            from = color==WHITE? from : Square(flip[from]);
            features[std::string({'s',SQCHAR[from][0],SQCHAR[from][1],PIECENAMES[pt]})] += ColorWeight[color];
        }
                
    }
    
    
    if(castles[WHITE] != CASTLING_NONE)
        features[std::string("k")+CASTLE_NAMES[castles[WHITE]]] += ColorWeight[WHITE];
    
    if(castles[BLACK] != CASTLING_NONE)
        features[std::string("k")+CASTLE_NAMES[castles[BLACK]]] += ColorWeight[BLACK];
    
    for(auto iter=features.begin(); iter != features.end(); ) {
        if (iter->second == 0) {
            features.erase(iter++);
        } else {
            ++iter;
        }
    }
    
    features[std::string({'c','m'})] += cm;
    features[std::string({'c','s'})] += cs;
    features[std::string({'c','x'})] += cx;
         
    
    return features;
}
