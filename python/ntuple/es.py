"""(10+90)-ES training loop mirroring C++ train_ntuple."""

from __future__ import annotations

import random
from typing import Callable

from .board import play_game, swh_eval
from .model import NTupleNet

MU = 10
LAMBDA = 90
ES_EVAL = 100
PRINT_EVERY = LAMBDA * ES_EVAL
SWH_QUICK = 20
EPS = 0.10


def _net_fn(net: NTupleNet):
    return lambda board, player: net.evaluate_board(board.black, board.white, player)


def _swh_fn():
    return lambda board, player: swh_eval(board, player)


def win_rate(
    net: NTupleNet,
    opponent: NTupleNet | None,
    games: int,
    rng: random.Random,
) -> float:
    hero = _net_fn(net)
    villain = _net_fn(opponent) if opponent is not None else _swh_fn()
    score = 0.0
    for i in range(games):
        color = 0 if i % 2 == 0 else 1
        result = play_game(hero, villain, color, EPS, rng)
        score += 1.0 if result > 0 else 0.5 if result == 0 else 0.0
    return score / games


def run_es(
    game_budget: int,
    target: float,
    opponent: NTupleNet | None,
    tag: str,
    out_path: str,
    rng: random.Random,
    on_progress: Callable[[str], None] | None = None,
) -> NTupleNet:
    def log(msg: str) -> None:
        if on_progress:
            on_progress(msg)
        else:
            print(msg, flush=True)

    def eval_individual(ind: NTupleNet) -> float:
        return win_rate(ind, opponent, ES_EVAL, rng)

    def vs_swh_quick(ind: NTupleNet) -> float:
        if opponent is None:
            return 0.0
        return win_rate(ind, None, SWH_QUICK, random.Random(42))

    pop = [NTupleNet() for _ in range(MU)]
    fit = [eval_individual(ind) for ind in pop]
    total = MU * ES_EVAL
    next_print = PRINT_EVERY

    def best_idx() -> int:
        return max(range(MU), key=lambda i: fit[i])

    def report() -> bool:
        nonlocal next_print
        if total < next_print:
            return False
        next_print = ((total // PRINT_EVERY) + 1) * PRINT_EVERY
        bi = best_idx()
        bf = fit[bi]
        vs_sw = bf if opponent is None else vs_swh_quick(pop[bi])
        log(f"[{tag}] games={total} fitness={bf:.3f} vs_swh={vs_sw:.3f}")
        pop[bi].save_bin(out_path)
        if vs_sw >= target:
            log(f"[{tag}] target {target} reached")
            return True
        return False

    if report():
        return pop[best_idx()]

    while True:
        if game_budget > 0 and total >= game_budget:
            break

        offspring = [pop[rng.randrange(MU)].mutate() for _ in range(LAMBDA)]
        off_fit: list[float] = []
        for off in offspring:
            f = eval_individual(off)
            off_fit.append(f)
            total += ES_EVAL
            if report():
                pool = [(fit[i], i) for i in range(MU)] + [(off_fit[j], MU + j) for j in range(len(off_fit))]
                best_fit, best_i = max(pool, key=lambda x: x[0])
                if best_i < MU:
                    return pop[best_i]
                return offspring[best_i - MU]
            if game_budget > 0 and total >= game_budget:
                break

        pool = [(fit[i], i) for i in range(MU)] + [(off_fit[i], MU + i) for i in range(LAMBDA)]
        pool.sort(key=lambda x: x[0], reverse=True)
        for i in range(MU):
            idx = pool[i][1]
            pop[i] = pop[idx] if idx < MU else offspring[idx - MU]
            fit[i] = pool[i][0]

    bi = best_idx()
    if game_budget > 0:
        log(f"[{tag}] budget exhausted games={total} best_fitness={fit[bi]:.3f}")
    return pop[bi]
