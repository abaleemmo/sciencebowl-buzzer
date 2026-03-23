# Science Bowl Buzzer

A hardware + software buzzer system for practicing [National Science Bowl](https://science.osti.gov/wdts/nsb/), built with ESP32 microcontrollers and a Node.js WebSocket server. Features a polished web frontend with a live scoreboard, match timer, question timer, and full event log.

Made for the goat Mr. Bullington, because these buzzer systems are way too expensive for no reason.
---

## How It Works

Each team has a physical buzzer unit (an ESP32 with a button and an LED). When a player presses their button, the ESP32 sends a WebSocket message to a central Node.js server running on your local network. The server applies a first-in-wins lock — only the first buzz is accepted, all others are dropped — and broadcasts the result to every connected client. The web frontend updates in real time for everyone watching.

```
[ESP32 #1] ──┐
             ├──► [Node.js Server] ──► [Browser Frontend]
[ESP32 #2] ──┘
```

---

## Features

- **Hardware buzzer lockout** — first button press wins, all others are ignored until reset
- **Live scoreboard** — Team A vs Team B with +4 Tossup, +10 Bonus, and Blurt (−4) buttons
- **Match (half) timer** — configurable duration, with color warnings as time runs low
- **Question timer** — one-click presets for Tossup (5s) and Bonus (20s) with a draining progress bar
- **Event log** — timestamped record of every buzz, score change, blurt, undo, and timer event
- **Auto-reset** — buzzer clears automatically when any score button is pressed
- **WebSocket auto-reconnect** — frontend and server recover gracefully from dropped connections

---

## Hardware Requirements

- 1× ESP32 development board per buzzer (tested on standard ESP32-WROOM)
- 1× momentary push button per buzzer (wired to GPIO 4)
- 1× LED per buzzer (wired to GPIO 12, active LOW)
- 1x 220 Ohm resistor per buzzer
- A computer to run the Node.js server
- All devices on the same local WiFi network

---

## Software Requirements

- [Node.js](https://nodejs.org/) (v14 or later)
- [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/) for flashing the ESP32
- The following Arduino libraries (install via Library Manager):
  - `WiFi` (built-in)
  - `WebSocketsClient` by Markus Sattler

---

## Setup & Installation

### 1. Clone the repo

```bash
git clone https://github.com/abaleemmo/sciencebowl-buzzer.git
cd sciencebowl-buzzer
```

### 2. Set up the Node.js server

```bash
npm install
node server.js
```

The server will start on `http://0.0.0.0:3000`. Note the local IP address of the machine running it (e.g. `192.168.1.15`) — you'll need it in the next two steps.

### 3. Configure the frontend

Open `index.html` and go to **line 519**. Change the WebSocket URL to your server's local IP:

```js
const SERVER = "ws://192.168.1.15:3000"; // ← replace with your server's IP
```

Then open `index.html` in a browser on any device connected to the same network.

### 4. Flash the ESP32

Open `main.cpp` in Arduino IDE or PlatformIO and update the following lines:

| Line | Variable | What to change it to |
|------|----------|----------------------|
| 4 | `ssid` | Your WiFi network name |
| 5 | `password` | Your WiFi password |
| 6 | `serverIP` | Your server's local IP address |
| 8 | `PLAYER_ID` | A unique number for each buzzer (1, 2, 3…) |

Flash the code to each ESP32. The onboard LED will indicate connection status, and the buzzer LED (GPIO 12) will light up when that player buzzes in.

---

## Usage

1. Start the server (`node server.js`)
2. Open `index.html` in a browser
3. Power on the ESP32 buzzers — they will connect to the server automatically
4. Press a buzzer button to buzz in — the frontend will display the winner and lock out all other buzzers
5. Use the **Reset Buzzer** button (or any score button) on the frontend to clear and start the next question

---

## Project Structure

```
sciencebowl-buzzer/
├── main.cpp        # ESP32 firmware (Arduino/PlatformIO)
├── server.js       # Node.js WebSocket + HTTP server
├── index.html      # Web frontend (scoreboard, timers, buzzer display)
├── package.json    # Node.js dependencies
└── README.md
```

---

## Disclaimer

`index.html` was designed and written by [Claude](https://claude.ai) (Anthropic's AI assistant). Claude was also used throughout the project to debug the ESP32 firmware and WebSocket logic. The hardware wiring, system architecture, and integration were done by the project author.

---

## License

MIT — do whatever you want with it, just don't blame me if your science bowl team loses.
