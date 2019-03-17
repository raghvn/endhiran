##
#  feature_gen.py
#  Script to generate features from processed 
#  game data
#  Author: Raghav
##

import pandas as pd
import numpy as np
import pyendhiran
import sys
import copy
import json

board_df = pd.read_pickle("data/test/board.pkl")
move_df = pd.read_pickle("data/test/move.pkl")

move_gdf = move_df.groupby('id')
move_df_cols = move_df.columns

features_df = pd.DataFrame()

board = pyendhiran.Board()
features_list = []
move_hashes = set()
duplicate_counter = 0

for grp_index, (name, grp) in enumerate(move_gdf):

    board.init()
    #print(grp_index, name)

    for row_index,row in grp.iterrows():
        board.updateMove(row['move'])
        hashkey = board.hashkey()
        if hashkey in move_hashes:
            duplicate_counter+=1
            continue
        else:
            move_hashes.add(hashkey)
        features_o = board.eval_features()
        features = dict([(t.key(),t.data()) for t in features_o if int(t.data())!=0])
        features['id'] = row['id']
        features['mid'] = row['mid']
        features_list.append(features)

        del features_o

    if grp_index %100==0:
        sys.stdout.write(str(grp_index//100)+' ')
        sys.stdout.flush()

    if grp_index %10000==0:
        features_local_df = pd.DataFrame(features_list)
        features_df = features_df.append(features_local_df, ignore_index=True, sort=False)
        features_list = []
        print()

features_df = features_df.append(pd.DataFrame(features_list), ignore_index=True, sort=False)
features_df.set_index(['id','mid'],inplace=True)
features_df.fillna(0.0,inplace=True)
features_df.to_pickle("data/test/features.pkl", compression='gzip',protocol=4)
