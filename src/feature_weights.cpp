/*
 * feature_weights.cpp
 *
 *  Created on: 23-Oct-2018
 *      Author: raghav
 */

#include "feature_weights.h"
#include<fstream>
#include <sstream>

constexpr float EvaluationFeatureWeights::score_ul_bound[11][2];

map_str_float_t file_to_feature_map(std::string filename){
    std::ifstream in(filename);
    
    map_str_float_t eval_weights = stream_to_feature_map(in);
    
    return eval_weights;
}

map_str_float_t stream_to_feature_map(std::istream& in){
    std::string line;
    map_str_float_t eval_weights;
    while(!in.eof()){
        std::getline(in, line);
        int spoint = line.find(",");
        if(spoint< line.size())
            eval_weights[line.substr(0,spoint)] = std::stof(line.substr(spoint+1));
    }
  
    return eval_weights;
}

void file_to_feature_maps(std::string filename, std::map<int,map_str_float_t>& maps){
    std::ifstream in(filename);
    stream_to_feature_maps(in, maps);
}

void stream_to_feature_maps(std::istream& in, std::map<int,map_str_float_t>& maps){

    std::vector<int> header;

    std::string line;
    //header
    if(!in.eof()){
        std::getline(in, line);
        std::stringstream buf(line);
        std::string cell;
        while(std::getline(buf, cell , ',')){
            if(!cell.empty())
                header.push_back(std::stoi(cell));
        }
    }
    size_t n_cols = header.size();
    for(auto& cell: header){
        maps[cell] = map_str_float_t();
    }
    
    while(!in.eof()){
        std::getline(in, line);
        std::stringstream buf(line);
        std::string cell;
        int index = 0;
        std::string row_key;
        while(std::getline(buf, cell , ',')){
            //row.push_back(cell);
            if(index==0){
                row_key = cell;
            }else{
                maps[header[index-1]][row_key] = std::stof(cell);
            }
            
            index++;
        }

    }
  
}

void stream_to_feature_vec(std::istream& in, map_str_vecf_t& maps){
    
    std::string line=" ";

    while(!(in.eof() || line.empty())){
        std::getline(in, line);

        if(line.empty())
            break;
        
        
        std::stringstream buf(line);
        std::string cell;
        int index = 0;
        std::string row_key;
        std::vector<float> vec;
        while(std::getline(buf, cell , ',')){

            if(index==0){
                row_key = cell;
            }else{
                 vec.push_back(stod(cell));
            }
            
            index++;
        }
        
        
        maps[row_key] = Eigen::VectorXf(vec.size());
        for(int i=0;i<vec.size();i++)
            maps[row_key][i] = vec[i];
        
    }
    
}

void stream_to_eigenmat(std::istream& in, Eigen::MatrixXf& mat){
    std::string line;
    std::getline(in, line);
    std::stringstream stream(line);
    size_t nrow, ncol;

    stream>>nrow>>ncol;

    mat.resize(nrow, ncol);
    
    size_t row = 0, col;
    std::string cell;
    while(!(line.empty() || in.eof())){
        std::getline(in, line);
        std::stringstream buf(line);

        col = 0;
        while(std::getline(buf, cell , ',')){
            mat(row, col) = stod(cell);
            col++;
        }
        row++;
        
    }            
}

void stream_to_eigen_colvec(std::istream& in, Eigen::VectorXf& vec){
    std::string line;
    std::getline(in, line);
    std::stringstream stream(line);
    size_t nrow;
    
    stream>>nrow;
    
    vec.resize(nrow);
    size_t row = 0;
    while(!(line.empty() || in.eof())){
        std::getline(in, line);
        if(line.empty())
            break;
        vec(row) = stod(line);
        row++;
    }
    
}

void stream_to_eigen_rowvec(std::istream& in, Eigen::VectorXf& vec){
    std::string line;
    std::getline(in, line);
    std::stringstream stream(line);
    size_t nrow;
    
    stream>>nrow;
    
    vec.resize(nrow);
    
    if(!(line.empty() || in.eof())){
        std::getline(in, line);
        std::stringstream buf(line);
        std::string cell;
        std::string row_key;
        size_t row = 0;
        while(std::getline(buf, cell , ',')){
            vec(row) = stod(cell);
            row++;
        }
    }
    
}

