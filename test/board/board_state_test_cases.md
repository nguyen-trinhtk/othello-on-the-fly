# Board State Test Checklist

## Core Board State
- [ ] Constructor: initializes correct starting position
	<!--
	Test Case 1: Default construction
		- Setup: Create Board object with default constructor.
		- Check: Board is 8x8 (all indices 0-7 valid).
		- Check: Center 4 discs are correct ([3,3]=WHITE, [3,4]=BLACK, [4,3]=BLACK, [4,4]=WHITE).
		- Check: All other squares are EMPTY.
		- Check: Current player is BLACK (or as per rules).
	Test Case 2: Multiple Board objects
		- Setup: Create two Board objects.
		- Check: Both are independent, initial state correct for both.
	-->
- [ ] get_square(r, c): returns correct disc for all positions
	<!--
	Test Case 1: Valid indices
		- Setup: Create Board object.
		- Check: get_square returns correct value for all positions (center, edge, corner).
	Test Case 2: Out-of-bounds
		- Setup: Call get_square with r or c < 0 or >= 8.
		- Check: Returns error or throws exception.
	-->
- [ ] set_square(r, c, disc): sets disc correctly, handles invalid input
	<!--
	Test Case 1: Set BLACK
		- Setup: Create Board, set [2,2] to BLACK.
		- Check: get_square([2,2]) == BLACK.
	Test Case 2: Set WHITE
		- Setup: Create Board, set [5,5] to WHITE.
		- Check: get_square([5,5]) == WHITE.
	Test Case 3: Overwrite disc
		- Setup: Set [2,2] to BLACK, then to WHITE.
		- Check: get_square([2,2]) == WHITE.
	Test Case 4: Invalid disc value
		- Setup: set_square([1,1], 99).
		- Check: Returns error.
	Test Case 5: Out-of-bounds
		- Setup: set_square([-1,0], BLACK) or set_square([0,8], WHITE).
		- Check: Returns error or throws exception.
	-->
- [ ] print_square(disc): returns correct symbol for each disc type
	<!--
	Test Case 1: BLACK
		- Setup: Call print_square(BLACK).
		- Check: Returns BLACK symbol.
	Test Case 2: WHITE
		- Setup: Call print_square(WHITE).
		- Check: Returns WHITE symbol.
	Test Case 3: EMPTY
		- Setup: Call print_square(EMPTY).
		- Check: Returns EMPTY symbol.
	Test Case 4: Invalid value
		- Setup: Call print_square(99).
		- Check: Returns INVALID symbol or error.
	-->
- [ ] print_board(): prints correct board state
	<!--
	Test Case 1: Initial board
		- Setup: Create Board, call print_board.
		- Check: Output matches expected initial layout.
	Test Case 2: After moves
		- Setup: Make several moves, call print_board.
		- Check: Output matches expected state.
	Test Case 3: Full board
		- Setup: Fill board, call print_board.
		- Check: Output shows all squares filled.
	-->

## Game State

- [ ] get_black_count(), get_white_count(): correct disc counts
	<!--
	Test Case 1: Initial board
		- Setup: Create Board.
		- Check: get_black_count() == 2, get_white_count() == 2.
	Test Case 2: After moves
		- Setup: Make moves, check counts after each.
	Test Case 3: Full board
		- Setup: Fill board with known pattern, check counts.
	-->
- [ ] evaluate(player): correct evaluation for both players
	<!--
	Test Case 1: Initial board
		- Setup: Create Board.
		- Check: evaluate(BLACK) == 0, evaluate(WHITE) == 0.
	Test Case 2: After moves
		- Setup: Make moves, check evaluation after each.
	Test Case 3: More BLACK or WHITE discs
		- Setup: Fill board with more BLACK or WHITE, check evaluation sign and value.
	-->
- [ ] is_game_over(): detects end of game
	<!--
	Test Case 1: Not over
		- Setup: Initial board, is_game_over() == false.
	Test Case 2: Full board
		- Setup: Fill board, is_game_over() == true.
	Test Case 3: No valid moves for both
		- Setup: Set up board with no valid moves, is_game_over() == true.
	-->
- [ ] get_current_player(), set_current_player(): gets/sets turn correctly
	<!--
	Test Case 1: Initial player
		- Setup: Create Board, check current player.
	Test Case 2: Set to WHITE
		- Setup: set_current_player(WHITE), check.
	Test Case 3: Set to BLACK
		- Setup: set_current_player(BLACK), check.
	Test Case 4: Switch after move
		- Setup: Make move, check if turn switches.
	-->

## Endgame/Stats

- [ ] sum_game_stats(): outputs correct stats at game end
	<!--
	Test Case 1: Known board
		- Setup: Fill board with known counts, call sum_game_stats.
		- Check: Output shows correct winner/loser and counts.
	-->
- [ ] Destructor: cleans up if needed
	<!--
	Test Case 1: Memory cleanup
		- Setup: Create and destroy Board object.
		- Check: No memory leaks (use valgrind or similar tool).
	-->
