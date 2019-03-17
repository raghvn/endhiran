import pandas as pd
import numpy as np
import pyendhiran
import sys
from sklearn.externals import joblib

board_df = pd.read_pickle("data/train/board.pkl")
move_df = pd.read_pickle("data/train/move.pkl")

move_gdf = move_df.groupby('id')
move_df_cols = move_df.columns

board = pyendhiran.Board()

l2clip = 5
clip = np.power(2,l2clip)
l2clip_birange = 2*l2clip+1

l2_arr_lb = np.arange(-l2clip,l2clip)
l2_arr_ub = l2_arr_lb + 1
clip_pow2_ub_lb = np.array([np.sign(l2_arr_lb)*np.power(2.0,np.abs(l2_arr_lb)), np.sign(l2_arr_ub)*np.power(2.0,np.abs(l2_arr_ub))]).T
clip_pow2_ub_lb = np.insert(clip_pow2_ub_lb, l2clip, [-1,1],axis=0)
clip_pow2_ub_lb[l2clip-1,1] = -1
clip_pow2_ub_lb[l2clip+1,0] = 1

# wnp = pd.read_csv("data/eval_weights.csv",index_col=[0],header=None).sort_index().values[:,0]
# wnpl2 = pd.read_csv("data/eval_weights_l2.csv",index_col=[0],header=None).sort_index().values[:,0]
# wnpl2c = pd.read_csv("data/eval_weights_l2c.csv",index_col=[0]).sort_index().values.T

# def predict_np(x, w):
#     return np.dot(x, w)
#
# def predictc_np(x, w1, wc):
#     predl1 = np.dot(x, w1)
#     windex = (np.clip(np.round(predl1), -l2clip, l2clip) + l2clip).astype(np.int32)
#     predl1_l = clip_pow2_ub_lb[windex,0]
#     predl1_u = clip_pow2_ub_lb[windex,1]
#
#     wgather = wc[windex,:]
#     pred = np.sum(np.multiply(x,wgather))
#     pred = np.clip(pred, predl1_l, predl1_u)
#     return pred

reg_t = joblib.load('../../model/dnnreg.pkl')


