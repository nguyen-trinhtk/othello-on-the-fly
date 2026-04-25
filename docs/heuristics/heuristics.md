# Heuristic Evaluation Roadmap

Goal: define a first real Othello evaluation function for this repo that is:

- grounded in Othello-engine literature
- simple enough to implement and debug
- structured so weights can be tuned later

## Research Summary

The literature points in a consistent direction:

- strong Othello programs use feature-based evaluation, not raw disc count alone
- feature importance changes by game stage
- top engines eventually learn or tune weights automatically instead of trusting one hand-tuned vector forever

Three especially useful references for this repo:

1. Paul Rosenbloom, *A World-Championship-Level Othello Program* (1982)  
   DOI: `10.1016/0004-3702(82)90003-0`

2. Kai-Fu Lee and Sanjoy Mahajan, *The Development of a World Class Othello Program* (1990)  
   DOI: `10.1016/0004-3702(90)90068-B`

3. Michael Buro, *The Evolution of Strong Othello Programs* (2002)  
   DOI: `10.1007/978-0-387-35660-0_10`

Useful newer implementation-oriented references:

4. Jorge Hernandez, Karen Daza, Hector Florez, *Alpha-Beta vs Scout Algorithms for the Othello Game* (2019)  
   PDF: `https://ceur-ws.org/Vol-2486/icaiw_wdea_3.pdf`

5. Prakhar Gangwar et al., *Hardware/Software Co-Design of a High-Speed Othello Solver*  
   PDF: `https://pgangwar-ucsd.github.io/publications/Othello.pdf`

Important interpretation:

- The papers strongly support using mobility, corner-related structure, stability, and stage-dependent evaluation.
- They do **not** give one universally accepted fixed weight table for `mobility + corners + frontier + parity + stability`.
- So for this repo, the exact formulas below are a **starter engineering design**, informed by the literature, not claimed as the one canonical research formula.

## Repo Starter Formula

Use a weighted linear combination:

```text
eval(player) =
    w_mobility        * actual_mobility_score
  + w_potential       * potential_mobility_score
  + w_corners         * corner_score
  + w_frontier        * frontier_score
  + w_parity          * parity_score
  + w_stability       * stability_score
```

All feature terms should be from the perspective of `player`, where positive means better for `player`.

### Normalization Helper

Use one bounded helper for count-style features:

```text
norm(a, b) =
    0                              if a + b == 0
    100 * (a - b) / (a + b)        otherwise
```

This keeps features on roughly the same scale and makes later weight tuning easier.

## Exact Starter Feature Formulas

### 1. Actual Mobility

Definition:

- `my_moves  = legal move count for player`
- `opp_moves = legal move count for opponent`

Formula:

```text
actual_mobility_score = norm(my_moves, opp_moves)
```

Why:

- Othello literature consistently treats mobility as a central early/midgame feature.

### 2. Potential Mobility

Definition:

- `my_potential  = number of empty squares adjacent to at least one opponent disc`
- `opp_potential = number of empty squares adjacent to at least one player disc`

Bitboard-friendly version:

```text
my_potential  = popcount(empty_squares adjacent_to opponent_discs)
opp_potential = popcount(empty_squares adjacent_to player_discs)
```

Formula:

```text
potential_mobility_score = norm(my_potential, opp_potential)
```

Why:

- This is the standard cheap approximation of future move availability.
- It is simpler than lookahead but still more informative than actual mobility alone.

### 3. Corners

Definition:

- corners are `(0,0)`, `(0,7)`, `(7,0)`, `(7,7)`
- `my_corners  = number of corners occupied by player`
- `opp_corners = number of corners occupied by opponent`

Formula:

```text
corner_score = norm(my_corners, opp_corners)
```

Why:

- Corner occupancy is one of the most stable and strategically important Othello features.

### 4. Frontier

Definition:

- a frontier disc is a disc adjacent to at least one empty square
- `my_frontier  = number of player discs adjacent to any empty square`
- `opp_frontier = number of opponent discs adjacent to any empty square`

Formula:

```text
frontier_score = norm(opp_frontier, my_frontier)
```

Why:

- Fewer own frontier discs is usually better, so the formula is intentionally reversed.
- Frontier is included here as a common handcrafted Othello feature and a practical extension to the primary-source feature set above.

### 5. Parity

Definition:

- `my_discs  = number of player discs`
- `opp_discs = number of opponent discs`

Formula:

```text
parity_score = norm(my_discs, opp_discs)
```

Why:

- Disc differential matters much more late than early.
- Early over-weighting of parity is a classic Othello mistake.

### 6. Stability-ish

For the first version, do **not** try to solve exact full-board stability.

Use a stable-like approximation:

- all occupied corners count as stable
- edge discs connected contiguously to an owned corner along that edge count as stable-like
- count each stable-like disc once

Definition:

- `my_stable_like  = count of stable-like discs for player`
- `opp_stable_like = count of stable-like discs for opponent`

Formula:

```text
stability_score = norm(my_stable_like, opp_stable_like)
```

Why:

- This captures the main corner-anchored stability idea without the full complexity of exact global stability detection.

## Abstract Phase Weights

The literature supports stage-dependent weighting more than one fixed weight vector.

For this repo, start with three abstract stages:

- `early`: many empties remain
- `mid`: board is contested and mobility/stability tradeoffs matter most
- `end`: few empties remain, parity and exact outcomes matter more

Suggested first-pass priorities:

| Feature | Early | Mid | End |
|---|---|---|---|
| Actual mobility | High | High | Low |
| Potential mobility | Medium | Medium | Low |
| Corners | Medium | High | Medium |
| Frontier | Medium | Medium | Low |
| Parity | Low | Low-to-Medium | High |
| Stability-ish | Low | Medium-to-High | High |

Interpretation:

- Early: prioritize move freedom and avoiding structurally bad shapes.
- Mid: prioritize mobility, corners, and growing stable structure.
- End: let parity dominate more, with stability still important.

## Stage Boundaries

Use simple empty-square boundaries first:

```text
early: empty_squares >= 44
mid:   10 <= empty_squares < 44
end:   empty_squares < 10
```

These are implementation choices, not sacred thresholds.
They are mainly meant to give a clean first version and can be tuned later.

## First Implementation Advice

Implement in this order:

1. actual mobility
2. corners
3. parity with low early weight
4. frontier
5. potential mobility
6. stability-ish

Reason:

- mobility + corners + parity already gives a meaningful first evaluation
- frontier and potential mobility improve positional understanding
- stability-ish is valuable, but easier to get wrong, so it should come after the simpler terms are in place

## What To Tune Later

After the first version works:

- convert abstract weights into numeric weights
- self-play or benchmark-tune the numeric weights
- add corner-danger terms (`X` and `C` squares)
- consider pattern-based evaluation
- eventually replace hand tuning with data-driven tuning

## High-Level Takeaway

- Use phase-aware, normalized, feature-difference scoring.
- Do not trust parity early.
- Treat the formulas above as the repo's exact starter design.
- Treat the weights as stage-dependent priorities first, numeric tuning second.
