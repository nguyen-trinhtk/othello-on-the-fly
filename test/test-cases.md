Continue chat: https://claude.ai/chat/c4340562-818b-4dec-a324-b698d2675dd2

# Othello Valid Move Test Cases

This file contains a set of test case descriptions for validating Othello (Reversi) move generation logic. Each test case describes a board state and the expected valid moves for a given player.

## Board Setup Notation

Use an 8x8 grid, rows 1-8 (top to bottom), columns A-H (left to right).
B = Black disc, W = White disc, . = Empty square

Example:
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . W B . . .
5 . . . B W . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

---

## Test Case 1 — Standard Opening Board

**Description:** The default starting position. Black moves first. Only 4 squares are valid — each one flanks exactly one White disc.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . W B . . .
5 . . . B W . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** C4, D3, E6, F5

**Notes:** Any square not in this set should not be returned. Confirms the four-directional flanking logic works from the initial state.

---

## Test Case 2 — No Valid Moves for One Player

**Description:** A board state where Black has no legal moves. The function should return an empty list without crashing.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . W W . . .
5 . . . W W . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** (none — empty list)

**Notes:** The function must handle zero valid moves gracefully. The game should pass the turn to White.

---

## Test Case 3 — Full Board

**Description:** Every square is occupied. Neither player should have any valid moves.

**Board State:**
```
  A B C D E F G H
1 B B B B B B B B
2 B B B B B B B B
3 B B B B B B B B
4 B B B W W B B B
5 B B B W W B B B
6 B B B B B B B B
7 B B B B B B B B
8 B B B B B B B B
```

**Player to Move:** Black (and White)

**Expected Valid Moves:** (none for either player)

**Notes:** Boundary check — no empty squares means no moves possible.

---

## Test Case 4 — Horizontal Flank (Right to Left)

**Description:** A White chain runs horizontally. Black can flank it by playing to the left of the chain.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . B W W W . . .
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** E4 (and any other geometrically valid squares)

**Notes:** Confirms left-to-right horizontal scanning. The anchor is B4, the chain is C4–D4, and E4 completes the flank.

---

## Test Case 5 — Horizontal Flank (Left to Right)

**Description:** Mirror of Test Case 4. Black anchors on the right, move is to the left.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . W W B . .
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** C4 (and any other geometrically valid squares)

**Notes:** Confirms right-to-left horizontal scanning. The anchor is F4, chain is D4–E4, and C4 flanks them.

---

## Test Case 6 — Vertical Flank (Top to Bottom)

**Description:** A White chain runs vertically downward from an anchor. Black plays below the chain.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . B . . . .
3 . . . W . . . .
4 . . . W . . . .
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** D5 (and any other geometrically valid squares)

**Notes:** Confirms top-to-bottom vertical scanning. Anchor is D2, chain is D3–D4, and D5 completes the flank.

---

## Test Case 7 — Vertical Flank (Bottom to Top)

**Description:** Mirror of Test Case 6. Black anchors below, move is above the chain.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . W . . . .
5 . . . W . . . .
6 . . . B . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** D3 (and any other geometrically valid squares)

**Notes:** Confirms bottom-to-top vertical scanning.

---

## Test Case 8 — Diagonal Flank (Top-Left to Bottom-Right)

**Description:** A single White disc sits diagonally between two squares. Black flanks along the top-left to bottom-right diagonal.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . B . . . . .
4 . . . W . . . .
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** E5 (and any other geometrically valid squares)

**Notes:** Confirms diagonal scanning in the ↘ direction.

---

## Test Case 9 — Diagonal Flank (Bottom-Right to Top-Left)

**Description:** Mirror of Test Case 8. Black plays above-left of a White disc to flank it.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . W . . . .
5 . . . . B . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** C3 (and any other geometrically valid squares)

**Notes:** Confirms diagonal scanning in the ↖ direction.

---

## Test Case 10 — Diagonal Flank (Top-Right to Bottom-Left)

**Description:** Black flanks a White disc along the ↙ diagonal.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . B . . .
4 . . . W . . . .
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** C5 (and any other geometrically valid squares)

**Notes:** Confirms diagonal scanning in the ↙ direction.

---

## Test Case 11 — Diagonal Flank (Bottom-Left to Top-Right)

**Description:** Black flanks a White disc along the ↗ diagonal.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . W . . . .
5 . . B . . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** E3 (and any other geometrically valid squares)

**Notes:** Confirms diagonal scanning in the ↗ direction.

---

## Test Case 12 — Multi-Directional Flank

