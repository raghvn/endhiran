##
#  read_pgn.py
#  Script to read bzipped pgn file from lichess
#  downloaded from https://database.lichess.org/
#  and convert to processed board data.
#  Filters games scored by stockfish engine.
#  Author: Raghav
##

import chess.pgn as pgn
import bz2
import sys
from datetime import datetime
import re
import math
import pandas as pd
import numpy as np

class MyVisitor(pgn.GameModelCreator):

    def __init__(self):
        super().__init__()
        self.comment_regex = re.compile(r"\[\%(\w+) (.*?)\]")
        self.start_time  = datetime.strptime('0:0:0', '%H:%M:%S')
        self.move_stack = []


    def visit_comment(self, comment):
        super().visit_comment(comment)
        eval_score = float('nan')
        clk_seconds = 0
        mate_in = 0

        for match in re.finditer(self.comment_regex,comment):
            if match is not None:
                marg = match.group(1)
                marvgv = match.group(2).strip()
                if marg =='clk':
                    time = datetime.strptime(marvgv, '%H:%M:%S')
                    clk_seconds = (time - self.start_time).total_seconds()
                elif marg =='eval':
                    if marvgv[0]!='#':
                        eval_score = float(eval(marvgv))
                    else:
                        eval_score = sys.float_info.max/float(eval(marvgv[1:]))
                        mate_in = int(eval(marvgv[1:]))

        move = self.variation_stack[-1].move
        self.move_stack.append((move, eval_score, mate_in, clk_seconds))

    def result(self):
        return (self.game, self.move_stack)

def parse_result(rstr):
    if rstr == "1-0":
        return 1
    elif rstr == "0-1":
        return -1
    elif rstr == "1/2-1/2":
        return 0
    else:
        return float('nan')

def parse_headers(game):
    id = game.headers['Site'].split('/')[-1]
    result = parse_result(game.headers['Result'])
    white_elo = int(eval(game.headers["WhiteElo"]))
    black_elo = int(eval(game.headers["BlackElo"]))
    date = datetime.strptime(game.headers["UTCDate"] + " "+ game.headers["UTCTime"], '%Y.%m.%d %H:%M:%S')
    try:
        time_control =  list(map(lambda x: int(eval(x)), game.headers["TimeControl"].split('+')))
    except:
        time_control = [0,0]
    start_time = time_control[0]
    inc_time = time_control[1]

    header_map = {
        'id': id,
        'result' : result,
        'white_elo': white_elo,
        'black_elo': black_elo,
        'date':date,
        'start_time': start_time,
        'inc_time': inc_time
    }

    return header_map


def parse_pgn(filename):

    board_df = pd.DataFrame()
    move_df = pd.DataFrame()

    board_df_cache = []
    move_df_cache = []

    counter = 0
    with bz2.open(filename, "rt") as source_file:
        nan = float('nan')

        try:

            while 1:
                game, move_stack = pgn.read_game(source_file, MyVisitor)

                if game is None:
                    break

                header_map = parse_headers(game)
                board_df_cache.append(pd.DataFrame(header_map,index=['id']))

                counter+=1
                if counter%1000==0:
                    sys.stdout.write(str(counter//1000)+' ')
                    sys.stdout.flush()

                if counter%100000==0:
                    board_df = board_df.append(pd.concat(board_df_cache),ignore_index=True)
                    move_df = move_df.append(pd.concat(move_df_cache))
                    board_df_cache = []
                    move_df_cache = []
                    board_df.to_pickle(filename.replace('.pgn.bz2','_board_ck.pkl'),protocol=2)
                    move_df.to_pickle(filename.replace('.pgn.bz2','_move_ck.pkl'),protocol=2)


                if len(move_stack)>1 and not math.isnan(move_stack[0][1]):
                    mdf = pd.DataFrame([dict({'id': header_map['id'], 'mid':index,'move':str(move), 'score':eval_score, 'mate_in':mate_in, 'clock':clock}) for index, (move,eval_score, mate_in, clock) in enumerate(move_stack)])
                    move_df_cache.append(mdf)

        except Exception as ex:
            print(ex)

    board_df.to_pickle(filename.replace('.pgn.bz2','_board.pkl'),protocol=2)
    move_df.to_pickle(filename.replace('.pgn.bz2','_move.pkl'),protocol=2)


parse_pgn(sys.argv[1])
