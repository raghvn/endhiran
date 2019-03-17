##
#  feature_verify.py
#  Script to verify features created by engine 
#  Author: Raghav
##

import pandas as pd
import numpy as np
import pyendhiran
import chess
import sys
import collections

board_df = pd.read_pickle("data/games/lichess_db_standard_rated_2018-07_board_ck.pkl")
move_df = pd.read_pickle("data/games/lichess_db_standard_rated_2018-07_move_ck.pkl")

move_gdf = move_df.groupby('id')
move_df_cols = move_df.columns


def validate_fattacks(eboard, pboard, features, movestr):
    f = collections.defaultdict(int)
    for from_square in chess.SQUARES:
        to_squares = pboard.attacks(from_square)
        for to_square in to_squares:
            p = pboard.piece_at(from_square)
            p2 = pboard.piece_at(to_square)

            if p is not None and p2 is not None:
                ptchar = str(p.symbol().upper())
                ptchar2 = str(p2.symbol().upper()) if p.color==p2.color else str(p2.symbol().lower())
                pkey = 'x'+ptchar+ptchar2
                x[pkey] += int(p.color)*(2) - 1

    cnm = collections.OrderedDict(sorted(filter(lambda t:t[1]!=0, x.items())))
    endnm = collections.OrderedDict(sorted(filter(lambda t: t[0].startswith('x'),features.items())))

    if cnm != endnm:
        print([(key,cnm[key],endnm[key]) for key in cnm if key in endnm and endnm[key]!=cnm[key]])
        print(pboard)
        print(pboard.fen())
        print()

def validate_attacks(eboard, pboard, features, movestr):
    x = collections.defaultdict(int)
    for from_square in chess.SQUARES:
        to_squares = pboard.attacks(from_square)
        for to_square in to_squares:
            p = pboard.piece_at(from_square)
            p2 = pboard.piece_at(to_square)

            if p is not None and p2 is not None:
                ptchar = str(p.symbol().upper())
                ptchar2 = str(p2.symbol().upper()) if p.color==p2.color else str(p2.symbol().lower())
                pkey = 'x'+ptchar+ptchar2
                x[pkey] += int(p.color)*(2) - 1

    cnm = collections.OrderedDict(sorted(filter(lambda t:t[1]!=0, x.items())))
    endnm = collections.OrderedDict(sorted(filter(lambda t: t[0].startswith('x'),features.items())))

    if cnm != endnm:
        print([(key,cnm[key],endnm[key]) for key in cnm if key in endnm and endnm[key]!=cnm[key]])
        print(pboard)
        print(pboard.fen())
        print()


def validate_moves(eboard, pboard, features, movestr):

    nm = collections.defaultdict(int)
    for move in pboard.pseudo_legal_moves:
        p = pboard.piece_at(move.from_square)

        if p is not None:
            pkey = 'nm'+str(p.symbol().upper())
            nm[pkey] += int(p.color)*(2) - 1


    pboardc = pboard.copy()
    pboardc.turn = not pboardc.turn
    for move in pboardc.pseudo_legal_moves:
        p = pboardc.piece_at(move.from_square)

        if p is not None:
            pkey = 'nm'+str(p.symbol().upper())
            nm[pkey] += int(p.color)*(2) - 1

    for k,v in nm.items():

        if (not pboard.is_check()) and v!=0 and k not in features and k != 'nmK': #and k != 'nmP'

            cnm = collections.OrderedDict(sorted(filter(lambda t:t[1]!=0, nm.items())))
            endnm = collections.OrderedDict(sorted(filter(lambda t: t[0].startswith('nm'),features.items())))

            print(cnm)
            print(endnm)
            print([(key,cnm[key],endnm[key]) for key in cnm if key in endnm and endnm[key]!=cnm[key]])
            print(pboard)
            print(pboard.fen())

def validate_values(eboard, pboard, features, movestr):

    nv = collections.defaultdict(int)
    for square in chess.SQUARES:
        p = pboard.piece_at(square)
        if p is not None:
            pkey = 'nv'+str(p.symbol().upper())
            nv[pkey] += int(p.color)*(2) - 1


    for k,v in nv.items():
        if v!=0 and features[k]!= v:
            print(features[k], v)

def validate_squares(eboard, pboard, features, movestr):
    sqll = set(map(lambda y: (y[1:3]).lower(), filter(lambda x: x[0]=='s', features.keys())))
    for square in chess.SQUARES:
        p = pboard.piece_at(square)
        if p is not None:
            sqstr = "s"+str(chess.SQUARE_NAMES[square]).upper()+str(p)
            if sqstr not in features:
                print(sqstr)
                print(" ".join(features.keys()))
                print(eboard)
                print(pboard)
                print(movestr)
                pboard.pop()
                print(pboard.fen())

                sys.exit()
        else:
            print(sqll, chess.SQUARE_NAMES[square])
            if chess.SQUARE_NAMES[square] in sqll:
                print(sqll)




def validate(eboard, pboard, features, movestr):
    validate_attacks(eboard, pboard, features, movestr)

features_list = []
eboard = pyendhiran.Board()
pboard = chess.Board()
for grp_index, (name, grp) in enumerate(move_gdf):

    pboard.reset()
    eboard.init()

    print(grp_index, name)

    for row_index,row in grp.iterrows():


        movestr = row['move']
        pmove = chess.Move.from_uci(movestr)
        pboard.push(pmove)
        moved = eboard.updateMove(movestr)

        features_o = eboard.eval_features()
        features = dict([(t.key(),t.data()) for t in features_o if int(t.data())!=0])
        validate(eboard, pboard, features, movestr)
        del features_o