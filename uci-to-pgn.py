#!/usr/bin/env python

import Chessnut
import chess
import chess.pgn

import sys

def long_sequence_to_pgn(seq):
    moves = seq
    game = chess.pgn.Game()
#     with open(filename, 'r') as f:
#         moves = f.read().split('\n');
    node = game
    for move in moves:
        node = node.add_variation(chess.Move.from_uci(move))
    return str(game)

uci_long = sys.stdin.read().strip().split('\n')
print(uci_long)
print(long_sequence_to_pgn(uci_long))
