/*
 * feature_weights.h
 *
 *  Created on: 23-Oct-2018
 *      Author: raghav
 */

#include "board.h"
#include <vector>

#include <Eigen/Dense>

#ifndef FEATURE_WEIGHTS_H
#define FEATURE_WEIGHTS_H

class Board;
class EvaluationFeatures;

typedef std::unordered_map<std::string, Eigen::VectorXf> map_str_vecf_t;


map_str_float_t file_to_feature_map(std::string filename);
map_str_float_t stream_to_feature_map(std::istream&);

void file_to_feature_maps(std::string filename, std::map<int,map_str_float_t>& maps);
void stream_to_feature_maps(std::istream& in, std::map<int,map_str_float_t>& maps);

class EvaluationFeatureWeights{
    
public:
    
    EvaluationFeatureWeights(){
    };
    
    EvaluationFeatureWeights(const std::string& filename);
    EvaluationFeatureWeights(std::istream& file);
    EvaluationFeatureWeights(const map_str_float_t& weight_map);

    EvaluationFeatureWeights& operator=(const EvaluationFeatureWeights&) = default;
    float apply(const EvaluationFeatures& features);
    
private:
    
    float n_material[16];
    float n_moves[16];
    float n_supports[16];
    float n_attacks[16];
    float n_fattacks[16];
    float n_rattacks[16];

    float m_attacks[16][16];
    float m_fattacks[16][16];
    
    float cm;
    float cs;
    float cx;
    
    float castles[2][4];
    
    float pieceSquaresTable[16][64];
    
public:
        
    constexpr static float score_ul_bound[11][2] = {
        //{-64., -32.},
        {-32., -16.},
        {-16.,  -8.},
        { -8.,  -4.},
        { -4.,  -2.},
        { -2.,  -1.},
        { -1.,   1.},
        {  1.,   2.},
        {  2.,   4.},
        {  4.,   8.},
        {  8.,  16.},
        { 16.,  32.}//,
        //{ 32.,  64.}
    };
    
};

void stream_to_feature_vec(std::istream& in, map_str_vecf_t& maps);
void stream_to_eigenmat(std::istream& in, Eigen::MatrixXf& mat);
void stream_to_eigen_rowvec(std::istream& in, Eigen::VectorXf& mat);
void stream_to_eigen_colvec(std::istream& in, Eigen::VectorXf& mat);

class EvaluationDnnModel{
    
public:
  
    EvaluationDnnModel(){};
    
    ~EvaluationDnnModel(){};
    
    EvaluationDnnModel(const map_str_vecf_t& weights,
                       const Eigen::VectorXf& l0_bias,
                       const std::vector<std::pair<Eigen::MatrixXf,Eigen::VectorXf>>& hidden_units,
                       const Eigen::VectorXf& reg_vec,
                       const float reg_bias);

    EvaluationDnnModel& operator=(const EvaluationDnnModel&) = default;

    float apply(const EvaluationFeatures& features);
    
    friend std::istream& operator>>(std::istream& in, EvaluationDnnModel& model);

    
//private:
    
    
    
    Eigen::VectorXf n_material[16];
    Eigen::VectorXf n_moves[16];
    Eigen::VectorXf n_supports[16];
    Eigen::VectorXf n_attacks[16];
    Eigen::VectorXf n_fattacks[16];
    Eigen::VectorXf n_rattacks[16];
    
    Eigen::VectorXf m_attacks[16][16];
    Eigen::VectorXf m_fattacks[16][16];
    
    Eigen::VectorXf cm;
    Eigen::VectorXf cs;
    Eigen::VectorXf cx;
    
    Eigen::VectorXf castles[2][4];
    
    Eigen::VectorXf pieceSquaresTable[16][64];
    
    Eigen::VectorXf l0_bias;
    
    std::vector<std::pair<Eigen::MatrixXf,Eigen::VectorXf>>  hidden_units;
    
    Eigen::VectorXf reg_vec;
    float reg_bias;

    
};

#endif
