##
#  feature_gen.py
#  Script to generate features from processed 
#  game data and save it in TFRecord format.
#  Author: Raghav
##



import pandas as pd
import numpy as np
import pyendhiran
import sys
import copy
import json
import tensorflow as tf

board_df = pd.read_pickle("data/train/board.pkl")
move_df = pd.read_pickle("data/train/move.pkl")

move_gdf = move_df.groupby('id')
move_df_cols = move_df.columns

# features_df = pd.DataFrame()#index=['id','mid'])

board = pyendhiran.Board()
# features_list = []
move_hashes = set()
#duplicate_counter = 0

cols = ['cm', 'cs', 'cx', 'fBB', 'fBK', 'fBN', 'fBP', 'fBQ', 'fBR', 'fBk', 'fBn', 'fBp', 'fBq', 'fBr', 'fKB', 'fKK', 'fKN', 'fKP', 'fKQ', 'fKR', 'fKb', 'fKn', 'fKp', 'fKq', 'fKr', 'fNB', 'fNK', 'fNN', 'fNP', 'fNQ', 'fNR', 'fNb', 'fNk', 'fNp', 'fNq', 'fNr', 'fPB', 'fPK', 'fPN', 'fPP', 'fPQ', 'fPR', 'fPb', 'fPk', 'fPn', 'fPq', 'fPr', 'fQB', 'fQK', 'fQN', 'fQP', 'fQQ', 'fQR', 'fQb', 'fQk', 'fQn', 'fQp', 'fQr', 'fRB', 'fRK', 'fRN', 'fRP', 'fRQ', 'fRR', 'fRb', 'fRk', 'fRn', 'fRp', 'fRq', 'kOO', 'kOOO', 'kOO_OOO', 'nfB', 'nfK', 'nfN', 'nfP', 'nfQ', 'nfR', 'nmB', 'nmK', 'nmN', 'nmP', 'nmQ', 'nmR', 'nrB', 'nrK', 'nrN', 'nrP', 'nrQ', 'nrR', 'nsB', 'nsK', 'nsN', 'nsP', 'nsQ', 'nsR', 'nvB', 'nvN', 'nvP', 'nvQ', 'nvR', 'nxB', 'nxK', 'nxN', 'nxP', 'nxQ', 'nxR', 'sA1B', 'sA1K', 'sA1N', 'sA1Q', 'sA1R', 'sA2B', 'sA2K', 'sA2N', 'sA2P', 'sA2Q', 'sA2R', 'sA3B', 'sA3K', 'sA3N', 'sA3P', 'sA3Q', 'sA3R', 'sA4B', 'sA4K', 'sA4N', 'sA4P', 'sA4Q', 'sA4R', 'sA5B', 'sA5K', 'sA5N', 'sA5P', 'sA5Q', 'sA5R', 'sA6B', 'sA6K', 'sA6N', 'sA6P', 'sA6Q', 'sA6R', 'sA7B', 'sA7K', 'sA7N', 'sA7P', 'sA7Q', 'sA7R', 'sA8B', 'sA8K', 'sA8N', 'sA8Q', 'sA8R', 'sB1B', 'sB1K', 'sB1N', 'sB1Q', 'sB1R', 'sB2B', 'sB2K', 'sB2N', 'sB2P', 'sB2Q', 'sB2R', 'sB3B', 'sB3K', 'sB3N', 'sB3P', 'sB3Q', 'sB3R', 'sB4B', 'sB4K', 'sB4N', 'sB4P', 'sB4Q', 'sB4R', 'sB5B', 'sB5K', 'sB5N', 'sB5P', 'sB5Q', 'sB5R', 'sB6B', 'sB6K', 'sB6N', 'sB6P', 'sB6Q', 'sB6R', 'sB7B', 'sB7K', 'sB7N', 'sB7P', 'sB7Q', 'sB7R', 'sB8B', 'sB8K', 'sB8N', 'sB8Q', 'sB8R', 'sC1B', 'sC1K', 'sC1N', 'sC1Q', 'sC1R', 'sC2B', 'sC2K', 'sC2N', 'sC2P', 'sC2Q', 'sC2R', 'sC3B', 'sC3K', 'sC3N', 'sC3P', 'sC3Q', 'sC3R', 'sC4B', 'sC4K', 'sC4N', 'sC4P', 'sC4Q', 'sC4R', 'sC5B', 'sC5K', 'sC5N', 'sC5P', 'sC5Q', 'sC5R', 'sC6B', 'sC6K', 'sC6N', 'sC6P', 'sC6Q', 'sC6R', 'sC7B', 'sC7K', 'sC7N', 'sC7P', 'sC7Q', 'sC7R', 'sC8B', 'sC8K', 'sC8N', 'sC8Q', 'sC8R', 'sD1B', 'sD1K', 'sD1N', 'sD1Q', 'sD1R', 'sD2B', 'sD2K', 'sD2N', 'sD2P', 'sD2Q', 'sD2R', 'sD3B', 'sD3K', 'sD3N', 'sD3P', 'sD3Q', 'sD3R', 'sD4B', 'sD4K', 'sD4N', 'sD4P', 'sD4Q', 'sD4R', 'sD5B', 'sD5K', 'sD5N', 'sD5P', 'sD5Q', 'sD5R', 'sD6B', 'sD6K', 'sD6N', 'sD6P', 'sD6Q', 'sD6R', 'sD7B', 'sD7K', 'sD7N', 'sD7P', 'sD7Q', 'sD7R', 'sD8B', 'sD8K', 'sD8N', 'sD8Q', 'sD8R', 'sE1B', 'sE1K', 'sE1N', 'sE1Q', 'sE1R', 'sE2B', 'sE2K', 'sE2N', 'sE2P', 'sE2Q', 'sE2R', 'sE3B', 'sE3K', 'sE3N', 'sE3P', 'sE3Q', 'sE3R', 'sE4B', 'sE4K', 'sE4N', 'sE4P', 'sE4Q', 'sE4R', 'sE5B', 'sE5K', 'sE5N', 'sE5P', 'sE5Q', 'sE5R', 'sE6B', 'sE6K', 'sE6N', 'sE6P', 'sE6Q', 'sE6R', 'sE7B', 'sE7K', 'sE7N', 'sE7P', 'sE7Q', 'sE7R', 'sE8B', 'sE8K', 'sE8N', 'sE8Q', 'sE8R', 'sF1B', 'sF1K', 'sF1N', 'sF1Q', 'sF1R', 'sF2B', 'sF2K', 'sF2N', 'sF2P', 'sF2Q', 'sF2R', 'sF3B', 'sF3K', 'sF3N', 'sF3P', 'sF3Q', 'sF3R', 'sF4B', 'sF4K', 'sF4N', 'sF4P', 'sF4Q', 'sF4R', 'sF5B', 'sF5K', 'sF5N', 'sF5P', 'sF5Q', 'sF5R', 'sF6B', 'sF6K', 'sF6N', 'sF6P', 'sF6Q', 'sF6R', 'sF7B', 'sF7K', 'sF7N', 'sF7P', 'sF7Q', 'sF7R', 'sF8B', 'sF8K', 'sF8N', 'sF8Q', 'sF8R', 'sG1B', 'sG1K', 'sG1N', 'sG1Q', 'sG1R', 'sG2B', 'sG2K', 'sG2N', 'sG2P', 'sG2Q', 'sG2R', 'sG3B', 'sG3K', 'sG3N', 'sG3P', 'sG3Q', 'sG3R', 'sG4B', 'sG4K', 'sG4N', 'sG4P', 'sG4Q', 'sG4R', 'sG5B', 'sG5K', 'sG5N', 'sG5P', 'sG5Q', 'sG5R', 'sG6B', 'sG6K', 'sG6N', 'sG6P', 'sG6Q', 'sG6R', 'sG7B', 'sG7K', 'sG7N', 'sG7P', 'sG7Q', 'sG7R', 'sG8B', 'sG8K', 'sG8N', 'sG8Q', 'sG8R', 'sH1B', 'sH1K', 'sH1N', 'sH1Q', 'sH1R', 'sH2B', 'sH2K', 'sH2N', 'sH2P', 'sH2Q', 'sH2R', 'sH3B', 'sH3K', 'sH3N', 'sH3P', 'sH3Q', 'sH3R', 'sH4B', 'sH4K', 'sH4N', 'sH4P', 'sH4Q', 'sH4R', 'sH5B', 'sH5K', 'sH5N', 'sH5P', 'sH5Q', 'sH5R', 'sH6B', 'sH6K', 'sH6N', 'sH6P', 'sH6Q', 'sH6R', 'sH7B', 'sH7K', 'sH7N', 'sH7P', 'sH7Q', 'sH7R', 'sH8B', 'sH8K', 'sH8N', 'sH8Q', 'sH8R', 'xBK', 'xBN', 'xBP', 'xBQ', 'xBR', 'xBk', 'xBn', 'xBp', 'xBq', 'xBr', 'xKB', 'xKN', 'xKP', 'xKQ', 'xKR', 'xKb', 'xKn', 'xKp', 'xKq', 'xKr', 'xNB', 'xNK', 'xNN', 'xNP', 'xNQ', 'xNR', 'xNb', 'xNk', 'xNn', 'xNp', 'xNq', 'xNr', 'xPB', 'xPK', 'xPN', 'xPP', 'xPQ', 'xPR', 'xPb', 'xPk', 'xPn', 'xPp', 'xPq', 'xPr', 'xQB', 'xQK', 'xQN', 'xQP', 'xQQ', 'xQR', 'xQb', 'xQk', 'xQn', 'xQp', 'xQq', 'xQr', 'xRB', 'xRK', 'xRN', 'xRP', 'xRQ', 'xRR', 'xRb', 'xRk', 'xRn', 'xRp', 'xRq', 'xRr']

