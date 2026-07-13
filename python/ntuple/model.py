"""PyTorch n-tuple network: 32 × 9 LUT weights (288 params), matches C++ NTuple::Net."""

from __future__ import annotations

from pathlib import Path

import struct
import torch
import torch.nn as nn

from .groups import ALL2_GROUPS

N_GROUPS = 32
LUT_SIZE = 9


def _cell_val(my_bb: int, opp_bb: int, pos: int) -> int:
    bit = 1 << pos
    if my_bb & bit:
        return 2
    if opp_bb & bit:
        return 0
    return 1


def _lut_idx(va: int, vb: int) -> int:
    return va + 3 * vb


def encode_indices(my_bb: int, opp_bb: int) -> list[tuple[int, int]]:
    """Return (group_id, lut_index) for every active symmetry lookup."""
    out: list[tuple[int, int]] = []
    for g, group in enumerate(ALL2_GROUPS):
        for sym in group.syms:
            va = _cell_val(my_bb, opp_bb, sym.a)
            vb = _cell_val(my_bb, opp_bb, sym.b)
            out.append((g, _lut_idx(va, vb)))
    return out


class NTupleNet(nn.Module):
    """Linear lookup-sum evaluator; weights shape (32, 9)."""

    def __init__(self, init_uniform: float = 0.1) -> None:
        super().__init__()
        w = torch.empty(N_GROUPS, LUT_SIZE)
        nn.init.uniform_(w, -init_uniform, init_uniform)
        self.weights = nn.Parameter(w)

    def forward_from_bbs(self, my_bb: int, opp_bb: int) -> torch.Tensor:
        total = self.weights.new_zeros(())
        for g, j in encode_indices(my_bb, opp_bb):
            total = total + self.weights[g, j]
        return total

    def evaluate_board(self, black: int, white: int, player: int) -> float:
        """Board inversion: current player treated as 'my' (black-side) discs."""
        my_bb = black if player == 0 else white
        opp_bb = white if player == 0 else black
        with torch.no_grad():
            return float(self.forward_from_bbs(my_bb, opp_bb).item())

    def mutate(self, sigma: float = 1.0) -> NTupleNet:
        child = NTupleNet(init_uniform=0.0)
        with torch.no_grad():
            child.weights.copy_(self.weights + torch.randn_like(self.weights) * sigma)
        return child

    def save_bin(self, path: str | Path) -> None:
        flat = self.weights.detach().cpu().flatten().tolist()
        Path(path).write_bytes(struct.pack(f"{len(flat)}f", *flat))

    @classmethod
    def load_bin(cls, path: str | Path) -> NTupleNet:
        raw = Path(path).read_bytes()
        n = len(raw) // 4
        if n != N_GROUPS * LUT_SIZE:
            raise ValueError(f"expected {N_GROUPS * LUT_SIZE} floats, got {n}")
        vals = struct.unpack(f"{n}f", raw)
        net = cls(init_uniform=0.0)
        with torch.no_grad():
            net.weights.copy_(torch.tensor(vals, dtype=torch.float32).reshape(N_GROUPS, LUT_SIZE))
        return net