cols = ['cm', 'cs', 'cx', 'fBB', 'fBK', 'fBN', 'fBP', 'fBQ', 'fBR', 'fBk', 'fBn', 'fBp', 'fBq', 'fBr', 'fKB', 'fKK', 'fKN', 'fKP', 'fKQ', 'fKR', 'fKb', 'fKn', 'fKp', 'fKq', 'fKr', 'fNB', 'fNK', 'fNN', 'fNP', 'fNQ', 'fNR', 'fNb', 'fNk', 'fNp', 'fNq', 'fNr', 'fPB', 'fPK', 'fPN', 'fPP', 'fPQ', 'fPR', 'fPb', 'fPk', 'fPn', 'fPq', 'fPr', 'fQB', 'fQK', 'fQN', 'fQP', 'fQQ', 'fQR', 'fQb', 'fQk', 'fQn', 'fQp', 'fQr', 'fRB', 'fRK', 'fRN', 'fRP', 'fRQ', 'fRR', 'fRb', 'fRk', 'fRn', 'fRp', 'fRq', 'kOO', 'kOOO', 'kOO_OOO', 'nfB', 'nfK', 'nfN', 'nfP', 'nfQ', 'nfR', 'nmB', 'nmK', 'nmN', 'nmP', 'nmQ', 'nmR', 'nrB', 'nrK', 'nrN', 'nrP', 'nrQ', 'nrR', 'nsB', 'nsK', 'nsN', 'nsP', 'nsQ', 'nsR', 'nvB', 'nvN', 'nvP', 'nvQ', 'nvR', 'nxB', 'nxK', 'nxN', 'nxP', 'nxQ', 'nxR', 'sA1B', 'sA1K', 'sA1N', 'sA1Q', 'sA1R', 'sA2B', 'sA2K', 'sA2N', 'sA2P', 'sA2Q', 'sA2R', 'sA3B', 'sA3K', 'sA3N', 'sA3P', 'sA3Q', 'sA3R', 'sA4B', 'sA4K', 'sA4N', 'sA4P', 'sA4Q', 'sA4R', 'sA5B', 'sA5K', 'sA5N', 'sA5P', 'sA5Q', 'sA5R', 'sA6B', 'sA6K', 'sA6N', 'sA6P', 'sA6Q', 'sA6R', 'sA7B', 'sA7K', 'sA7N', 'sA7P', 'sA7Q', 'sA7R', 'sA8B', 'sA8K', 'sA8N', 'sA8Q', 'sA8R', 'sB1B', 'sB1K', 'sB1N', 'sB1Q', 'sB1R', 'sB2B', 'sB2K', 'sB2N', 'sB2P', 'sB2Q', 'sB2R', 'sB3B', 'sB3K', 'sB3N', 'sB3P', 'sB3Q', 'sB3R', 'sB4B', 'sB4K', 'sB4N', 'sB4P', 'sB4Q', 'sB4R', 'sB5B', 'sB5K', 'sB5N', 'sB5P', 'sB5Q', 'sB5R', 'sB6B', 'sB6K', 'sB6N', 'sB6P', 'sB6Q', 'sB6R', 'sB7B', 'sB7K', 'sB7N', 'sB7P', 'sB7Q', 'sB7R', 'sB8B', 'sB8K', 'sB8N', 'sB8Q', 'sB8R', 'sC1B', 'sC1K', 'sC1N', 'sC1Q', 'sC1R', 'sC2B', 'sC2K', 'sC2N', 'sC2P', 'sC2Q', 'sC2R', 'sC3B', 'sC3K', 'sC3N', 'sC3P', 'sC3Q', 'sC3R', 'sC4B', 'sC4K', 'sC4N', 'sC4P', 'sC4Q', 'sC4R', 'sC5B', 'sC5K', 'sC5N', 'sC5P', 'sC5Q', 'sC5R', 'sC6B', 'sC6K', 'sC6N', 'sC6P', 'sC6Q', 'sC6R', 'sC7B', 'sC7K', 'sC7N', 'sC7P', 'sC7Q', 'sC7R', 'sC8B', 'sC8K', 'sC8N', 'sC8Q', 'sC8R', 'sD1B', 'sD1K', 'sD1N', 'sD1Q', 'sD1R', 'sD2B', 'sD2K', 'sD2N', 'sD2P', 'sD2Q', 'sD2R', 'sD3B', 'sD3K', 'sD3N', 'sD3P', 'sD3Q', 'sD3R', 'sD4B', 'sD4K', 'sD4N', 'sD4P', 'sD4Q', 'sD4R', 'sD5B', 'sD5K', 'sD5N', 'sD5P', 'sD5Q', 'sD5R', 'sD6B', 'sD6K', 'sD6N', 'sD6P', 'sD6Q', 'sD6R', 'sD7B', 'sD7K', 'sD7N', 'sD7P', 'sD7Q', 'sD7R', 'sD8B', 'sD8K', 'sD8N', 'sD8Q', 'sD8R', 'sE1B', 'sE1K', 'sE1N', 'sE1Q', 'sE1R', 'sE2B', 'sE2K', 'sE2N', 'sE2P', 'sE2Q', 'sE2R', 'sE3B', 'sE3K', 'sE3N', 'sE3P', 'sE3Q', 'sE3R', 'sE4B', 'sE4K', 'sE4N', 'sE4P', 'sE4Q', 'sE4R', 'sE5B', 'sE5K', 'sE5N', 'sE5P', 'sE5Q', 'sE5R', 'sE6B', 'sE6K', 'sE6N', 'sE6P', 'sE6Q', 'sE6R', 'sE7B', 'sE7K', 'sE7N', 'sE7P', 'sE7Q', 'sE7R', 'sE8B', 'sE8K', 'sE8N', 'sE8Q', 'sE8R', 'sF1B', 'sF1K', 'sF1N', 'sF1Q', 'sF1R', 'sF2B', 'sF2K', 'sF2N', 'sF2P', 'sF2Q', 'sF2R', 'sF3B', 'sF3K', 'sF3N', 'sF3P', 'sF3Q', 'sF3R', 'sF4B', 'sF4K', 'sF4N', 'sF4P', 'sF4Q', 'sF4R', 'sF5B', 'sF5K', 'sF5N', 'sF5P', 'sF5Q', 'sF5R', 'sF6B', 'sF6K', 'sF6N', 'sF6P', 'sF6Q', 'sF6R', 'sF7B', 'sF7K', 'sF7N', 'sF7P', 'sF7Q', 'sF7R', 'sF8B', 'sF8K', 'sF8N', 'sF8Q', 'sF8R', 'sG1B', 'sG1K', 'sG1N', 'sG1Q', 'sG1R', 'sG2B', 'sG2K', 'sG2N', 'sG2P', 'sG2Q', 'sG2R', 'sG3B', 'sG3K', 'sG3N', 'sG3P', 'sG3Q', 'sG3R', 'sG4B', 'sG4K', 'sG4N', 'sG4P', 'sG4Q', 'sG4R', 'sG5B', 'sG5K', 'sG5N', 'sG5P', 'sG5Q', 'sG5R', 'sG6B', 'sG6K', 'sG6N', 'sG6P', 'sG6Q', 'sG6R', 'sG7B', 'sG7K', 'sG7N', 'sG7P', 'sG7Q', 'sG7R', 'sG8B', 'sG8K', 'sG8N', 'sG8Q', 'sG8R', 'sH1B', 'sH1K', 'sH1N', 'sH1Q', 'sH1R', 'sH2B', 'sH2K', 'sH2N', 'sH2P', 'sH2Q', 'sH2R', 'sH3B', 'sH3K', 'sH3N', 'sH3P', 'sH3Q', 'sH3R', 'sH4B', 'sH4K', 'sH4N', 'sH4P', 'sH4Q', 'sH4R', 'sH5B', 'sH5K', 'sH5N', 'sH5P', 'sH5Q', 'sH5R', 'sH6B', 'sH6K', 'sH6N', 'sH6P', 'sH6Q', 'sH6R', 'sH7B', 'sH7K', 'sH7N', 'sH7P', 'sH7Q', 'sH7R', 'sH8B', 'sH8K', 'sH8N', 'sH8Q', 'sH8R', 'xBK', 'xBN', 'xBP', 'xBQ', 'xBR', 'xBk', 'xBn', 'xBp', 'xBq', 'xBr', 'xKB', 'xKN', 'xKP', 'xKQ', 'xKR', 'xKb', 'xKn', 'xKp', 'xKq', 'xKr', 'xNB', 'xNK', 'xNN', 'xNP', 'xNQ', 'xNR', 'xNb', 'xNk', 'xNn', 'xNp', 'xNq', 'xNr', 'xPB', 'xPK', 'xPN', 'xPP', 'xPQ', 'xPR', 'xPb', 'xPk', 'xPn', 'xPp', 'xPq', 'xPr', 'xQB', 'xQK', 'xQN', 'xQP', 'xQQ', 'xQR', 'xQb', 'xQk', 'xQn', 'xQp', 'xQq', 'xQr', 'xRB', 'xRK', 'xRN', 'xRP', 'xRQ', 'xRR', 'xRb', 'xRk', 'xRn', 'xRp', 'xRq', 'xRr']

cols_map = dict([(v,k) for k,v in enumerate(cols)])


for grp_index, (name, grp) in enumerate(move_gdf):

    board.init()


    # features_o = board.eval_features()
    # features = np.nan_to_num(np.array([ float(features_o[col]) if col in features_o else 0.0 for col in cols]))
    #
    # pyend_score = board.eval()
    # pyend_score = -pyend_score if board.turn()==1 else pyend_score
    # np_score = int(np.round(reg_t.predict(np.reshape(features,(1,543)))*100))
    #
    # print(str(board.turn())+" "+str(np_score)+"/"+str(pyend_score))
    #
    # break

    for row_index,row in grp.iterrows():
        board.updateMove(row['move'])
        score = np.clip(np.nan_to_num(float(row['score'])),-clip,clip)
        features_o = board.eval_features()
        features = np.nan_to_num(np.array([ float(features_o[col]) if col in features_o else 0.0 for col in cols]))

        pyend_score = board.eval()
        pyend_score = -pyend_score if board.turn()==1 else pyend_score
        np_score = int(np.round(reg_t.predict(np.reshape(features,(1,543)))*100))

        if np_score!=pyend_score:
            print(str(board.turn())+" "+str(np_score)+"/"+str(pyend_score)+"/"+str(score))