EvaluationFeatureWeights::EvaluationFeatureWeights(const std::string& filename): EvaluationFeatureWeights(file_to_feature_map(filename)){
    
}
EvaluationFeatureWeights::EvaluationFeatureWeights(std::istream& file): EvaluationFeatureWeights(stream_to_feature_map(file)) {
    
}

EvaluationFeatureWeights::EvaluationFeatureWeights(const map_str_float_t& weights) {
    
    memset(n_material, 0, sizeof(n_material));
    memset(n_moves, 0, sizeof(n_moves));
    memset(n_supports, 0, sizeof(n_supports));
    memset(n_attacks, 0, sizeof(n_attacks));
    memset(n_rattacks, 0, sizeof(n_rattacks));
    memset(n_fattacks, 0, sizeof(n_fattacks));

    memset(m_attacks, 0, sizeof(m_attacks));
    memset(m_fattacks, 0, sizeof(m_fattacks));

    memset(pieceSquaresTable, 0, sizeof(pieceSquaresTable));
    
    memset(castles, 0, sizeof(castles));
    
    cm = 0;
    cs = 0;
    cx = 0;
    
    Square from, to;
    Bitboard attacks, attacks2;
    PieceType pt ,pt2;
    Bitboard pos, pos2, iter;
    Color color, color2;
    
    
    char ptchar, ptchar2;
    int colorWeight, colorWeight2;
    for (Piece p = WP; p <= BQ; p++) {
        pt = pieceType(p);
        color = pieceColor(p);
        colorWeight = ColorWeight[color];
        
        ptchar = PIECENAMES[pt];
        if(ptchar==' ')
            continue;

        
        try{
            n_material[p] += colorWeight*weights.at(std::string({'n','v',ptchar}));
        }catch(std::out_of_range ex){
        }
   
        try{
            n_moves[p] += colorWeight*weights.at(std::string({'n','m',ptchar}));
        }catch(std::out_of_range ex){
        }

        try{
            n_supports[p] += colorWeight*weights.at(std::string({'n','s',ptchar}));
        }catch(std::out_of_range ex){
        }

        try{
            n_attacks[p] += colorWeight*weights.at(std::string({'n','x',ptchar}));
        }catch(std::out_of_range ex){
        }

        try{
            n_rattacks[p] += colorWeight*weights.at(std::string({'n','r',ptchar}));
        }catch(std::out_of_range ex){
        }

        try{
            n_fattacks[p] += colorWeight*weights.at(std::string({'n','f',ptchar}));
        }catch(std::out_of_range ex){
        }

        for (Piece p2 = WP; p2 <= BQ; p2++) {
            pt2 = pieceType(p2);
            color2 = pieceColor(p2);
            colorWeight2 = ColorWeight[color2];
            
            ptchar2 = PIECENAMES[pt2];
            if(ptchar2==' ')
                continue;
            
            if(color2!=color)
                ptchar2 = tolower(ptchar2);
            
            try{
                m_attacks[p][p2] += colorWeight*weights.at(std::string({'x',ptchar,ptchar2}));
            }catch(std::out_of_range ex){
            }

            try{
                m_fattacks[p][p2] += colorWeight*weights.at(std::string({'f',ptchar,ptchar2}));
            }catch(std::out_of_range ex){
            }
        }
        
        for (Square square = SQ_A1; square <= SQ_H8; square++) {
            Square from = square;
            from = color==WHITE? from : Square(flip[from]);
            float square_weight = 0.0;
            try{
                square_weight += colorWeight*weights.at(std::string({'s',SQCHAR[from][0],SQCHAR[from][1],ptchar}));
            }catch(std::out_of_range ex){
            }
            
            pieceSquaresTable[p][square] += square_weight;
        }
        
    }
    
    
    for(Color color = WHITE; color<=BLACK; color++)
        for(Castling castling = CASTLING_OO; castling<=CASTLING_OO_OOO; castling++)
            castles[color][castling] = ColorWeight[color]*weights.at(std::string("k")+CASTLE_NAMES[castling]);
    
    cm += weights.at(std::string({'c','m'}));
    cs += weights.at(std::string({'c','s'}));
    cx += weights.at(std::string({'c','x'}));
    
}

