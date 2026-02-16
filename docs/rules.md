### Game logic
#### Individual game
Pseudocode for start_game
```
create and set up the board
while (there are still legal moves for at least one) {
    check for all valid moves on the board and store it
    if there's no moves left for this player {
        continue
    }
    keep parsing user's move from stdin & checking if it's in valid moves
    process the move: flipping
    print the current board (later: modify only no print)
    alternate player
}
summarize stats
destroy the board
```
#### Game loop
Pseudocode for main
```
score for p1, p2
while (playing) {
    start game
    update score
}
print final scores
```

#### Get all valid moves
Pseudocode for is_valid_move()
```
direction_x // can make it a macro
direction_y

if (is occupied) {
    return FALSE
}

for each direction {
    if (neighbor in that direction is op's disc) {
        for (each disc in that path) {
            if (is player's disc) {
                return TRUE
            }
        }
    }
}
return FALSE
```
Pseudocode for get_valid_moves()
```
valid_moves
for each square in the board {
    if (is_valid_move(square, player)) {
        append to valid_moves
    }
}
return valid_moves
```


### Move format
- Spaces are accepted 
- Other than spaces, only 2 char:
    + A-H any cases: Column
    + 1-8: Row