**Description:** A single move flanks opponent discs in multiple directions simultaneously. The square should appear exactly once in the result.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . B . . . .
4 . . W . W . . .
5 . . . B . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** D4 (and any other geometrically valid squares)

**Notes:** D4 flanks horizontally (C4 captured via E4 anchor) and is the intersection of two diagonal anchors. The move should appear only once in the result — no duplicates.

---

## Test Case 13 — Valid Move on the Board Edge

**Description:** A valid move exists along the top edge of the board (row 1).

**Board State:**
```
  A B C D E F G H
1 . . B . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . W B . . .
5 . . . B W . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** White

**Expected Valid Moves:** Includes C1 if it geometrically flanks a Black disc; verify no out-of-bounds error occurs.

**Notes:** Confirms boundary row/column handling. The function must not crash or skip valid edge squares.

---

## Test Case 14 — Corner Move

**Description:** A corner square (A1) is a valid move because it flanks a chain of opponent discs.

**Board State:**
```
  A B C D E F G H
1 . W W B . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . . . . . .
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** Includes A1

**Notes:** Corner squares are the most strategically valuable in Othello. Confirm the function correctly identifies A1 as valid when it flanks horizontally.

---

## Test Case 15 — Occupied Square Not Returned

**Description:** A square that already contains a disc should never appear in the valid moves list, even if it would otherwise satisfy flanking conditions.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . W B . . .
5 . . . B W . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** C4, D3, E6, F5 (D4 and E5 are occupied and must NOT appear)

**Notes:** Basic occupancy check. Occupied squares, regardless of color, are ineligible.

---

## Test Case 16 — Surrounded by Friendly Pieces Only

**Description:** An empty square is adjacent only to friendly discs with no opponent discs in any direction to flank.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . B B . . . .
4 . . B . B . . .
5 . . B B . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** D4 is NOT valid (only surrounded by friendly pieces)

**Notes:** The function must require at least one opponent disc in the flank chain.

---

## Test Case 17 — Long Chain Flank

**Description:** A chain of 6 consecutive White discs lies between a Black anchor and a potential move. The move should still be recognized as valid.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 B W W W W W W .
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** Includes H4

**Notes:** Confirms the scanning loop runs for the full chain length, not just 1–2 discs.

---

## Test Case 18 — Chain Reaches Board Edge Without Anchor

**Description:** A chain of White discs extends all the way to the board edge, but there is no Black anchor on the other side.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . W W W W W
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** C4 is NOT valid (no Black anchor on the right side of the chain)

**Notes:** The function must not treat the board edge itself as an anchor disc.

---

## Test Case 19 — No Row Wrap-Around

**Description:** Ensures horizontal scanning does not wrap from the end of one row (H) to the beginning of the next row (A).

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 B . . . . . W .
4 . . . . . . . B
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** H3 is NOT valid due to wrap-around (W at G3 is not flanked between A3 and H4)

**Notes:** Critical edge case. Row 3 column H and Row 4 column A are not adjacent; the function must not connect them.

---

## Test Case 20 — Different Valid Moves per Player

**Description:** The same board state produces different valid move sets for Black and White.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . W B . . .
5 . . . B W . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Both (test each separately)

**Expected Valid Moves (Black):** C4, D3, E6, F5

**Expected Valid Moves (White):** C5, D6, E3, F4

**Notes:** Confirms the function correctly uses the active player's color to determine flanking, and that the two sets are distinct.

---

## Test Case 21 — Near-Endgame Board

**Description:** The board is nearly full with only a few empty squares. The function should correctly identify which, if any, are valid.

**Board State:**
```
  A B C D E F G H
1 B B B B B B B B
2 B B B B B B B B
3 B B B B B B B B
4 B B B W B B B B
5 B B B B W B B B
6 B B B B B B B B
7 B B B B B B B B
8 B B B B B B . .
```

**Player to Move:** White

**Expected Valid Moves:** Only squares that genuinely flank a Black chain (e.g., G8 or H8 if geometrically valid); all others are occupied or non-flanking.

**Notes:** Stress test for a dense board. The function should not slow down or produce false positives.

---

## Test Case 22 — Single Opponent Disc Flanked

**Description:** The minimum valid flank: exactly one opponent disc between the move and the anchor.

**Board State:**
```
  A B C D E F G H
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . W B . . .
5 . . . . . . . .
6 . . . . . . . .
7 . . . . . . . .
8 . . . . . . . .
```

**Player to Move:** Black

**Expected Valid Moves:** Includes C4 (flanks W at D4 with anchor at E4)

**Notes:** Confirms the minimum flank (chain of 1) is accepted as valid.