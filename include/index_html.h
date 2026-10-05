#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="uk">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ATE Optical Tester</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #0b0f19; color: #f1f5f9; padding: 20px; }
        .header { display: flex; justify-content: space-between; align-items: center; padding-bottom: 15px; border-bottom: 1px solid #1e293b; margin-bottom: 20px; }
        .brand { font-size: 1.25rem; font-weight: bold; color: #38bdf8; display: flex; align-items: center; gap: 8px; }
        .controls { display: flex; gap: 12px; margin-bottom: 24px; }
        .btn { padding: 12px 24px; font-weight: 600; border: none; border-radius: 6px; cursor: pointer; font-size: 0.9rem; transition: all 0.2s; }
        .btn-start { background: #0284c7; color: white; }
        .btn-start:hover { background: #0369a1; }
        .btn-stop { background: #dc2626; color: white; }
        .btn-stop:hover { background: #b91c1c; }
        .btn-reset { background: #1e293b; color: #94a3b8; border: 1px solid #334155; }
        .btn-reset:hover { background: #334155; color: white; }
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(210px, 1fr)); gap: 16px; margin-bottom: 24px; }
        .card { background: #151d30; padding: 16px; border-radius: 8px; border: 1px solid #1e293b; }
        .card-label { font-size: 0.75rem; font-weight: 600; color: #64748b; text-transform: uppercase; letter-spacing: 0.5px; margin-bottom: 8px; }
        .card-val { font-size: 1.5rem; font-weight: bold; font-family: 'Courier New', Courier, monospace; }
        .c-blue { color: #38bdf8; }
        .c-green { color: #4ade80; }
        .c-red { color: #f87171; }
        .c-yellow { color: #facc15; }
        .console-box { background: #030712; border: 1px solid #1e293b; border-radius: 8px; padding: 16px; font-family: monospace; font-size: 0.85rem; height: 160px; overflow-y: auto; color: #a1a1aa; }
        .log-time { color: #475569; margin-right: 8px; }
    </style>
</head>
<body>

    <div class="header">
        <div class="brand">⚡ ATE OPTICAL TESTER</div>        
    </div>

    <div class="controls">
        <button class="btn btn-start" id="startBtn" onclick="toggleTest()">START TEST</button>
        <button class="btn btn-reset" onclick="resetStats()">RESET STATS</button>
    </div>

    <div class="grid">
        <div class="card">
            <div class="card-label">Packets Sent / Recv</div>
            <div class="card-val c-blue"><span id="sent">0</span> / <span id="recv">0</span></div>
        </div>
        <div class="card">
            <div class="card-label">Link Quality (LQ)</div>
            <div class="card-val c-green"><span id="exp-id">100.0</span>%</div>
        </div>
        <div class="card">
            <div class="card-label">CRC8 Errors</div>
            <div class="card-val c-red" id="crc">0</div>
        </div>
        <div class="card">
            <div class="card-label">Packet Loss</div>
            <div class="card-val c-yellow" id="ber">0.00%</div>
        </div>
        <div class="card">
            <div class="card-label">Latency / RTT</div>
            <div class="card-val c-blue" id="rtt">0.0 ms</div>
        </div>
    </div>

    <div class="console-box" id="log">
        <div><span class="log-time">[SYSTEM]</span> Connected via HTTP AP.</div>
    </div>

    <script>
        let isRunning = false;

        function log(msg) {
            const consoleEl = document.getElementById('log');
            const time = new Date().toLocaleTimeString();
            consoleEl.innerHTML += `<div><span class="log-time">[${time}]</span> ${msg}</div>`;
            consoleEl.scrollTop = consoleEl.scrollHeight;
        }

        async function fetchTelemetry() {
            try {
                const res = await fetch('/api/data');
                if (res.ok) {
                    const data = await res.json();
                    document.getElementById('sent').innerText = data.sent;
                    document.getElementById('recv').innerText = data.recv;

                    const serverState = Boolean(data.running);
                    if (serverState !== isRunning) {
                        isRunning = serverState;
                        const btn = document.getElementById('startBtn');
                        if (isRunning) {
                            btn.innerText = 'STOP TEST';
                            btn.className = 'btn btn-stop';
                            log('<span style="color:#4ade80">[CMD] Test Started</span>');
                        } else {
                            btn.innerText = 'START TEST';
                            btn.className = 'btn btn-start';
                            log('<span style="color:#f87171">[CMD] Test Stopped</span>');
                        }
                    }
                }
            } catch (e) {
                // Помилка мережі
            }
        }

        async function toggleTest() {
            try {
                await fetch('/api/toggle', { method: 'POST' });
                setTimeout(fetchTelemetry, 50);
            } catch (e) {
                log('<span style="color:#f87171">[ERR] Failed to toggle test</span>');
            }
        }

        async function resetStats() {
            try {
                await fetch('/api/reset', { method: 'POST' });
                document.getElementById('sent').innerText = '0';
                document.getElementById('recv').innerText = '0';
                log('<span style="color:#38bdf8">[CMD] Stats Reset</span>');
            } catch (e) {
                log('<span style="color:#f87171">[ERR] Failed to reset stats</span>');
            }
        }

        setInterval(fetchTelemetry, 300);
        fetchTelemetry();
    </script>
</body>
</html>
)rawliteral";

#endif