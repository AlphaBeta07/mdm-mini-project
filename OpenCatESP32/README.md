# OpenCatESP32 — Quadruped Robot Firmware

A production-quality, modular, OpenCat-inspired quadruped robot firmware
built from scratch for the **ESP32 DevKit V1 + PCA9685 + 8× SG90/MG90S servos**,
controlled via Bluetooth Classic.

## Hardware Requirements

| Component        | Model                    | Notes                         |
|-----------------|--------------------------|-------------------------------|
| Controller      | ESP32 DevKit V1          | ESP-WROOM-32                  |
| Servo Driver    | PCA9685                  | 16-channel, I2C, addr 0x40    |
| Servos          | 8× SG90 or MG90S        | Channels 0–7                  |
| Servo Power     | External 5V supply       | ≥3A recommended               |
| Communication   | Built-in Bluetooth       | Classic SPP                   |

## Wiring

```
ESP32 GPIO21 (SDA) ──→ PCA9685 SDA
ESP32 GPIO22 (SCL) ──→ PCA9685 SCL
ESP32 GND          ──→ PCA9685 GND ──→ Servo Power GND
ESP32 3.3V         ──→ PCA9685 VCC
Servo Power 5V     ──→ PCA9685 V+
```

## Servo Mapping

| PCA9685 Channel | Joint             |
|-----------------|-------------------|
| CH0             | Front Left Hip    |
| CH1             | Front Left Knee   |
| CH2             | Front Right Hip   |
| CH3             | Front Right Knee  |
| CH4             | Rear Left Hip     |
| CH5             | Rear Left Knee    |
| CH6             | Rear Right Hip    |
| CH7             | Rear Right Knee   |

## Bluetooth Commands

| Command | Action         |
|---------|----------------|
| `A`     | Stand          |
| `M`     | Sit            |
| `D`     | Lay            |
| `C`     | Sleep          |
| `F`     | Walk Forward   |
| `B`     | Walk Backward  |
| `L`     | Turn Left      |
| `R`     | Turn Right     |
| `S`     | Stop           |

## Setup

### 1. Install Arduino IDE and ESP32 Board Support

Add this URL to Arduino IDE Board Manager:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
Install "ESP32 by Espressif Systems".

### 2. Install Required Libraries

Via Arduino Library Manager:
- **Adafruit PWM Servo Driver Library** (by Adafruit)

Built-in (no install needed):
- Wire
- BluetoothSerial
- Preferences

### 3. Board Settings

| Setting         | Value                    |
|-----------------|--------------------------|
| Board           | ESP32 Dev Module         |
| Upload Speed    | 921600                   |
| CPU Frequency   | 240MHz                   |
| Flash Frequency | 80MHz                    |
| Flash Mode      | QIO                      |
| Flash Size      | 4MB                      |
| Partition       | Default 4MB with spiffs  |

### 4. Compile and Upload

Open `OpenCatESP32.ino` in Arduino IDE and upload to your ESP32.

## Serial Commands

After uploading, open Serial Monitor (115200 baud):

| Key | Action                        |
|-----|-------------------------------|
| `c` | Enter calibration mode        |
| `i` | Print system info             |
| `t` | Test: center all servos at 90°|
| `p` | Print calibration offsets     |
| `k` | Print registered skills       |

You can also type Bluetooth command characters (A/M/D/F/B/L/R/S) via Serial for testing without Bluetooth.

## Calibration

1. Power on and open Serial Monitor
2. Press `c` to enter calibration mode
3. All servos center at 90°
4. Select a channel (0–7)
5. Press `+`/`-` to adjust offset ±1°
6. Press `s` to save offsets to flash
7. Press `x` to exit calibration

## Architecture

```
Bluetooth → Command Parser → Robot State Machine → Motion Manager
                                                        ↓
                                            ┌───────────┴───────────┐
                                            │                       │
                                      MotionPlayer            GaitEngine
                                      (static poses)       (locomotion)
                                            │                       │
                                            └───────────┬───────────┘
                                                        ↓
                                              Interpolation Engine
                                                        ↓
                                                  Servo Driver
                                                        ↓
                                                    PCA9685
                                                        ↓
                                                     Servos
```

## License

Open source. See individual file headers for details.
