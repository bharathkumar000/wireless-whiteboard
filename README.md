# Wireless Whiteboard 🎨📱
> **Real-Time ESP32 + ST7735 TFT Live Sketchpad with WebSockets & Web UI**

An interactive, zero-latency wireless sketching system powered by an **ESP32** microcontroller and an **ST7735 1.8" TFT display (128x160)**. Draw directly from any phone, tablet, or desktop web browser using a smooth, responsive HTML5 canvas touch interface, and watch your sketches render on the hardware screen instantly over Wi-Fi.

📖 **For detailed step-by-step assembly, uploading, and operation instructions, see the [Usage & User Guide](USAGE_GUIDE.md).**

---

## 🌟 Key Features

- ⚡ **Real-Time WebSocket Streaming**: Instant bi-directional communication between your phone's browser and the ESP32 display via WebSockets (`ws://<IP>:81/`).
- 📱 **Mobile-Optimized Touch Web Interface**: Native touch & gesture support with smooth line rendering.
- 🎨 **High-Contrast Palette**: 7 vibrant presets (Cyan, Green, Yellow, Orange, Deep Pink, Purple, White) optimized for TFT displays.
- ✏️ **Dynamic Stroke Thickness**: Adjustable brush width from `1px` to `8px` with anti-aliased circle endpoints for smooth rendering.
- ↩️ **Undo / Redo Engine**: Full history stack management on the web client with instant screen re-synchronization to the TFT display.
- 📡 **Self-Contained Web Server**: Serves embedded lightweight web apps directly from ESP32 flash memory (`PROGMEM`).
- 📟 **On-Screen IP Status Display**: Shows Wi-Fi connection status and local server IP address on the TFT display during boot.

---

## 🔌 Hardware Pinout (ESP32 to ST7735 SPI Display)

| ST7735 Pin | ESP32 Pin | Function Description |
| :--- | :--- | :--- |
| **VCC** | `3.3V` / `5V` | Power Supply |
| **GND** | `GND` | Ground |
| **CS** | `GPIO 5` | SPI Chip Select |
| **RESET / RST** | `GPIO 4` | Hardware Reset |
| **A0 / DC** | `GPIO 2` | Data / Command Control |
| **SDA / MOSI** | `GPIO 23` | SPI Data Input |
| **SCK / SCLK** | `GPIO 18` | SPI Clock Input |
| **LED / BLK** | `3.3V` | Backlight Power |

---

## 🛠️ Software Dependencies

Make sure the following libraries are installed in your **Arduino IDE** (via *Tools -> Manage Libraries*):

1. **Adafruit GFX Library** (`Adafruit_GFX.h`)
2. **Adafruit ST7735 and ST7789 Library** (`Adafruit_ST7735.h`)
3. **WebSockets by Markus Sattler** (`WebSocketsServer.h`)
4. **WiFi** & **WebServer** *(Built-in with ESP32 board package)*

---

## 🚀 Quick Start & Installation Guide

### 1. Clone the Repository
```bash
git clone https://github.com/bharathkumar000/wireless-whiteboard.git
cd wireless-whiteboard
```

### 2. Configure Wi-Fi Credentials
Open [`display.ino`](file:///Users/bharathkumara/Desktop/display/display.ino) in the Arduino IDE and update lines 22–23 with your 2.4GHz Wi-Fi credentials:
```cpp
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 3. Upload Code to ESP32
1. Connect your ESP32 board to your computer via USB.
2. Select **ESP32 Dev Module** under *Tools -> Board -> ESP32 Arduino*.
3. Select your serial COM / TTY port under *Tools -> Port*.
4. Click **Upload**.

### 4. Connect & Draw!
1. Open the **Serial Monitor** at `115200 baud`.
2. Once connected to Wi-Fi, the ESP32 will output its local IP address (and display it on the TFT screen, e.g., `http://192.168.1.50`).
3. Open Chrome, Safari, or Firefox on any smartphone or computer connected to the **same Wi-Fi network**.
4. Navigate to `http://<ESP32_IP_ADDRESS>` and start drawing!

---

## 🏗️ Architecture & Communication Flow

```mermaid
flowchart LR
    A[Mobile Phone / Browser UI] -- "1. Touch Event (x0,y0 -> x1,y1)" --> B[HTML5 Canvas]
    B -- "2. WebSocket Packet: L,x0,y0,x1,y1,col,width" --> C[ESP32 WebSocketsServer :81]
    C -- "3. Parse Command (sscanf)" --> D[drawThickLine()]
    D -- "4. Hardware SPI Bus" --> E[ST7735 128x160 TFT Screen]
    B -- "5. Undo Action -> Clear 'C' + Replay Stream" --> C
```

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
