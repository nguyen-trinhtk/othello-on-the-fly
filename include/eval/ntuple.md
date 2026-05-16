```
SETUP:
  Generate all-2 tuple network:
    - All 32 unique adjacent pairs (horizontal, vertical, diagonal) on the 8×8 board (before symmetry expansion)
        initially has 210 pairs: 
            56 vert, 56 hor, 49 diag, 49 skew diag
        symmetry remove
            rotational (0, 90, 180, 270)
            flip (axis sym) x, y
        remaining 32
    - Each tuple has 3² = 9 LUT entries → 32 × 9 = 288 weights total
    - Each tuple used up to 8 times via symmetric sampling // table looks same if rotate, flip, etc

ENCODE(board, tuple {pos_a, pos_b}):
  // Cell values: 0=white, 1=empty, 2=black
  // base 3
  index = board[pos_a] * 3⁰ + board[pos_b] * 3¹
  return index   // ∈ [0, 8]

EVALUATE(board):
  value = 0
  for each tuple i (up to 32):
    for each of 8 symmetric rotations/reflections of tuple i:
      value += LUT_i[ ENCODE(board_rotated, tuple) ]
  return value

BEST_MOVE(board, player):
  // 1-ply greedy search (paper uses 1-ply only)
  best_val = -∞
  for each legal move m:
    next = apply(board, m)
    val  = EVALUATE(next)        // always from current player's POV
    if val > best_val: best = m
  return best

BOARD_INVERSION(board, player):
  // Paper shows board inversion >> output negation
  if player == WHITE:
    flip all pieces on board   // pretend we're always black
  evaluate as black
  flip back and play the selected move

LEARN (Evolution Strategy, not TD):
  // NOTE: paper uses (10+90)-ES, NOT TD learning
  population = 10 individuals, each = a full set of 288 weights
  initialize weights ~ Uniform(-0.1, 0.1)

  for 5000 generations:
    mutate 90 offspring via Gaussian mutation (σ = 1.0)
    evaluate each individual:
      play 1000 double games vs SWH (Standard WPC Heuristic)
      fitness = win rate in ε-Othello (ε = 0.1 random moves)
    select top 10 survivors (elitist)

OUTPUT:
  best individual's 288 weights = your n-tuple network
```