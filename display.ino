#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// --- PIN DEFINITIONS (Right Header only) ---
#define TFT_CS    5
#define TFT_MOSI  23
#define TFT_SCLK  18
#define TFT_DC    2
#define TFT_RST   4

// Initialize hardware SPI display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// --- ENTER YOUR 2.4GHz HOME WI-FI CREDENTIALS ---
const char* ssid     = "YOUR WIFI NAME";
const char* password = "YOUR WIFI PASSWORD";

// Embedded HTML/CSS/JS Interface with Undo/Redo Engine
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>TFT Live Sketchpad</title>
  <style>
    * { box-sizing: border-box; touch-action: none; }
    body {
      margin: 0;
      padding: 8px;
      background-color: #0d0d0d;
      color: #ffffff;
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
      text-align: center;
      display: flex;
      flex-direction: column;
      align-items: center;
      user-select: none;
      -webkit-user-select: none;
    }
    h2 { margin: 2px 0 4px 0; font-size: 1.1rem; color: #00FFFF; }
    #status { font-size: 0.8rem; color: #888; margin-bottom: 6px; }

    /* Palette Row */
    .palette-row {
      display: flex;
      justify-content: center;
      gap: 8px;
      margin-bottom: 8px;
    }
    .swatch {
      width: 32px;
      height: 32px;
      border-radius: 50%;
      border: 2px solid #333;
      cursor: pointer;
      box-shadow: 0 0 5px rgba(0,0,0,0.6);
      transition: transform 0.1s;
    }
    .swatch.selected {
      border: 3px solid #ffffff;
      transform: scale(1.15);
      box-shadow: 0 0 8px #ffffff;
    }

    /* Slider Bar */
    .slider-box {
      width: 256px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      margin-bottom: 8px;
      font-size: 0.85rem;
      color: #bbb;
    }
    input[type=range] {
      width: 160px;
      accent-color: #00FFFF;
    }

    /* Canvas Screen */
    canvas {
      background: #000000;
      border: 2px solid #333;
      border-radius: 6px;
      display: block;
      margin: 0 auto;
      width: 256px;
      height: 320px;
    }

    /* Action Buttons Row */
    .btn-row {
      display: flex;
      justify-content: space-between;
      width: 256px;
      margin-top: 8px;
      gap: 6px;
    }
    .btn {
      flex: 1;
      padding: 10px 0;
      font-size: 0.85rem;
      font-weight: bold;
      color: #fff;
      background: #2c3e50;
      border: none;
      border-radius: 5px;
      cursor: pointer;
    }
    .btn:active { background: #34495e; }
    .btn-clear { background: #c0392b; }
    .btn-clear:active { background: #e74c3c; }
    .btn:disabled { opacity: 0.4; cursor: not-allowed; }
  </style>
</head>
<body>
  <h2>Live TFT Sketchpad</h2>
  <div id="status">Connecting...</div>

  <!-- 7 High Contrast Colors -->
  <div class="palette-row">
    <div class="swatch selected" style="background:#00FFFF" onclick="setColor('#00FFFF', 0x07FF, this)"></div>
    <div class="swatch" style="background:#00FF00" onclick="setColor('#00FF00', 0x07E0, this)"></div>
    <div class="swatch" style="background:#FFFF00" onclick="setColor('#FFFF00', 0xFFE0, this)"></div>
    <div class="swatch" style="background:#FFA500" onclick="setColor('#FFA500', 0xFD20, this)"></div>
    <div class="swatch" style="background:#FF1493" onclick="setColor('#FF1493', 0xF8B2, this)"></div>
    <div class="swatch" style="background:#BF00FF" onclick="setColor('#BF00FF', 0xB81F, this)"></div>
    <div class="swatch" style="background:#FFFFFF" onclick="setColor('#FFFFFF', 0xFFFF, this)"></div>
  </div>

  <!-- Horizontal Stroke Width Slider -->
  <div class="slider-box">
    <span>Width:</span>
    <input type="range" id="widthSlider" min="1" max="8" value="2" oninput="setWidth(this.value)">
    <span id="widthVal">2px</span>
  </div>

  <canvas id="cv" width="128" height="160"></canvas>

  <!-- Control Buttons -->
  <div class="btn-row">
    <button id="undoBtn" class="btn" onclick="undo()" disabled>Undo</button>
    <button id="redoBtn" class="btn" onclick="redo()" disabled>Redo</button>
    <button class="btn btn-clear" onclick="clearBoard()">Clear</button>
  </div>

  <script>
    const cv = document.getElementById('cv');
    const ctx = cv.getContext('2d');
    const statusDiv = document.getElementById('status');
    const widthVal = document.getElementById('widthVal');
    const undoBtn = document.getElementById('undoBtn');
    const redoBtn = document.getElementById('redoBtn');

    let currentColorHex = '#00FFFF';
    let currentColor16  = 0x07FF;
    let currentWidth    = 2;

    ctx.strokeStyle = currentColorHex;
    ctx.lineWidth = currentWidth;
    ctx.lineCap = 'round';
    ctx.lineJoin = 'round';

    let drawing = false;
    let lastX = 0, lastY = 0;

    // Undo / Redo Stacks
    let historyStack = []; // Array of strokes: [ [ {x0,y0,x1,y1,c,w}, ... ], ... ]
    let redoStack = [];
    let currentStroke = [];

    const ws = new WebSocket('ws://' + window.location.hostname + ':81/');

    ws.onopen = () => {
      statusDiv.innerText = "Connected! Ready to draw.";
      statusDiv.style.color = "#00FF00";
    };

    ws.onclose = () => {
      statusDiv.innerText = "Disconnected from ESP32.";
      statusDiv.style.color = "#FF4444";
    };

    function updateButtons() {
      undoBtn.disabled = historyStack.length === 0;
      redoBtn.disabled = redoStack.length === 0;
    }

    function setColor(hex, rgb565, el) {
      currentColorHex = hex;
      currentColor16 = rgb565;
      ctx.strokeStyle = hex;
      document.querySelectorAll('.swatch').forEach(s => s.classList.remove('selected'));
      el.classList.add('selected');
      document.getElementById('widthSlider').style.accentColor = hex;
    }

    function setWidth(val) {
      currentWidth = parseInt(val);
      ctx.lineWidth = currentWidth;
      widthVal.innerText = val + 'px';
    }

    function getCoords(e) {
      const rect = cv.getBoundingClientRect();
      const clientX = e.touches ? e.touches[0].clientX : e.clientX;
      const clientY = e.touches ? e.touches[0].clientY : e.clientY;

      let x = Math.round((clientX - rect.left) * (cv.width / rect.width));
      let y = Math.round((clientY - rect.top) * (cv.height / rect.height));

      x = Math.max(0, Math.min(127, x));
      y = Math.max(0, Math.min(159, y));

      return [x, y];
    }

    function startDraw(e) {
      drawing = true;
      currentStroke = [];
      [lastX, lastY] = getCoords(e);
      e.preventDefault();
    }

    function draw(e) {
      if (!drawing) return;
      const [x, y] = getCoords(e);

      // Local Canvas Line Render
      ctx.beginPath();
      ctx.moveTo(lastX, lastY);
      ctx.lineTo(x, y);
      ctx.stroke();

      const seg = {
        x0: lastX, y0: lastY,
        x1: x, y1: y,
        c: currentColor16,
        hex: currentColorHex,
        w: currentWidth
      };

      currentStroke.push(seg);

      // Real-time stream packet to ESP32: L,x0,y0,x1,y1,color,width
      if (ws.readyState === WebSocket.OPEN) {
        ws.send("L," + lastX + "," + lastY + "," + x + "," + y + "," + currentColor16 + "," + currentWidth);
      }

      [lastX, lastY] = [x, y];
      e.preventDefault();
    }

    function stopDraw() {
      if (!drawing) return;
      drawing = false;
      if (currentStroke.length > 0) {
        historyStack.push(currentStroke);
        redoStack = []; // Clear redo stack on new action
        updateButtons();
      }
    }

    // Redraws the local canvas from history
    function redrawLocalCanvas() {
      ctx.clearRect(0, 0, cv.width, cv.height);
      for (const stroke of historyStack) {
        for (const seg of stroke) {
          ctx.beginPath();
          ctx.strokeStyle = seg.hex;
          ctx.lineWidth = seg.w;
          ctx.moveTo(seg.x0, seg.y0);
          ctx.lineTo(seg.x1, seg.y1);
          ctx.stroke();
        }
      }
      ctx.strokeStyle = currentColorHex;
      ctx.lineWidth = currentWidth;
    }

    // Replays the entire history stack to the TFT display
    function syncToESP32() {
      if (ws.readyState !== WebSocket.OPEN) return;
      
      // Step 1: Clear TFT
      ws.send("C");

      // Step 2: Stream all remaining line segments
      for (const stroke of historyStack) {
        for (const seg of stroke) {
          ws.send("L," + seg.x0 + "," + seg.y0 + "," + seg.x1 + "," + seg.y1 + "," + seg.c + "," + seg.w);
        }
      }
    }

    function undo() {
      if (historyStack.length === 0) return;
      const undone = historyStack.pop();
      redoStack.push(undone);
      updateButtons();
      redrawLocalCanvas();
      syncToESP32();
    }

    function redo() {
      if (redoStack.length === 0) return;
      const redone = redoStack.pop();
      historyStack.push(redone);
      updateButtons();
      redrawLocalCanvas();
      syncToESP32();
    }

    function clearBoard() {
      if (historyStack.length === 0) return;
      historyStack = [];
      redoStack = [];
      updateButtons();
      ctx.clearRect(0, 0, cv.width, cv.height);
      if (ws.readyState === WebSocket.OPEN) {
        ws.send("C");
      }
    }

    cv.addEventListener('touchstart', startDraw, { passive: false });
    cv.addEventListener('touchmove', draw, { passive: false });
    window.addEventListener('touchend', stopDraw);

    cv.addEventListener('mousedown', startDraw);
    cv.addEventListener('mousemove', draw);
    window.addEventListener('mouseup', stopDraw);
  </script>
</body>
</html>
)rawliteral";

// Smooth thick-line drawer for TFT
void drawThickLine(int x0, int y0, int x1, int y1, uint16_t color, int width) {
  if (width <= 1) {
    tft.drawLine(x0, y0, x1, y1, color);
    return;
  }

  int radius = width / 2;
  tft.fillCircle(x0, y0, radius, color);
  tft.fillCircle(x1, y1, radius, color);

  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);

  if (dx >= dy) {
    for (int offset = -radius; offset <= radius; offset++) {
      tft.drawLine(x0, y0 + offset, x1, y1 + offset, color);
    }
  } else {
    for (int offset = -radius; offset <= radius; offset++) {
      tft.drawLine(x0 + offset, y0, x1 + offset, y1, color);
    }
  }
}

// WebSocket Event Processor
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_CONNECTED) {
    Serial.println(">>> Client Connected! <<<");
  } else if (type == WStype_TEXT) {
    if (payload[0] == 'L') {
      int x0, y0, x1, y1;
      unsigned int col;
      int w;
      if (sscanf((char*)payload, "L,%d,%d,%d,%d,%u,%d", &x0, &y0, &x1, &y1, &col, &w) == 6) {
        drawThickLine(x0, y0, x1, y1, (uint16_t)col, w);
      }
    } else if (payload[0] == 'C') {
      tft.fillScreen(ST77XX_BLACK);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize display (Black Background)
  tft.initR(INITR_BLACKTAB); // Change to INITR_18REDTAB if colors or edges offset
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(1);
  tft.setCursor(10, 20);
  tft.println("Connecting to Wi-Fi...");

  // Connect to home Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("\n");
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Terminal Output Banner
  Serial.println("\n\n========================================================");
  Serial.println(">>> WI-FI CONNECTED SUCCESSFULLY! <<<");
  Serial.println("========================================================");
  Serial.println("STEP 1: Make sure your phone is connected to this Wi-Fi:");
  Serial.print("        Network: ");
  Serial.println(ssid);
  Serial.println("--------------------------------------------------------");
  Serial.println("STEP 2: Open Chrome or Safari on your phone.");
  Serial.println("--------------------------------------------------------");
  Serial.println("STEP 3: Go to this link to start drawing:");
  Serial.print("        http://");
  Serial.println(WiFi.localIP());
  Serial.println("========================================================\n");

  // Show status & IP on TFT
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 20);
  tft.println("Ready to Draw!");
  tft.setCursor(10, 45);
  tft.println("Open on phone:");
  tft.setCursor(10, 60);
  tft.setTextColor(ST77XX_CYAN);
  tft.println(WiFi.localIP().toString());

  // Web and WebSocket Servers
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", index_html);
  });
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();
}
