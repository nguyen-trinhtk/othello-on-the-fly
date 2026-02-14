### Game logic
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

Pseudocode for main
```
score for p1, p2
while (playing) {
    start game
    update score
}
print final scores
```

### Move format
- Spaces are accepted 
- Other than spaces, only 2 char:
    + A-H any cases: Column
    + 1-8: Row