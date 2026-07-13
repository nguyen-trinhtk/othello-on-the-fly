"""32 canonical all-2 tuple groups with D4 symmetry expansion (matches C++ ntuple.cpp)."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Tuple2:
    a: int
    b: int


@dataclass
class TupleGroup:
    canonical: Tuple2
    syms: list[Tuple2]


def _d4(s: int, r: int, c: int) -> tuple[int, int]:
    if s == 0:
        return r, c
    if s == 1:
        return c, 7 - r
    if s == 2:
        return 7 - r, 7 - c
    if s == 3:
        return 7 - c, r
    if s == 4:
        return 7 - r, c
    if s == 5:
        return r, 7 - c
    if s == 6:
        return c, r
    return 7 - c, 7 - r


def _norm(a: int, b: int) -> Tuple2:
    return Tuple2(a, b) if a < b else Tuple2(b, a)


def _canonical(a: int, b: int) -> Tuple2:
    ra, ca = divmod(a, 8)
    rb, cb = divmod(b, 8)
    best = _norm(a, b)
    for s in range(1, 8):
        r1, c1 = _d4(s, ra, ca)
        r2, c2 = _d4(s, rb, cb)
        q = _norm(r1 * 8 + c1, r2 * 8 + c2)
        if (q.a, q.b) < (best.a, best.b):
            best = q
    return best


def build_groups() -> list[TupleGroup]:
    gmap: dict[tuple[int, int], set[tuple[int, int]]] = {}

    def insert(r1: int, c1: int, r2: int, c2: int) -> None:
        p = _norm(r1 * 8 + c1, r2 * 8 + c2)
        can = _canonical(p.a, p.b)
        key = (can.a, can.b)
        ra, ca = divmod(p.a, 8)
        rb, cb = divmod(p.b, 8)
        bucket = gmap.setdefault(key, set())
        for s in range(8):
            r1s, c1s = _d4(s, ra, ca)
            r2s, c2s = _d4(s, rb, cb)
            q = _norm(r1s * 8 + c1s, r2s * 8 + c2s)
            bucket.add((q.a, q.b))

    for r in range(8):
        for c in range(8):
            if c + 1 < 8:
                insert(r, c, r, c + 1)
            if r + 1 < 8:
                insert(r, c, r + 1, c)
            if r + 1 < 8 and c + 1 < 8:
                insert(r, c, r + 1, c + 1)
            if r + 1 < 8 and c - 1 >= 0:
                insert(r, c, r + 1, c - 1)

    groups: list[TupleGroup] = []
    for key in sorted(gmap):
        syms = [Tuple2(a, b) for a, b in sorted(gmap[key])]
        groups.append(TupleGroup(canonical=Tuple2(key[0], key[1]), syms=syms))

    if len(groups) != 32:
        raise RuntimeError(f"expected 32 groups, got {len(groups)}")
    return groups


ALL2_GROUPS: list[TupleGroup] = build_groups()
