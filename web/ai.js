// Asks the C++ engine for the AI's move.
// The engine runs in the background, so the page never freezes while the AI thinks.
// If the engine can't start, a simple backup AI plays instead.

import { EMPTY, allowedMoves, discsToFlip } from './game.js'

// How many moves ahead the engine looks.
const SEARCH_DEPTH = 8

// Use the all-cores engine when the page allows it (see all-cores.js), otherwise the one-core engine.
const useThreads = window.crossOriginIsolated === true
const engine = new Worker(useThreads ? 'engine/othello-threads.js' : 'engine/othello.js')
let engineWorks = true

// Each question to the engine gets a number, so answers never get mixed up.
let nextQuestion = 0
const waitingForAnswer = {}

engine.onmessage = (message) => {
  const { id, square } = message.data
  waitingForAnswer[id](square)
  delete waitingForAnswer[id]
}

engine.onerror = () => {
  engineWorks = false
  // Answer anything still waiting with the backup AI.
  for (const id in waitingForAnswer) waitingForAnswer[id](null)
}

export function chooseMove(board, color) {
  if (!engineWorks) return Promise.resolve(backupMove(board, color))

  return new Promise((resolve) => {
    const id = nextQuestion++
    waitingForAnswer[id] = (square) => {
      resolve(square === null || square < 0 ? backupMove(board, color) : square)
    }
    engine.postMessage({ id, board, color, depth: SEARCH_DEPTH, useThreads })
  })
}


// ---- Backup AI ----
// Takes the move that turns over the most discs, likes corners,
// and avoids the squares right next to an empty corner.

const CORNERS = [0, 7, 56, 63]
const NEXT_TO_CORNER = { 9: 0, 14: 7, 49: 56, 54: 63 }

function backupMove(board, color) {
  let bestMove = null
  let bestScore = -Infinity

  for (const square of allowedMoves(board, color)) {
    let score = discsToFlip(board, square, color).length

    if (CORNERS.includes(square)) score += 20

    const corner = NEXT_TO_CORNER[square]
    if (corner !== undefined && board[corner] === EMPTY) score -= 8

    if (score > bestScore) {
      bestScore = score
      bestMove = square
    }
  }

  return bestMove
}
