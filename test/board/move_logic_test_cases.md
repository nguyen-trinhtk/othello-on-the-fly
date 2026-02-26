# Move Logic & Edge Cases Test Checklist


## Move Logic
- [ ] is_valid_move(r, c, player): detects valid/invalid moves for both players
	<!--
	Test Case 1: Valid move for BLACK
		- Setup: Initial board, call is_valid_move on one of the 4 valid starting moves for BLACK.
		- Check: Returns true.
	Test Case 2: Valid move for WHITE
		- Setup: Make a valid BLACK move, then call is_valid_move for WHITE on a valid response.
		- Check: Returns true.
	Test Case 3: Occupied square
		- Setup: Try is_valid_move on a square already occupied by a disc.
		- Check: Returns false.
	Test Case 4: No flips
		- Setup: Try is_valid_move on an empty square that would not flip any discs.
		- Check: Returns false.
	Test Case 5: Out-of-bounds
		- Setup: Call is_valid_move(-1,0,BLACK) or is_valid_move(0,8,WHITE).
		- Check: Returns false or error.
	Test Case 6: Invalid player value
		- Setup: Call is_valid_move with player not BLACK or WHITE.
		- Check: Returns error or handles gracefully.
	-->
- [ ] process_move(r, c, player): applies move, flips discs, updates board
	<!--
	Test Case 1: Valid move
		- Setup: Initial board, process a valid move for BLACK.
		- Check: Correct discs are flipped, board updated.
	Test Case 2: Invalid move
		- Setup: Try to process an invalid move (occupied, no flips, out-of-bounds).
		- Check: No change to board, error returned.
	Test Case 3: Edge/corner moves
		- Setup: Set up board so a valid move is on an edge or corner, process move.
		- Check: Correct discs flipped.
	Test Case 4: Multi-direction flips
		- Setup: Set up board so a move flips discs in multiple directions, process move.
		- Check: All appropriate discs flipped.
	Test Case 5: Invalid player value
		- Setup: Call process_move with player not BLACK or WHITE.
		- Check: Returns error or handles gracefully.
	-->
- [ ] compute_valid_moves(): computes all valid moves for current player
	<!--
	Test Case 1: Initial board
		- Setup: Create Board, set current player to BLACK.
		- Check: compute_valid_moves returns 4.
	Test Case 2: After a move
		- Setup: Make a valid move, call compute_valid_moves for next player.
		- Check: Returns correct number of moves.
	Test Case 3: No valid moves (pass)
		- Setup: Fill board or set up a position with no valid moves for current player.
		- Check: Returns 0.
	-->
- [ ] get_valid_moves(): returns correct set of valid moves
	<!--
	Test Case 1: Matches compute_valid_moves
		- Setup: After compute_valid_moves, get_valid_moves returns same set.
	Test Case 2: After board changes
		- Setup: Make a move, call get_valid_moves, verify updated set.
	-->
- [ ] parse_move(): parses user input (if testable)
	<!--
	Test Case 1: Valid input
		- Setup: Simulate user input for a valid move (e.g., "3 4").
		- Check: Returns correct coordinates.
	Test Case 2: Invalid input
		- Setup: Simulate non-numeric or out-of-bounds input.
		- Check: Returns error or prompts again.
	-->

## Edge & Game End Cases
- [ ] Moves on edges/corners
	<!--
	Test Case 1: All edge/corner positions
		- Setup: For each edge/corner, set up board so move is valid, process move, check flips.
	-->
- [ ] Valid move after pass
	<!--
	Test Case 1: Player passes, opponent has valid move
		- Setup: Set up board so current player has no valid moves, opponent does.
		- Check: After pass, opponent can make a valid move, game continues.
	-->
- [ ] No valid moves for one/both players
	<!--
	Test Case 1: Pass turn
		- Setup: Set up board so current player has no valid moves, but opponent does.
		- Check: compute_valid_moves returns 0, turn passes.
	-->
- [ ] Full board
	<!--
	Test Case 1: 64 discs, no moves
		- Setup: Fill board, call compute_valid_moves for both players.
		- Check: Returns 0 for both.
	-->
- [ ] Game end detection with mixed full/no-move/multiple passes
	<!--
	Test Case 1: Both players have no valid moves (consecutive passes)
		- Setup: Board state where neither player can move.
		- Check: Game ends after two passes.
	Test Case 2: Board not full, but no valid moves for both
		- Setup: Board not full, but neither player has a valid move.
		- Check: Game ends.
	-->
- [ ] Invalid moves (out of bounds, occupied, etc)
	<!--
	Test Case 1: Negative indices
		- Setup: Call is_valid_move(-1,0,BLACK) or process_move(-1,0,BLACK).
		- Check: Returns false or error.
	Test Case 2: Indices >= 8
		- Setup: Call is_valid_move(8,0,WHITE) or process_move(0,8,WHITE).
		- Check: Returns false or error.
	Test Case 3: Occupied squares
		- Setup: Try to move on a square already occupied.
		- Check: Returns false or error.
	-->
