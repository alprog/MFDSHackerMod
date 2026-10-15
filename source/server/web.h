#pragma once

inline constexpr const char* WEB_PAGE = R"html(<!doctype html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>MFDS Hacker</title>
<style>
  :root { --bg: #fff; --fg: #111; --dim: #777; --line: #ddd; --on: #0a7d32; --err: #c62828; }
  @media (prefers-color-scheme: dark) {
    :root { --bg: #111; --fg: #e8e8e8; --dim: #888; --line: #333; --on: #4caf50; --err: #ef5350; }
  }
  body { background: var(--bg); color: var(--fg); font: 15px/1.4 Consolas, monospace; margin: 0 auto; padding: 16px; max-width: 760px; }
  h2 { font-size: 15px; color: var(--dim); font-weight: normal; margin: 24px 0 8px; border-bottom: 1px solid var(--line); }
  #tx { font-size: 18px; }
  .on { color: var(--on); }
  .err { color: var(--err); }
  .dim { color: var(--dim); }
  input, button { font: inherit; padding: 4px 10px; }
  input { width: 16ch; }
  #log { white-space: pre; overflow-x: auto; }
</style>
</head>
<body>
<h2>Transmitter <span id="txConn" class="dim"></span></h2>
<div id="tx">-</div>
<p>
  <input id="freq" placeholder="Hz" inputmode="decimal">
  <button id="send">Send</button>
  <button id="stop">Stop</button>
  <span id="txError" class="err"></span>
</p>

<h2>Receiver <span id="rxConn" class="dim"></span></h2>
<div id="log"></div>

<script>
const LOG_LINES = 30;
const $ = id => document.getElementById(id);

async function follow(url, onLine, onStatus) {
  while (true) {
    try {
      const response = await fetch(url);
      onStatus('');
      const reader = response.body.pipeThrough(new TextDecoderStream()).getReader();
      let buffer = '';
      while (true) {
        const { value, done } = await reader.read();
        if (done) break;
        buffer += value;
        let end;
        while ((end = buffer.indexOf('\n')) >= 0) {
          onLine(buffer.slice(0, end));
          buffer = buffer.slice(end + 1);
        }
      }
    } catch (e) {}
    onStatus('(disconnected, retrying...)');
    await new Promise(resolve => setTimeout(resolve, 1000));
  }
}

const log = [];
follow('/receiver/log', line => {
  log.push(line);
  if (log.length > LOG_LINES) log.shift();
  $('log').textContent = log.join('\n');
}, status => $('rxConn').textContent = status);

follow('/transmitter/log', line => {
  $('tx').textContent = line;
  $('tx').className = line.includes('TX ON') ? 'on' : '';
}, status => $('txConn').textContent = status);

async function command(url) {
  try {
    const response = await fetch(url);
    $('txError').textContent = response.ok ? '' : await response.text();
  } catch (e) {
    $('txError').textContent = 'Server is not reachable';
  }
}
$('send').onclick = () => command('/transmitter/send?freq=' + encodeURIComponent($('freq').value.trim()));
$('stop').onclick = () => command('/transmitter/stop');
$('freq').onkeydown = e => { if (e.key === 'Enter') $('send').click(); };
</script>
</body>
</html>
)html";