cols_map = dict([(v,k) for k,v in enumerate(cols)])

options = tf.python_io.TFRecordOptions(tf.python_io.TFRecordCompressionType.GZIP)
tfwriter = tf.python_io.TFRecordWriter("data/train/features_full.tfr", options=options)
counter = 0

for grp_index, (name, grp) in enumerate(move_gdf):

    board.init()

    for row_index,row in grp.iterrows():
        board.updateMove(row['move'])
        hashkey = board.hashkey()
        if hashkey in move_hashes:
            continue
        else:
            move_hashes.add(hashkey)

        score = np.clip(np.nan_to_num(float(row['score'])),-1024.0,1024.0)
        mate_in = int(row['mate_in'])
        score_log2 = np.sign(score)*np.log2(np.maximum(np.abs(score),1.0))

        # if  np.round(score_log2)==0 and np.random.rand()>0.1:
        #     continue
        features_o = board.eval_features()
        features = np.nan_to_num(np.array([ float(features_o[col]) if col in features_o else 0.0 for col in cols]))
        label = np.nan_to_num(np.array([score,mate_in,score_log2]))

        example = tf.train.Example()
        example.features.feature["features"].float_list.value.extend(features)
        example.features.feature["label"].float_list.value.extend(label)

        tfwriter.write(example.SerializeToString())
        counter+=1


        del features_o
    if grp_index %100==0:
        sys.stdout.write(str(counter//100)+'/'+str(grp_index//100)+" ")
        # sys.stdout.write(str(grp_index//100)+' ')
        sys.stdout.flush()

tfwriter.close()
