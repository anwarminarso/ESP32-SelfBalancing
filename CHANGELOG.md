# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Project documentation: `README.md`, `LICENSE` (MIT), `CONTRIBUTING.md`,
  `CODE_OF_CONDUCT.md`, GitHub issue and pull-request templates.

## [1.0.0] - 2019

### Added
- Initial ESP32 self-balancing robot firmware.
- Cascaded PID balancing control (angle + rate) with yaw steering.
- Cooperative task scheduler running alongside the async web server.
- MPU-9250 IMU driver with DCM orientation filter and gyro/accel calibration.
- TB6612FNG dual-motor driver support and quadrature encoder counting.
- Web dashboard (SPIFFS) with live telemetry, PID tuning, calibration and
  joystick remote control.
- REST API and binary WebSocket protocol, protected by HTTP basic auth.
- Persistent configuration in NVS, dual WiFi (STA + AP) mode, and RC failsafe.
