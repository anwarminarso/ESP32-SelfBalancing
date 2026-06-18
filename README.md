# ESP32 Self-Balancing Robot

A two-wheeled self-balancing robot built around an **ESP32**, an **MPU-9250** IMU
and a **TB6612FNG** dual motor driver. The robot keeps itself upright using a
cascaded PID controller and can be tuned, calibrated and driven in real time from
a built-in web interface over WiFi.

> Inverted-pendulum control running on FreeRTOS, with a responsive web dashboard
> for live telemetry, PID tuning, sensor calibration and remote control.

## Table of Contents

- [Features](#features)
- [Hardware](#hardware)
- [Wiring / Pin Map](#wiring--pin-map)
- [Software Architecture](#software-architecture)
- [Getting Started](#getting-started)
- [Configuration](#configuration)
- [Web Interface & API](#web-interface--api)
- [Control Theory](#control-theory)
- [Project Layout](#project-layout)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

## Features

- **Self-balancing control** using a cascaded PID loop (angle → angular-rate)
  for pitch, plus a rate loop for yaw steering.
- **Cooperative scheduler** running the control loop at a fixed rate alongside
  the async web server, with a spinlock guarding shared state between the
  control task and the network task.
- **Built-in web dashboard** served from SPIFFS for live telemetry, PID tuning,
  IMU calibration and joystick-style remote control.
- **REST API + WebSocket** interface (HTTP basic-auth protected) for both
  human and programmatic access.
- **Persistent configuration** stored in NVS (ESP32 `Preferences`) so PID gains
  and calibration survive reboots.
- **Dual WiFi mode**: connects to your home network (STA) and simultaneously
  runs its own access point (AP) so the robot stays controllable even when the
  home network is unavailable.
- **Failsafe**: motors are cut if remote-control commands stop arriving.
- **Quadrature motor encoders** for wheel-position feedback.

## Hardware

| Component | Description |
|-----------|-------------|
| Microcontroller | ESP32 development board |
| IMU | MPU-9250 (9-DoF accelerometer / gyro / magnetometer), optional MS5611 barometer |
| Motor driver | TB6612FNG dual H-bridge |
| Motors | 2 × JGB37-520 geared DC motors with quadrature encoders |
| Power | LiPo battery + step-down regulator |
| Chassis | Aluminum-alloy two-wheel self-balancing frame |

Datasheets and reference photos are available in the [`Docs/`](Docs) folder,
and a 3D-printable PCB bracket is provided in [`3D/`](3D).

## Wiring / Pin Map

Pin assignments are defined in the firmware source. Default values:

### Motor driver (TB6612FNG)

| Signal | ESP32 GPIO | Source |
|--------|-----------|--------|
| Motor 1 AIN1 | 14 | `Stabilizer.cpp` |
| Motor 1 AIN2 | 12 | `Stabilizer.cpp` |
| Motor 1 PWM  | 32 | `Stabilizer.cpp` |
| Motor 2 BIN1 | 26 | `Stabilizer.cpp` |
| Motor 2 BIN2 | 25 | `Stabilizer.cpp` |
| Motor 2 PWM  | 33 | `Stabilizer.cpp` |
| STBY (standby) | 27 | `Stabilizer.cpp` |

### Encoders

| Signal | ESP32 GPIO | Source |
|--------|-----------|--------|
| Motor 1 encoder A (yellow) | 13 | `MotorEncoder.cpp` |
| Motor 1 encoder B (green)  | 15 | `MotorEncoder.cpp` |
| Motor 2 encoder A (yellow) | 4  | `MotorEncoder.cpp` |
| Motor 2 encoder B (green)  | 5  | `MotorEncoder.cpp` |

### IMU (I2C)

| Signal | ESP32 GPIO |
|--------|-----------|
| SDA | 21 (default `Wire`) |
| SCL | 22 (default `Wire`) |

A breadboard wiring reference is included at
[`Docs/Photo/self balancing_bb.png`](Docs/Photo).

## Software Architecture

The firmware is organized into focused modules:

| Module | Responsibility |
|--------|----------------|
| `main.cpp` | Setup and registration of scheduled control loops |
| `Task.cpp/.h` | Lightweight cooperative scheduler (circular task queue) |
| `Sensor.cpp/.h` | IMU read, calibration application and orientation update |
| `SimpleMPU9250` | Minimal MPU-9250 driver |
| `DCM` | Direction-Cosine-Matrix orientation filter (yaw/pitch/roll) |
| `Stabilizer.cpp/.h` | Cascaded PID control loop and motor mixing |
| `ESP32_TB6612` | Motor-driver abstraction |
| `MotorEncoder.cpp/.h` | Quadrature encoder counting via interrupts |
| `Server.cpp/.h` | Async web server, REST API and WebSocket endpoint |
| `Communication.cpp/.h` | Binary WebSocket message protocol |
| `Configuration.cpp/.h` | Persisting/loading config from NVS |
| `Secrets.h` | WiFi and HTTP credentials (git-ignored) |

The control loop and the AsyncTCP/web-server task run concurrently. Shared
state (PID gains, RC command, enable flag, failsafe counter) is guarded by a
`portMUX_TYPE` spinlock (`dataMux`) so the control loop never reads a
half-updated value while the web task is writing.

## Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) (or PlatformIO) with the
  [ESP32 board support package](https://github.com/espressif/arduino-esp32).
- The following Arduino libraries:
  - [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
  - [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
  - [ArduinoJson](https://arduinojson.org/)
- A SPIFFS upload tool to flash the `data/` web assets
  (e.g. the [Arduino ESP32 filesystem uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin)).

### Build & Flash

1. **Clone the repository**
   ```bash
   git clone https://github.com/anwarminarso/ESP32-SelfBalancing.git
   cd ESP32-SelfBalancing
   ```

2. **Create your credentials file**

   Copy the template and fill in your own values:
   ```bash
   cp SelfBalancing/Secrets.example.h SelfBalancing/Secrets.h
   ```
   `Secrets.h` is git-ignored, so your credentials stay local.

3. **Open the project** (`SelfBalancing/SelfBalancing.ino`) in the Arduino IDE
   and select your ESP32 board and port.

4. **Upload the firmware** with the standard upload button.

5. **Upload the web assets** in `SelfBalancing/data/` to SPIFFS using the
   filesystem uploader.

6. **Open the Serial Monitor** at `115200` baud to see the assigned IP address.

## Configuration

WiFi and authentication credentials are defined in `SelfBalancing/Secrets.h`
(copied from `Secrets.example.h`):

```c
// WiFi station (your existing network)
#define WIFI_STA_SSID       "YOUR_WIFI_SSID"
#define WIFI_STA_PASSWORD   "YOUR_WIFI_PASSWORD"

// Soft-AP (the robot's own hotspot)
#define WIFI_AP_SSID        "Self Balancing Robot"
#define WIFI_AP_PASSWORD    "CHANGE_ME_AP_PASSWORD"

// HTTP / WebSocket basic-auth credentials
#define HTTP_AUTH_USER      "admin"
#define HTTP_AUTH_PASS      "CHANGE_ME_HTTP_PASSWORD"
```

> **Security note:** Always change the default AP and HTTP passwords before
> deploying. The web UI, REST API and control WebSocket are all protected by
> HTTP basic auth.

PID gains and IMU calibration values are persisted to NVS and can be edited live
from the web UI. Default values are defined in `Configuration.cpp`
(`resetConfig`).

## Web Interface & API

Once connected, browse to the robot's IP address (STA) or to its access point.

### REST API

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET  | `/api/info` | Chip / firmware / heap info |
| GET  | `/api/ypr` | Yaw, pitch, roll |
| GET  | `/api/gyro` | Gyroscope readings |
| GET  | `/api/acc` | Accelerometer readings |
| GET  | `/api/motor` | Encoder counters |
| GET  | `/api/accMinMax` | Accelerometer calibration range |
| GET  | `/api/cal` | Full calibration + PID snapshot |
| POST | `/api/savePID` | Save PID gains (param `pid`, JSON) |
| POST | `/api/saveGyroOffset` | Save gyro offset (param `gyroOffset`, JSON) |
| POST | `/api/saveAccMinMax` | Save accel min/max (param `accMinMax`, JSON) |
| POST | `/api/toggleStabilizer` | Enable/disable balancing |
| POST | `/api/reloadCfg` | Reload config from NVS |
| POST | `/api/resetCfg` | Reset config to defaults and restart |
| POST | `/api/restart` | Restart the ESP32 |

All `POST` endpoints require HTTP basic auth.

### WebSocket (`/ws`)

A compact binary protocol is used for high-rate telemetry and control. Each
message starts with a one-byte message code (see `Communication.h`). Highlights:

| Code | Name | Direction | Purpose |
|------|------|-----------|---------|
| 11 | `MSG_IMU_DATA_ALL` | get | Full IMU + motor snapshot |
| 14 | `MSG_IMU_DATA_YPR` | get | Yaw/pitch/roll |
| 34 | `MSG_CAL_PID` | get | Current PID gains |
| 44 | `MSG_SET_CAL_PID` | set | Update PID gains |
| 52 | `MSG_SET_STABILZE_STATE` | set | Enable/disable balancing |
| 61–64 | `MSG_ACC_CAL_*` | set/get | Accelerometer calibration capture |
| 101 | `MSG_SET_RC` | set | Remote-control command (forward/back, left/right) |
| 255 | `MSG_RESET` | set | Restart the ESP32 |

## Control Theory

The robot is an inverted pendulum. The balancing controller is **cascaded**:

1. **Angle (outer) loop** — compares the desired tilt (from the RC forward/back
   command) against the measured pitch from the DCM filter and produces a target
   angular rate.
2. **Rate (inner) loop** — compares that target rate against the measured gyro
   rate and produces the motor command.

A separate yaw rate loop handles left/right steering. The two axis outputs are
combined in a mixer (`PIDMIX`) and written to the two motors, clamped to the
PWM range. Derivative terms are smoothed with a 3-sample moving average and
integral terms are clamped to prevent wind-up.

## Project Layout

```
ESP32-SelfBalancing/
├── 3D/                     # 3D-printable PCB bracket (STL, gcode, Inventor)
├── Docs/                   # Datasheets, photos and component specs
├── SelfBalancing/          # Firmware (Arduino/ESP32)
│   ├── data/               # Web UI assets (uploaded to SPIFFS)
│   ├── Secrets.example.h   # Credentials template
│   └── *.cpp / *.h         # Firmware modules
├── LICENSE
└── README.md
```

## Troubleshooting

- **Robot won't balance / oscillates** — re-tune PID gains from the web UI,
  starting from the defaults in `Configuration.cpp`.
- **Drifting orientation** — re-run the gyro and accelerometer calibration from
  the web UI.
- **Can't reach the web UI** — check the Serial Monitor for the assigned IP. If
  the home network is unavailable, connect to the robot's own access point
  (`WIFI_AP_SSID`).
- **Motors don't respond** — confirm the stabilizer is enabled and verify the
  TB6612FNG wiring and STBY pin.
- **Build errors about missing headers** — make sure ESPAsyncWebServer, AsyncTCP
  and ArduinoJson are installed and the ESP32 board package is selected.

## Contributing

Contributions are welcome. Please read [CONTRIBUTING.md](CONTRIBUTING.md) for
guidelines on reporting issues and submitting pull requests.

## License

This project is licensed under the [MIT License](LICENSE).

© 2019 Anwar Minarso
