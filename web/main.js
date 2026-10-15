// Runs the page: draws the board, handles clicks, and takes turns with the AI.

import { BLACK, WHITE, other, newBoard, allowedMoves, discsToFlip, playMove, countDiscs, squareName } from './game.js'
import { chooseMove } from './ai.js'

const COLOR_NAMES = { [BLACK]: 'Black', [WHITE]: 'White' }
const AI_DELAY = 500 // milliseconds the AI "thinks" before moving

const page = {
  squares: document.querySelector('#squares'),
  status: document.querySelector('#status'),
  blackCount: document.querySelector('#black-count'),
  whiteCount: document.querySelector('#white-count'),
  blackWho: document.querySelector('#black-who'),
  whiteWho: document.querySelector('#white-who'),
  switchSide: document.querySelector('#switch-side'),
  undo: document.querySelector('#undo'),
  newGame: document.querySelector('#new-game'),
  result: document.querySelector('#result'),
  resultTitle: document.querySelector('#result-title'),
  finalBlack: document.querySelector('#final-black'),
  finalWhite: document.querySelector('#final-white'),
  finalBlackWho: document.querySelector('#final-black-who'),
  finalWhiteWho: document.querySelector('#final-white-who'),
  playAgain: document.querySelector('#play-again'),
  closeResult: document.querySelector('#close-result'),
}

// Everything about the game being played.
let game = {}
let aiQuestion = 0 // goes up each time the AI is asked, or the question is cancelled

// The 64 square buttons and the disc inside each one.
const squareButtons = []
const discs = []


// ---- Building the board ----

// A number between -1 and 1 that is always the same for a given square.
// Used to make every disc look a little different, as if drawn by hand.
function wobble(square, n) {
  const x = Math.sin(square * 12.9898 + n * 78.233) * 43758.5453
  return (x - Math.floor(x)) * 2 - 1
}

function giveHandDrawnLook(disc, square) {
  const shape = [1, 2, 3, 4].map(n => Math.round(50 + 7 * wobble(square, n)) + '%')

  disc.style.setProperty('--disc-width', (74 + 4 * wobble(square, 5)).toFixed(1) + '%')
  disc.style.setProperty('--disc-height', (74 + 4 * wobble(square, 6)).toFixed(1) + '%')
  disc.style.setProperty('--disc-x', (3 * wobble(square, 7)).toFixed(1) + '%')
  disc.style.setProperty('--disc-y', (3 * wobble(square, 8)).toFixed(1) + '%')
  disc.style.setProperty('--disc-turn', Math.round(25 * wobble(square, 9)) + 'deg')
  disc.style.setProperty('--disc-shape', `${shape[0]} ${shape[1]} ${shape[2]} ${shape[3]} / ${shape[1]} ${shape[3]} ${shape[0]} ${shape[2]}`)
}

for (let square = 0; square < 64; square++) {
  const button = document.createElement('button')
  button.className = 'square'
  button.innerHTML = '<span class="disc"></span><span class="hint"></span><span class="dot"></span>'
  button.addEventListener('click', () => playerMoves(square))

  const disc = button.querySelector('.disc')
  giveHandDrawnLook(disc, square)

  page.squares.append(button)
  squareButtons.push(button)
  discs.push(disc)
}


// ---- Playing ----

function startGame(yourColor) {
  aiQuestion++
  game = {
    board: newBoard(),
    you: yourColor,
    turn: BLACK,
    lastMove: null,
    history: [],
    aiThinking: false,
    over: false,
    message: '',
  }
  page.result.close()
  draw()
  if (game.turn !== game.you) aiMoves()
}

function playerMoves(square) {
  const yourTurn = game.turn === game.you && !game.aiThinking && !game.over
  if (!yourTurn) return
  if (discsToFlip(game.board, square, game.you).length === 0) return

  game.history.push({ board: game.board, lastMove: game.lastMove })
  game.board = playMove(game.board, square, game.you)
  game.lastMove = square
  nextTurn(game.you)
}

