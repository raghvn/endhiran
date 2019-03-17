##
#  feature_gen.py
#  Script to generate features from processed 
#  game data and save it in json format.
#  Author: Raghav
##


import pandas as pd
import numpy as np
import pyendhiran
import sys
import copy
import json

board_df = pd.read_pickle("data/train/board.pkl")
move_df = pd.read_pickle("data/train/move.pkl")

move_gdf = move_df.groupby('id')
move_df_cols = move_df.columns

features_df = pd.DataFrame()#index=['id','mid'])

board = pyendhiran.Board()
features_list = []
move_hashes = set()
duplicate_counter = 0

jsonf = open("data/train/features.json",'w')

for grp_index, (name, grp) in enumerate(move_gdf):

    board.init()

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
        jsonf.write(json.dumps(features)+"\n")

        del features_o
    #print(grp_index)
    if grp_index %100==0:
        sys.stdout.write(str(grp_index//100)+' ')
        sys.stdout.flush()

jsonf.close()