std::istream& operator>>(std::istream& in, EvaluationDnnModel& model){
    map_str_vecf_t weights;
    Eigen::VectorXf l0_bias;
    std::vector<std::pair<Eigen::MatrixXf,Eigen::VectorXf>> hidden_units;
    Eigen::VectorXf reg_vec;
    double reg_bias;
    
    stream_to_feature_vec(in, weights);
    
    stream_to_eigen_colvec(in, l0_bias);
        
    size_t n_hidden;
    in>>n_hidden;
    in.get();
    
    for(int i =0; i < n_hidden; i++){
        Eigen::MatrixXf weight;
        stream_to_eigenmat(in, weight);
        Eigen::VectorXf bias;
        stream_to_eigen_colvec(in, bias);
        
        hidden_units.push_back(std::make_pair(weight.transpose(), bias));
    }
    
    stream_to_eigen_colvec(in, reg_vec);
    std::string line;
    std::getline(in, line);
    in>>reg_bias;
    
    EvaluationDnnModel em(weights,l0_bias,hidden_units,reg_vec,reg_bias);
    
    model = em;
    
    return in;
    
}



EvaluationDnnModel::EvaluationDnnModel(const map_str_vecf_t& weights,
                                       const Eigen::VectorXf& l0_bias,
                                       const std::vector<std::pair<Eigen::MatrixXf,Eigen::VectorXf>>& hidden_units,
                                       const Eigen::VectorXf& reg_vec,
                                       const float reg_bias) :
    l0_bias{l0_bias}, hidden_units{hidden_units}, reg_vec{reg_vec}, reg_bias{reg_bias}{
    

        
    size_t l0_vec_size = weights.begin()->second.size();
                
    for(int i = 0; i< 16; i++){
        n_material[i] = Eigen::VectorXf::Zero(l0_vec_size);
        n_moves[i] = Eigen::VectorXf::Zero(l0_vec_size);
        n_supports[i] = Eigen::VectorXf::Zero(l0_vec_size);
        n_attacks[i] = Eigen::VectorXf::Zero(l0_vec_size);
        n_rattacks[i] = Eigen::VectorXf::Zero(l0_vec_size);
        n_fattacks[i] = Eigen::VectorXf::Zero(l0_vec_size);
        
        for(int j = 0; j< 16; j++){
            m_attacks[i][j] = Eigen::VectorXf::Zero(l0_vec_size);
            m_fattacks[i][j] = Eigen::VectorXf::Zero(l0_vec_size);
        }
        
        for(int j = 0; j< 64; j++){
            pieceSquaresTable[i][j] = Eigen::VectorXf::Zero(l0_vec_size);
        }
    }
        
    for(int i = 0; i< 2; i++){
        for(int j = 0; j< 4; j++){
            castles[i][j] = Eigen::VectorXf::Zero(l0_vec_size);
        }
    }
    
    
    cm = Eigen::VectorXf::Zero(l0_vec_size);
    cs = Eigen::VectorXf::Zero(l0_vec_size);
    cx = Eigen::VectorXf::Zero(l0_vec_size);
    
    Square from, to;
    Bitboard attacks, attacks2;
    PieceType pt ,pt2;
    Bitboard pos, pos2, iter;
    Color color, color2;
    
    
    char ptchar, ptchar2;
    int colorWeight, colorWeight2;
    for (Piece p = WP; p <= BQ; p++) {
        pt = pieceType(p);
        color = pieceColor(p);
        colorWeight = ColorWeight[color];
        
        ptchar = PIECENAMES[pt];
        if(ptchar==' ')
            continue;
        
        
        try{
            n_material[p] += colorWeight*weights.at(std::string({'n','v',ptchar}));
        }catch(std::out_of_range ex){
        }
        
        try{
            n_moves[p] += colorWeight*weights.at(std::string({'n','m',ptchar}));
        }catch(std::out_of_range ex){
        }
        
        try{
            n_supports[p] += colorWeight*weights.at(std::string({'n','s',ptchar}));
        }catch(std::out_of_range ex){
        }
        
        try{
            n_attacks[p] += colorWeight*weights.at(std::string({'n','x',ptchar}));
        }catch(std::out_of_range ex){
        }
        
        try{
            n_rattacks[p] += colorWeight*weights.at(std::string({'n','r',ptchar}));
        }catch(std::out_of_range ex){
        }
        
        try{
            n_fattacks[p] += colorWeight*weights.at(std::string({'n','f',ptchar}));
        }catch(std::out_of_range ex){
        }
        
        for (Piece p2 = WP; p2 <= BQ; p2++) {
            pt2 = pieceType(p2);
            color2 = pieceColor(p2);
            colorWeight2 = ColorWeight[color2];
            
            ptchar2 = PIECENAMES[pt2];
            if(ptchar2==' ')
                continue;
            
            if(color2!=color)
                ptchar2 = tolower(ptchar2);
            
            try{
                m_attacks[p][p2] += colorWeight*weights.at(std::string({'x',ptchar,ptchar2}));
            }catch(std::out_of_range ex){
            }
            
            try{
                m_fattacks[p][p2] += colorWeight*weights.at(std::string({'f',ptchar,ptchar2}));
            }catch(std::out_of_range ex){
            }
        }
        
        for (Square square = SQ_A1; square <= SQ_H8; square++) {
            Square from = square;
            from = color==WHITE? from : Square(flip[from]);
            Eigen::VectorXf square_weight = Eigen::VectorXf::Zero(l0_vec_size);
            try{
                square_weight += colorWeight*weights.at(std::string({'s',SQCHAR[from][0],SQCHAR[from][1],ptchar}));
            }catch(std::out_of_range ex){
            }
            
            pieceSquaresTable[p][square] += square_weight;
        }
        
    }
    
    
    for(Color color = WHITE; color<=BLACK; color++)
        for(Castling castling = CASTLING_OO; castling<=CASTLING_OO_OOO; castling++)
            castles[color][castling] = ColorWeight[color]*weights.at(std::string("k")+CASTLE_NAMES[castling]);
    
    cm += weights.at(std::string({'c','m'}));
    cs += weights.at(std::string({'c','s'}));
    cx += weights.at(std::string({'c','x'}));
            
}


