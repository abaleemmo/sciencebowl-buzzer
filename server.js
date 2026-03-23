const express = require("express");
const http    = require("http");
const WebSocket = require("ws");

const app  = express();
const PORT = 3000;

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

let buzzed      = false;
let firstPlayer = null;

app.get("/status", (req, res) => res.json({ buzzed, firstPlayer }));

app.post("/reset", (req, res) => {
  reset();
  res.json({ success: true });
});

const server = http.createServer(app);
const wss    = new WebSocket.Server({ server });

// Keepalive ping every 30s — prevents ESP32 from silently disconnecting
const keepalive = setInterval(() => {
  wss.clients.forEach((ws) => {
    if (ws.isAlive === false) { ws.terminate(); return; }
    ws.isAlive = false;
    ws.ping();
  });
}, 30000);

wss.on("close", () => clearInterval(keepalive));

wss.on("connection", (ws) => {
  ws.isAlive = true;
  ws.on("pong", () => { ws.isAlive = true; });

  console.log("Client connected");
  ws.send(JSON.stringify({ type: "status", buzzed, firstPlayer }));

  ws.on("message", (message) => {
    try {
      const data = JSON.parse(message);

      if (data.type === "buzz" && !buzzed) {
        buzzed      = true;
        firstPlayer = data.player;
        console.log(`Player ${data.player} buzzed first!`);
        broadcast({ type: "buzz", player: data.player });
      }

      if (data.type === "reset") {
        reset();
      }
    } catch (e) {
      console.error("Invalid message:", message);
    }
  });

  ws.on("close", () => console.log("Client disconnected"));
});

function reset() {
  buzzed      = false;
  firstPlayer = null;
  broadcast({ type: "reset" });
  console.log("Reset");
}

function broadcast(msg) {
  const data = JSON.stringify(msg);
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) client.send(data);
  });
}

server.listen(PORT, "0.0.0.0", () => {
  console.log(`Server running on http://0.0.0.0:${PORT}`);
});
