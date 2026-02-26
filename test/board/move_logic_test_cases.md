# Move Logic & Edge Cases Test Checklist

## Move Logic
- [ ] is_valid_move(r, c, player): detects valid/invalid moves for both players
	- Valid move for BLACK
	- Valid move for WHITE
	- Invalid move (occupied, out-of-bounds, no flips)
- [ ] compute_valid_moves(): computes all valid moves for current player
	- Initial board (should be 4 moves)
	- After a move
	- No valid moves (pass)
- [ ] get_valid_moves(): returns correct set of valid moves
	- Matches compute_valid_moves()
	- After board changes
- [ ] process_move(r, c, player): applies move, flips discs, updates board
	- Valid move: correct discs flipped
	- Invalid move: no change, error returned
	- Edge/corner moves
	- Multi-direction flips
- [ ] parse_move(): parses user input (if testable)
	- Valid input
	- Invalid input (non-numeric, out-of-bounds)

## Edge Cases
- [ ] Moves on edges/corners
	- All edge/corner positions
- [ ] No valid moves for one/both players
	- Pass turn
- [ ] Full board
	- 64 discs, no moves
- [ ] Invalid moves (out of bounds, occupied, etc)
	- Negative indices, indices >= 8
	- Occupied squares
