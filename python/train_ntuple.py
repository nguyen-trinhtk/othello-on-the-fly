#!/usr/bin/env python3
"""Train n-tuple weights with PyTorch + (10+90)-ES; exports ntuple.bin for C++ engine."""

from __future__ import annotations

import argparse
import random
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from ntuple.es import run_es
from ntuple.model import NTupleNet


def main() -> int:
    p = argparse.ArgumentParser(description="PyTorch n-tuple ES trainer")
    p.add_argument("--out", default="ntuple.bin", help="output weights (1152 bytes)")
    p.add_argument("--seed", type=int, default=None)
    p.add_argument("--target", type=float, default=0.99, help="vs-SWH win rate stop target")
    p.add_argument("--es-games", type=int, default=0, help="phase-1 eval budget (0=unlimited)")
    p.add_argument("--self-games", type=int, default=0, help="phase-2 budget; -1 skips phase 2")
    p.add_argument("--smoke", action="store_true", help="tiny budget for sanity check")
    args = p.parse_args()

    if args.smoke:
        args.es_games = 500
        args.self_games = -1
        args.target = 1.0

    seed = args.seed if args.seed is not None else random.randrange(2**32)
    rng = random.Random(seed)
    print(f"seed={seed}")

    es_budget = args.es_games
    self_budget = args.self_games

    print(f"\n=== Phase 1: ES vs SWH  budget={es_budget or 'unlimited'}  target={args.target} ===\n")
    best = run_es(es_budget, args.target, None, "ES", args.out, rng)

    if self_budget >= 0:
        print(f"\n=== Phase 2: ES vs self  budget={self_budget or 'unlimited'}  target={args.target} ===\n")
        snapshot = NTupleNet(init_uniform=0.0)
        import torch
        with torch.no_grad():
            snapshot.weights.copy_(best.weights)
        best = run_es(self_budget, args.target, snapshot, "SELF", args.out, rng)

    best.save_bin(args.out)
    print(f"\nfinal weights saved → {args.out} ({Path(args.out).stat().st_size} bytes)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