float EvaluationFeatureWeights::apply(const EvaluationFeatures& features){

    double score_ = 0;
    
    for (Piece p = WP; p <= BQ; p++) {
        
        if(p%4 == 0)
            continue;

        score_ += n_material[p]*features.n_material[p];
        score_ += n_moves[p]*features.n_moves[p];
        score_ += n_supports[p]*features.n_supports[p];
        score_ += n_attacks[p]*features.n_attacks[p];
        score_ += n_rattacks[p]*features.n_rattacks[p];
        score_ += n_fattacks[p]*features.n_fattacks[p];

        for (Piece p2 = WP; p2 <= BQ; p2++) {

            if(p2%4 == 0)
                continue;
            
            score_ += m_attacks[p][p2]*features.m_attacks[p][p2];
            score_ += m_fattacks[p][p2]*features.m_fattacks[p][p2];
        }

        for(int i = 0; i < features.pieceSquaresCount[p]; ++i )
            score_ += pieceSquaresTable[p][features.pieceSquares[p][i]];
        
    }
    
    
    for(Color color = WHITE; color <= BLACK; color++)
        score_ += castles[color][features.castles[color]];
    
    score_ += cm*features.cm;
    score_ += cs*features.cs;
    score_ += cx*features.cx;
    
    return score_;
    
}

float EvaluationDnnModel::apply(const EvaluationFeatures& features){
    
    Eigen::VectorXf score_ = l0_bias;
    
    for (Piece p = WP; p <= BQ; p++) {
        
        if(p%4 == 0)
            continue;
        
        score_ += n_material[p]*features.n_material[p];
        score_ += n_moves[p]*features.n_moves[p];
        score_ += n_supports[p]*features.n_supports[p];
        score_ += n_attacks[p]*features.n_attacks[p];
        score_ += n_rattacks[p]*features.n_rattacks[p];
        score_ += n_fattacks[p]*features.n_fattacks[p];
        
        for (Piece p2 = WP; p2 <= BQ; p2++) {

            if(p2%4 == 0)
                continue;

            score_ += m_attacks[p][p2]*features.m_attacks[p][p2];
            score_ += m_fattacks[p][p2]*features.m_fattacks[p][p2];
        }
        
        for(int i = 0; i < features.pieceSquaresCount[p]; ++i )
            score_ += pieceSquaresTable[p][features.pieceSquares[p][i]];
        
    }
    
    
    score_ += castles[WHITE][features.castles[WHITE]];
    score_ += castles[BLACK][features.castles[BLACK]];

    score_ += cm*features.cm;
    score_ += cs*features.cs;
    score_ += cx*features.cx;

    
    Eigen::VectorXf act =  score_.cwiseMax(0.0);
    
    for(auto& pair: hidden_units){
        act = (pair.first*act + pair.second).cwiseMax(0.0);
    }

    
    float reg_score = act.dot(reg_vec) + reg_bias;
    
    return reg_score;
    
}
