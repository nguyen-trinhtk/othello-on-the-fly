# Alpha-Beta Pruning Test Cases

This file contains test case descriptions for validating the `alphabeta` function in the Othello engine.

## Test Case 1 — Standard Opening Board
**Board State:**
```
	0 1 2 3 4 5 6 7
0 . . . . . . . .
1 . . . . . . . .
2 . . . . . . . .
3 . . . W B . . .
4 . . . B W . . .
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
```
Current player: Black
Search depth: 1
Expected evaluation: Best move for Black, evaluation > 0 (Black advantage)
Expected outcome: Black plays at (2,3), (3,2), (4,5), or (5,4); evaluation = +2 (Black has 4 discs, White has 2 after move; +2 disc advantage).
Notes: Standard opening, should select one of four valid moves.

## Test Case 2 — No Legal Moves for Maximizing Player
**Board State:**
```
	0 1 2 3 4 5 6 7
0 W W W W W W W W
1 W W W W W W W W
2 W W W W W W W W
3 W W W W W W W W
4 W W W W W W W W
5 W W W W W W W W
6 W W W W W W W W
7 . . . . . . . .
```
Current player: Black
Search depth: 2
Expected evaluation: White's best move, evaluation < 0 (Black cannot move)
Expected outcome: Black cannot move, White plays next; evaluation = -56 (White has 56 discs, Black has 0; -56 disc advantage).
Notes: Black has no legal moves, White will play next.

## Test Case 3 — Terminal State
**Board State:**
```
	0 1 2 3 4 5 6 7
0 B B B B B B B B
1 B B B B B B B B
2 B B B B B B B B
3 B B B B B B B B
4 B B B B B B B B
5 B B B B B B B B
6 B B B B B B B B
7 B B B B B B B B
```
Current player: Black
Search depth: 3
Expected evaluation: Maximum value (Black wins)
Expected outcome: Black wins, evaluation = +64 (Black has all 64 discs, White has 0).
Notes: Terminal state, no moves left, full board.

## Test Case 4 — Alpha Cutoff
**Board State:**
```
	0 1 2 3 4 5 6 7
0 B B B B B B B B
1 B B B B B B B B
2 B B B B B B B B
3 B B B B B B B W
4 W W W W W W W W
5 W W W W W W W W
6 W W W W W W W W
7 W W W W W W W W
```
Current player: Black
Search depth: 3
Expected evaluation: Pruned branches, correct value for Black
Expected outcome: Alpha cutoff occurs, search prunes unnecessary branches; evaluation = +8 (Black has 36 discs, White has 28; +8 disc advantage).
Notes: Alpha cutoff should occur as Black's best move is found early.

## Test Case 5 — Beta Cutoff
**Board State:**
```
	0 1 2 3 4 5 6 7
0 W W W W W W W W
1 W W W W W W W W
2 W W W W W W W W
3 W W W W W W W B
4 B B B B B B B B
5 B B B B B B B B
6 B B B B B B B B
7 B B B B B B B B
```
Current player: White
Search depth: 3
Expected evaluation: Pruned branches, correct value for White
Expected outcome: Beta cutoff occurs, search prunes unnecessary branches; evaluation = -8 (White has 36 discs, Black has 28; -8 disc advantage).
Notes: Beta cutoff should occur as White's best move is found early.

## Test Case 6 — Maximizing vs Minimizing
**Board State:**
```
	0 1 2 3 4 5 6 7
0 . . . . . . . .
1 . . . . . . . .
2 . . . . . . . .
3 . . . W B . . .
4 . . . B W . . .
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
```
Current player: Black
Search depth: 2
Expected evaluation: Optimal path for both players, value reflects best sequence
Expected outcome: Black chooses best move, White responds optimally; evaluation = +1 (Black has 5 discs, White has 4 after sequence; +1 disc advantage).
Notes: Both maximizing and minimizing moves available, tests alternation.

## Test Case 7 — Deep Search
**Board State:**
```
	0 1 2 3 4 5 6 7
0 . . . . . . . .
1 . . . . . . . .
2 . . . B W . . .
3 . . B W B . . .
4 . . W B W . . .
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
```
Current player: Black
Search depth: 5
Expected evaluation: Best sequence for Black, value > 0
Expected outcome: Black finds best sequence over 5 moves; evaluation = +3 (Black has 20 discs, White has 17 after deep search; +3 disc advantage).
Notes: Deep search, mid-game complexity, tests performance and correctness.

---
For each test case, specify:
- Board state (as matrix or notation)
- Current player
- Search depth
- Expected evaluation or move sequence
- Any relevant notes (e.g., cutoffs, edge cases)