async function aiMoves() {
  game.aiThinking = true
  draw()

  const thisQuestion = ++aiQuestion
  const aiColor = other(game.you)

  // Wait for the engine's answer, and at least AI_DELAY so the move doesn't feel instant.
  const [square] = await Promise.all([
    chooseMove(game.board, aiColor),
    new Promise((resolve) => setTimeout(resolve, AI_DELAY)),
  ])

  // Ignore the answer if a new game started or the player pressed undo meanwhile.
  if (thisQuestion !== aiQuestion) return

  game.board = playMove(game.board, square, aiColor)
  game.lastMove = square
  game.aiThinking = false
  nextTurn(aiColor)
}

// Decides who plays next after `justMoved` has moved.
function nextTurn(justMoved) {
  const opponent = other(justMoved)
  game.message = ''

  if (allowedMoves(game.board, opponent).length > 0) {
    game.turn = opponent
  } else if (allowedMoves(game.board, justMoved).length > 0) {
    // The opponent has nowhere to play, so they pass.
    game.turn = justMoved
    game.message = opponent === game.you ? 'You pass. ' : 'AI passes. '
  } else {
    game.over = true
  }

  draw()

  if (game.over) showResult()
  else if (game.turn !== game.you) aiMoves()
}

function undo() {
  if (game.history.length === 0) return
  aiQuestion++

  const before = game.history.pop()
  game.board = before.board
  game.lastMove = before.lastMove
  game.turn = game.you
  game.aiThinking = false
  game.over = false
  game.message = ''
  page.result.close()
  draw()
}


// ---- Drawing ----

function draw() {
  const yourTurn = game.turn === game.you && !game.aiThinking && !game.over
  const moves = yourTurn ? allowedMoves(game.board, game.you) : []

  for (let square = 0; square < 64; square++) {
    const color = game.board[square]
    const canPlay = moves.includes(square)

    discs[square].classList.toggle('black', color === BLACK)
    discs[square].classList.toggle('white', color === WHITE)
    squareButtons[square].classList.toggle('can', canPlay)
    squareButtons[square].classList.toggle('last', square === game.lastMove)

    let label = squareName(square)
    if (color === BLACK) label += ', black'
    else if (color === WHITE) label += ', white'
    else if (canPlay) label += ', you can play here'
    squareButtons[square].setAttribute('aria-label', label)
  }

  page.blackCount.textContent = countDiscs(game.board, BLACK)
  page.whiteCount.textContent = countDiscs(game.board, WHITE)
  page.blackWho.textContent = game.you === BLACK ? 'You' : 'AI'
  page.whiteWho.textContent = game.you === WHITE ? 'You' : 'AI'

  if (game.over) {
    page.status.textContent = 'Game over'
  } else if (game.aiThinking) {
    page.status.textContent = game.message + 'AI thinking (' + COLOR_NAMES[other(game.you)] + ')'
  } else {
    page.status.textContent = game.message + 'Your move (' + COLOR_NAMES[game.you] + ')'
  }

  page.switchSide.textContent = 'Play as ' + COLOR_NAMES[other(game.you)].toLowerCase() + ' instead'
}

function showResult() {
  const yours = countDiscs(game.board, game.you)
  const ais = countDiscs(game.board, other(game.you))

  if (yours > ais) page.resultTitle.textContent = 'You Win!'
  else if (yours < ais) page.resultTitle.textContent = 'AI Wins'
  else page.resultTitle.textContent = 'Draw'

  page.finalBlack.textContent = countDiscs(game.board, BLACK)
  page.finalWhite.textContent = countDiscs(game.board, WHITE)
  page.finalBlackWho.textContent = game.you === BLACK ? 'You' : 'AI'
  page.finalWhiteWho.textContent = game.you === WHITE ? 'You' : 'AI'

  page.result.showModal()
}


// ---- Buttons ----

page.undo.addEventListener('click', undo)
page.newGame.addEventListener('click', () => startGame(game.you))
page.switchSide.addEventListener('click', () => startGame(other(game.you)))
page.playAgain.addEventListener('click', () => startGame(game.you))
page.closeResult.addEventListener('click', () => page.result.close())

startGame(BLACK)
