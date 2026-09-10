# Wireless Whiteboard - Complete User & Usage Guide 📖🎨

Welcome to the comprehensive step-by-step user guide for setting up and operating the **ESP32 + ST7735 TFT Live Sketchpad**.

---

## 📋 Table of Contents
1. [Prerequisites & Bill of Materials](#1-prerequisites--bill-of-materials)
2. [Step-by-Step Hardware Wiring](#2-step-by-step-hardware-wiring)
3. [Software & Environment Setup](#3-software--environment-setup)
4. [Uploading Code to the ESP32](#4-uploading-code-to-the-esp32)
5. [Connecting & Operating the Sketchpad](#5-connecting--operating-the-sketchpad)
6. [Web App Interface Controls](#6-web-app-interface-controls)
7. [Troubleshooting & Frequently Asked Questions](#7-troubleshooting--frequently-asked-questions)

---

## 1. Prerequisites & Bill of Materials

Before starting, ensure you have the following hardware components:

| Item | Quantity | Description |
| :--- | :---: | :--- |
| **ESP32 Development Board** | 1 | 30-pin or 38-pin ESP-WROOM-32 board |
| **ST7735 1.8" SPI TFT Display** | 1 | 128x160 pixels RGB display |
| **Jumper Wires** | 8 | Female-to-Male or Male-to-Male |
| **Breadboard** | 1 | Standard breadboard for circuit prototyping |
| **Micro-USB / USB-C Cable** | 1 | Data cable for programming and power |
| **Wi-Fi Router / Hotspot** | 1 | 2.4 GHz Wi-Fi network |

---

## 2. Step-by-Step Hardware Wiring

Connect the ST7735 TFT display to your ESP32 board following the pin mapping below:

```
+-------------------+           +-------------------+
|  ST7735 TFT 1.8"  |           |   ESP32 Dev Board |
|                   |           |                   |
|   VCC  -----------|----------->  3.3V / 5V        |
|   GND  -----------|----------->  GND              |
|   CS   -----------|----------->  GPIO 5           |
|   RESET ----------|----------->  GPIO 4           |
|   A0/DC ----------|----------->  GPIO 2           |
|   SDA  -----------|----------->  GPIO 23 (MOSI)   |
|   SCK  -----------|----------->  GPIO 18 (SCLK)   |
|   LED  -----------|----------->  3.3V             |
+-------------------+           +-------------------+
```

> ⚠️ **Note**: Make sure your power supply provides steady 3.3V or 5V to the display `VCC`. Connecting `LED` (backlight) to `3.3V` lights up the screen display.

---

## 3. Software & Environment Setup

### A. Install Arduino IDE
Download and install [Arduino IDE 2.x](https://www.arduino.cc/en/software).

### B. Add ESP32 Board Support
1. Open Arduino IDE -> Go to **File > Preferences** (or `Cmd + ,` on macOS).
2. Paste the following URL into **Additional Boards Manager URLs**:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to **Tools > Board > Boards Manager...**, search for `esp32` by Espressif, and click **Install**.

### C. Install Required Libraries
Open **Tools > Manage Libraries...** (`Cmd + Shift + I`) and install:
1. `Adafruit GFX Library`
2. `Adafruit ST7735 and ST7789 Library`
3. `WebSockets` (by Markus Sattler)

---

## 4. Uploading Code to the ESP32

1. Open [`display.ino`](file:///Users/bharathkumara/Desktop/display/display.ino) in Arduino IDE.
2. Edit lines 22–23 with your Wi-Fi credentials:
   ```cpp
   const char* ssid     = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
3. Connect your ESP32 to your computer using a USB cable.
4. Select your board under **Tools > Board > ESP32 Arduino > ESP32 Dev Module**.
5. Select your COM / Serial port under **Tools > Port**.
6. Click the **Upload** button (`Cmd + U` / `Ctrl + U`).

---

## 5. Connecting & Operating the Sketchpad

1. Open the **Serial Monitor** (**Tools > Serial Monitor**) and set the baud rate to `115200`.
2. Upon startup, the ESP32 connects to Wi-Fi and prints the assigned IP address:
   ```text
   ========================================================
   >>> WI-FI CONNECTED SUCCESSFULLY! <<<
   ========================================================
   STEP 1: Make sure your phone is connected to this Wi-Fi:
           Network: YOUR_WIFI_NAME
   --------------------------------------------------------
   STEP 2: Open Chrome or Safari on your phone.
   --------------------------------------------------------
   STEP 3: Go to this link to start drawing:
           http://192.168.1.50
   ========================================================
   ```
3. The TFT display will also show `"Ready to Draw!"` along with the web URL.
4. On your mobile phone, tablet, or PC (connected to the same Wi-Fi network), open your web browser and enter the IP address (e.g. `http://192.168.1.50`).

---

## 6. Web App Interface Controls

| Control Element | Icon / UI Component | Action / Function |
| :--- | :--- | :--- |
| **Canvas Screen** | Black Box (128x160 scaled) | Touch/drag with finger or mouse to draw lines in real time. |
| **Color Palette** | Swatch circles | Select from 7 vibrant preset colors (Cyan, Green, Yellow, Orange, Pink, Purple, White). |
| **Width Slider** | Horizontal Slider | Adjust brush stroke thickness between `1px` and `8px`. |
| **Undo Button** | `Undo` | Removes the last drawn stroke from canvas & resyncs the TFT display. |
| **Redo Button** | `Redo` | Re-applies the previously undone stroke. |
| **Clear Button** | `Clear` (Red) | Clears the entire canvas on both phone screen and TFT display. |

---

## 7. Troubleshooting & Frequently Asked Questions

### Q1: The screen stays white or blank after powering on.
- Check the `LED` pin connection (must be connected to `3.3V`).
- Verify hardware SPI connections (`MOSI` on `GPIO 23`, `SCLK` on `GPIO 18`).
- Check if your board requires `INITR_18REDTAB` instead of `INITR_BLACKTAB` in `setup()`:
  ```cpp
  tft.initR(INITR_18REDTAB);
  ```

### Q2: Phone browser says "Disconnected from ESP32".
- Ensure your phone and ESP32 are connected to the exact same 2.4GHz Wi-Fi network.
- Verify WebSocket port `81` is not blocked on your router.
- Check Serial Monitor logs for incoming connection notifications.

### Q3: Drawings appear distorted or colors are inverted.
- Change screen rotation in `setup()`: `tft.setRotation(0);` (or `1`, `2`, `3` for landscape orientation).
- Use 16-bit RGB565 color values in the HTML palette definitions.
