// Lets the AI use every CPU core.
//
// Browsers only let threads share memory when the page arrives with two special
// headers. GitHub Pages can't add headers, so this file installs a small helper
// (a "service worker") that adds them to everything the page loads.
// The very first visit reloads once so the headers take effect.
//
// This one file plays both parts: the code below runs on the page,
// and the same file is installed as the helper.

if (typeof window === 'undefined') {

  // ---- The helper ----

  self.addEventListener('install', () => self.skipWaiting())
  self.addEventListener('activate', (event) => event.waitUntil(self.clients.claim()))

  self.addEventListener('fetch', (event) => {
    event.respondWith(addHeaders(event.request))
  })

  async function addHeaders(request) {
    const response = await fetch(request)

    // Responses from other sites that hide their details can't be changed.
    if (response.status === 0) return response

    const headers = new Headers(response.headers)
    headers.set('Cross-Origin-Opener-Policy', 'same-origin')
    headers.set('Cross-Origin-Embedder-Policy', 'require-corp')

    return new Response(response.body, {
      status: response.status,
      statusText: response.statusText,
      headers,
    })
  }

} else {

  // ---- On the page ----

  const alreadyAllowed = window.crossOriginIsolated
  const canInstall = 'serviceWorker' in navigator && window.isSecureContext

  if (!alreadyAllowed && canInstall) {
    navigator.serviceWorker.register(document.currentScript.src).then(async () => {
      await navigator.serviceWorker.ready

      // Reload once, so this visit gets the headers too. The note stops endless reloads.
      if (!sessionStorage.getItem('reloaded-for-all-cores')) {
        sessionStorage.setItem('reloaded-for-all-cores', 'yes')
        window.location.reload()
      }
    })
  }

}
