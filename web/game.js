// The rules of Othello.
//
// The board is a list of 64 squares, read left to right, top to bottom.
// Square 0 is a1 (top left) and square 63 is h8 (bottom right).
// Each square holds EMPTY, BLACK or WHITE.

export const EMPTY = 0
export const BLACK = 1
export const WHITE = 2

const LETTERS = 'abcdefgh'

// The eight ways a line can run from a square: up, down, left, right and diagonals.
const DIRECTIONS = [
  [-1, -1], [-1, 0], [-1, 1],
  [0, -1],           [0, 1],
  [1, -1],  [1, 0],  [1, 1],
]

export function other(color) {
  return color === BLACK ? WHITE : BLACK
}

export function newBoard() {
  const board = new Array(64).fill(EMPTY)
  board[27] = WHITE // d4
  board[28] = BLACK // e4
  board[35] = BLACK // d5
  board[36] = WHITE // e5
  return board
}

export function squareName(square) {
  const row = Math.floor(square / 8)
  const column = square % 8
  return LETTERS[column] + (row + 1)
}

function onBoard(row, column) {
  return row >= 0 && row < 8 && column >= 0 && column < 8
}

// Which discs would turn over if `color` played on `square`.
// An empty list means the move is not allowed.
export function discsToFlip(board, square, color) {
  if (board[square] !== EMPTY) return []

  const row = Math.floor(square / 8)
  const column = square % 8
  const flips = []

  for (const [rowStep, columnStep] of DIRECTIONS) {
    let r = row + rowStep
    let c = column + columnStep
    const line = []

    // Walk over the opponent's discs...
    while (onBoard(r, c) && board[r * 8 + c] === other(color)) {
      line.push(r * 8 + c)
      r += rowStep
      c += columnStep
    }

    // ...and keep them only if one of our own discs closes the line.
    if (line.length > 0 && onBoard(r, c) && board[r * 8 + c] === color) {
      flips.push(...line)
    }
  }

  return flips
}

export function allowedMoves(board, color) {
  const moves = []
  for (let square = 0; square < 64; square++) {
    if (discsToFlip(board, square, color).length > 0) moves.push(square)
  }
  return moves
}

// Returns a new board with the move played. The old board is left as it was.
export function playMove(board, square, color) {
  const next = board.slice()
  next[square] = color
  for (const flipped of discsToFlip(board, square, color)) {
    next[flipped] = color
  }
  return next
}

export function countDiscs(board, color) {
  return board.filter(square => square === color).length
}
