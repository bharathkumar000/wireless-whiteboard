# Hardware & Wiring Specifications ⚡

This guide provides technical specifications, detailed pin mappings, power considerations, and troubleshooting steps for the **Wireless Whiteboard (ESP32 + ST7735 TFT)** project.

---

## 🛠 Component List

1. **ESP32 Development Board** (ESP-WROOM-32 / ESP32 Dev Module)
2. **ST7735 1.8" Color SPI TFT Display** (128x160 resolution, Red or Black tab)
3. **Breadboard & Jumper Wires** (Male-to-Female / Male-to-Male)
4. **Micro-USB / USB-C Cable** (Data capable)
5. **2.4 GHz Wi-Fi Router / Hotspot**

---

## 📌 Complete Pin Wiring Diagram

| ESP32 Pin Name | ESP32 GPIO Pin | ST7735 Display Module Pin | Signal Function |
| :--- | :--- | :--- | :--- |
| `3V3` / `5V` | Power Out | `VCC` | Module Power (3.3V or 5V depending on module regulator) |
| `GND` | Ground | `GND` | Ground Reference |
| `GPIO 5` | Pin 5 | `CS` / `SS` | SPI Chip Select |
| `GPIO 4` | Pin 4 | `RST` / `RESET` | Hardware Display Reset |
| `GPIO 2` | Pin 2 | `DC` / `A0` | Data / Command Selection |
| `GPIO 23` | Pin 23 | `SDA` / `MOSI` | SPI Master Out Slave In |
| `GPIO 18` | Pin 18 | `SCLK` / `SCK` | Hardware SPI Clock |
| `3V3` | Power Out | `LED` / `BLK` | Backlight LED Power |

---

## 🔌 SPI Bus Overview

The ESP32 uses **VSPI** hardware SPI pins by default for optimal rendering performance:

- **SPI Clock Speed**: Default hardware SPI frequency (~27 MHz - 40 MHz).
- **Display Resolution**: $128 \times 160$ pixels.
- **Color Depth**: 16-bit RGB565 color format.

---

## 🔍 Display Tab & Offset Troubleshooting

ST7735 TFT modules come in several hardware revisions, distinguished by the color of the protective plastic tab on the screen:

### 1. Black Tab (`INITR_BLACKTAB`) — Default in code
```cpp
tft.initR(INITR_BLACKTAB);
```
Used for most standard 1.8" 128x160 SPI modules.

### 2. Red Tab (`INITR_18REDTAB`)
If your display has color offset issues or a shifted 2-pixel margin along the edges:
```cpp
tft.initR(INITR_18REDTAB);
```

### 3. Green Tab (`INITR_GREENTAB`)
For 1.44" or specialized 128x128 green tab displays:
```cpp
tft.initR(INITR_GREENTAB);
```

---

## ⚠️ Common Hardware Checks

1. **Blank / White Screen**:
   - Verify `VCC` and `GND` connections.
   - Ensure the `LED` / `BLK` backlight pin is connected to `3.3V`.
   - Double-check `MOSI` (`GPIO 23`) and `SCLK` (`GPIO 18`) wiring.

2. **Garbled / Corrupted Display**:
   - Check jumper wire length (keep SPI leads under 20cm).
   - Ensure a common ground between the ESP32 and external power supply if used.
