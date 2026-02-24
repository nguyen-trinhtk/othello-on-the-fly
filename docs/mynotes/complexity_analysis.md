### Naive implementation
- is_valid_move: O(D) (check all directions for flips, D = number of directions = 8)
- get_valid_moves: O(BOARD_SIZE^2 * D) (check is_valid_move for all squares)
- process_move: O(D) (fixed number of directions, board size is constant)
- Valid moves: vector -> O(n) lookup, O(1) insert at end

### Bitboard & unordered_set implementation
- is_valid_move: O(1) (unordered_set lookup)
- get_valid_moves: O(BOARD_SIZE^2) (bitwise scan, but constant for 8x8)
- process_move: O(1) per direction, O(1) overall (bitwise, fixed directions)
- Valid moves: unordered_set -> O(1) average lookup and insert

