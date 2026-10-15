// Added to the end of the engine's JavaScript file when it is built.
// The engine file runs in the background as its own worker:
// the page sends it a board, and it sends back the AI's move.

// The engine's helper threads load this same file, but only the main copy answers the page.
if (globalThis.name !== 'em-pthread') {

  const engineReady = new Promise((resolve) => {
    if (runtimeInitialized) resolve()
    else Module.onRuntimeInitialized = resolve
  })

  self.onmessage = async (message) => {
    const { id, board, color, depth, useThreads } = message.data
    await engineReady

    const square = Module.ccall(
      'best_move', 'number',
      ['string', 'number', 'number', 'number'],
      [board.join(''), color, depth, useThreads ? 1 : 0],
    )

    self.postMessage({ id, square })
  }

}
