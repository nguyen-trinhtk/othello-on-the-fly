"""Minimal Othello bitboard engine for n-tuple self-play (matches C++ rules)."""

from __future__ import annotations

import random
from dataclasses import dataclass

NOT_FILE_A = 0xFEFEFEFEFEFEFEFE
NOT_FILE_H = 0x7F7F7F7F7F7F7F7F
MAX_CAPTURE_CHAIN = 5

BLACK = 0
WHITE = 1

SHIFTS = [
    lambda b: b >> 8,
    lambda b: b << 8,
    lambda b: (b << 1) & NOT_FILE_A,
    lambda b: (b >> 1) & NOT_FILE_H,
    lambda b: (b >> 7) & NOT_FILE_A,
    lambda b: (b >> 9) & NOT_FILE_H,
    lambda b: (b << 9) & NOT_FILE_A,
    lambda b: (b << 7) & NOT_FILE_H,
]

SWH_WEIGHTS = [
    [1.00, -0.25, 0.10, 0.05, 0.05, 0.10, -0.25, 1.00],
    [-0.25, -0.25, 0.01, 0.01, 0.01, 0.01, -0.25, -0.25],
    [0.10, 0.01, 0.05, 0.02, 0.02, 0.05, 0.01, 0.10],
    [0.05, 0.01, 0.02, 0.01, 0.01, 0.02, 0.01, 0.05],
    [0.05, 0.01, 0.02, 0.01, 0.01, 0.02, 0.01, 0.05],
    [0.10, 0.01, 0.05, 0.02, 0.02, 0.05, 0.01, 0.10],
    [-0.25, -0.25, 0.01, 0.01, 0.01, 0.01, -0.25, -0.25],
    [1.00, -0.25, 0.10, 0.05, 0.05, 0.10, -0.25, 1.00],
]


def bit_at(row: int, col: int) -> int:
    return 1 << ((row << 3) | col)


def opponent(player: int) -> int:
    return WHITE if player == BLACK else BLACK


@dataclass
class Board:
    black: int = 0
    white: int = 0

    @staticmethod
    def standard_start() -> Board:
        return Board(
            black=bit_at(3, 4) | bit_at(4, 3),
            white=bit_at(3, 3) | bit_at(4, 4),
        )

    def discs(self, player: int) -> int:
        return self.black if player == BLACK else self.white

    def occupied(self) -> int:
        return self.black | self.white

    def apply(self, player: int, move_bit: int, flip_mask: int) -> None:
        if player == BLACK:
            self.black |= move_bit | flip_mask
            self.white &= ~flip_mask
        else:
            self.white |= move_bit | flip_mask
            self.black &= ~flip_mask

    def clone(self) -> Board:
        return Board(self.black, self.white)


def _bitboards(board: Board, player: int) -> tuple[int, int]:
    return board.discs(player), board.discs(opponent(player))


def flip_mask(board: Board, player: int, move_bit: int) -> int:
    player_bb, opp_bb = _bitboards(board, player)
    flips = 0
    for shift in SHIFTS:
        ray = shift(move_bit)
        directional = 0
        while ray & opp_bb:
            directional |= ray
            ray = shift(ray)
        if ray & player_bb:
            flips |= directional
    return flips


def valid_moves_mask(board: Board, player: int) -> int:
    player_bb, opp_bb = _bitboards(board, player)
    empty_bits = ~(player_bb | opp_bb) & 0xFFFFFFFFFFFFFFFF
    moves = 0
    for shift in SHIFTS:
        candidates = shift(player_bb) & opp_bb
        for _ in range(MAX_CAPTURE_CHAIN):
            candidates |= shift(candidates) & opp_bb
        moves |= shift(candidates) & empty_bits
    return moves


def iter_moves(mask: int) -> list[tuple[int, int]]:
    out: list[tuple[int, int]] = []
    while mask:
        idx = (mask & -mask).bit_length() - 1
        out.append((idx >> 3, idx & 7))
        mask &= mask - 1
    return out


def has_valid_move(board: Board, player: int) -> bool:
    return valid_moves_mask(board, player) != 0


def apply_move(board: Board, player: int, row: int, col: int) -> bool:
    move_bit = bit_at(row, col)
    flips = flip_mask(board, player, move_bit)
    if flips == 0:
        return False
    board.apply(player, move_bit, flips)
    return True


def winner(board: Board) -> int | None:
    bc = board.black.bit_count()
    wc = board.white.bit_count()
    if bc > wc:
        return BLACK
    if wc > bc:
        return WHITE
    return None


def swh_eval(board: Board, player: int) -> float:
    raw = 0.0
    occ = board.occupied()
    for r in range(8):
        for c in range(8):
            bit = bit_at(r, c)
            if not (occ & bit):
                continue
            w = SWH_WEIGHTS[r][c]
            raw += w if board.black & bit else -w
    return raw if player == BLACK else -raw


def pick_move(board: Board, player: int, eval_fn, eps: float, rng: random.Random) -> tuple[int, int] | None:
    mask = valid_moves_mask(board, player)
    moves = iter_moves(mask)
    if not moves:
        return None
    if eps > 0.0 and rng.random() < eps:
        return rng.choice(moves)
    best = moves[0]
    best_val = float("-inf")
    for row, col in moves:
        nxt = board.clone()
        apply_move(nxt, player, row, col)
        val = eval_fn(nxt, player)
        if val > best_val:
            best_val = val
            best = (row, col)
    return best


def play_game(hero_eval, villain_eval, hero_color: int, eps: float, rng: random.Random) -> int:
    board = Board.standard_start()
    player = BLACK
    while True:
        if not has_valid_move(board, player):
            if not has_valid_move(board, opponent(player)):
                break
            player = opponent(player)
            continue
        eval_fn = hero_eval if player == hero_color else villain_eval
        move = pick_move(board, player, eval_fn, eps, rng)
        if move is None:
            break
        apply_move(board, player, move[0], move[1])
        player = opponent(player)
    w = winner(board)
    if w is None:
        return 0
    return 1 if w == hero_color else -1
