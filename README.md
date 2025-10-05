# DMD32-v3

A library for controlling those cheap Chinese P10 **single color / monochrome** LED matrix display panels (32x16 and larger, tiled) with the ESP32.

## History

- **[DMD2](https://github.com/freetronics/DMD)** - Original library for Arduino boards - doesn't work with ESP32
- **[DMD32](https://github.com/Qudor-Engineer/DMD32)** - Forked for ESP32 with Arduino Core v2.x - can't be used with current Espressif32 platform
- **[DMD32-v3](https://github.com/alessandroamella/DMD32-v3)** (this version) - Updated for **ESP32 Arduino Core v3.0+**, which introduced significant breaking changes

## Key Changes in This Version

- **Compatibility with Arduino Core v3.0+:** Resolves compilation errors caused by the new Peripheral Manager and updated Timer APIs.
- **Fully Configurable Pins:** The constructor now accepts pin numbers, so you are no longer locked to a fixed hardware layout! (Although using the defaults is still fine, I haven't tried other pin combinations).
- **SPI Performance Improvements:** SPI communication is now more efficient.

## Setup & Installation

### PlatformIO (Recommended)

PlatformIO's official ESP32 platform does not yet support Arduino Core v3.0. You must use the community `pioarduino` fork!

1.  **Create a `platformio.ini` file** in your project root with the following content:

    ```ini
    [env:your_board]
    ; Use the pioarduino fork to get Arduino Core v3.0+
    platform = https://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip ; Optional: pin to a specific version

    board = esp32doit-devkit-v1 ; Change to your board
    framework = arduino
    monitor_speed = 115200 ; Optional: set your serial monitor speed
    ```

2.  **Add this library:** Place the `DMD32-v3` library folder into the `lib/` directory of your PlatformIO project.

### Arduino IDE

1.  **Install ESP32 Core v3.0+:** Make sure you have installed version 3.0.0 or newer of the "esp32" boards package from the Arduino IDE's Boards Manager. Unless you haven't manually downgraded it, it should be already set correctly.
2.  **Install this library:** Download this repository as a ZIP file. In the Arduino IDE, go to `Sketch > Include Library > Add .ZIP Library...` and select the downloaded file.

## Wiring

Connect the DMD panel to your ESP32. The pins listed here are the defaults, but they can now be changed in the code.

| DMD Panel Pin | ESP32 Pin (Default) | Description                           |
| :-----------: | :-----------------: | :------------------------------------ |
|       A       |       GPIO 19       | Row Address A                         |
|       B       |       GPIO 21       | Row Address B                         |
|     SCLK      |       GPIO 2        | Latch Clock                           |
|      nOE      |       GPIO 22       | Output Enable (Active Low)            |
|      CLK      |       GPIO 18       | **SPI Clock (SCK)**                   |
|       R       |       GPIO 23       | **SPI Data (MOSI)**                   |
|      GND      |         GND         | Ground                                |
|      VCC      |         5V          | **Power (Requires external supply!)** |

Here's a simple connection diagram, made by the [original DMD32 author](https://github.com/Qudor-Engineer):

![DMD32 Connection Diagram](connection.png)

**WARNING:** A single 32x16 DMD panel can draw significant current. **DO NOT power the panel from your ESP32's 5V pin.** Use a separate, stable 5V power supply (2A or more is recommended).

## Basic Usage

The constructor is now flexible. You can define custom pins or rely on the defaults.

```cpp
#include <Arduino.h>
#include <DMD32.h>
#include "fonts/SystemFont5x7.h"

// --- Define how many displays you chained ---
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1

// --- Define your pins (optional, but good practice) ---
#define PIN_A      19
#define PIN_B      21
#define PIN_SCLK   2
#define PIN_nOE    22
#define PIN_CLK    18
#define PIN_MOSI   23

// --- Initialize the display ---
// You can pass your custom pins to the constructor.
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN, PIN_A, PIN_B, PIN_SCLK, PIN_nOE, PIN_CLK, PIN_MOSI);

// Or use the defaults:
// DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

// Timer for display scanning ISR
hw_timer_t *timer = NULL;

void IRAM_ATTR triggerScan() {
  dmd.scanDisplayBySPI();
}

void setup() {
  // Configure the hardware timer for a flicker-free refresh
  timer = timerBegin(40000); // 40kHz frequency
  timerAttachInterrupt(timer, &triggerScan);
  timerAlarm(timer, 12, true, 0); // Set alarm for every 300us, unlimited reloads

  // Initialize DMD
  dmd.clearScreen(true);
  dmd.selectFont(SystemFont5x7);
}

void loop() {
  dmd.clearScreen(true);
  dmd.drawString(2, 4, "Hello", 5, GRAPHICS_NORMAL);
  delay(2000);
}
```

**For more examples, check out the [`examples/`](examples/) folder** in this repository.

I also made a demo project, that fetches train departures and weather data from the internet, and displays them on 2 horizontally adjacent 32x16 DMD panels. You can find it here: [arduino-train-api-client](https://github.com/alessandroamella/arduino-train-api-client/).
