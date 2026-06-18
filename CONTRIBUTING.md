# Contributing to ESP32 Self-Balancing Robot

Thanks for your interest in improving this project! Contributions of all kinds
are welcome — bug reports, documentation, hardware notes, and code.

## Code of Conduct

By participating in this project you agree to abide by the
[Code of Conduct](CODE_OF_CONDUCT.md). Please be respectful and constructive.

## Ways to Contribute

- **Report bugs** by opening an issue. Include your hardware setup, the
  firmware version/commit, steps to reproduce, and what you expected to happen.
- **Suggest features or improvements** via an issue before starting large work,
  so we can discuss the approach.
- **Improve documentation** — fixes to the README, wiring notes, and inline
  comments are always appreciated.
- **Submit code** through a pull request (see below).

## Development Setup

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) (or PlatformIO)
   with the [ESP32 board package](https://github.com/espressif/arduino-esp32).
2. Install the required libraries: ESPAsyncWebServer, AsyncTCP, ArduinoJson.
3. Copy `SelfBalancing/Secrets.example.h` to `SelfBalancing/Secrets.h` and fill
   in your credentials. Never commit `Secrets.h` — it is git-ignored.
4. Build and flash as described in the [README](README.md#getting-started).

## Pull Request Guidelines

1. **Fork** the repository and create a topic branch from `master`:
   ```bash
   git checkout -b feature/my-change
   ```
2. **Keep changes focused.** One logical change per pull request makes review
   easier.
3. **Match the existing style.** Follow the formatting and naming conventions
   already used in the surrounding code (tabs for indentation, existing module
   structure).
4. **Do not commit secrets or build artifacts.** The `.gitignore` already
   excludes `Secrets.h`, IDE folders and build outputs.
5. **Test on hardware** when possible, and describe what you tested in the PR.
6. **Write a clear PR description**: what changed, why, and any hardware or
   configuration implications.

## Reporting Security Issues

If you find a security issue (for example, in the web/auth layer), please avoid
filing a public issue with exploit details. Open a minimal issue asking a
maintainer to get in touch, or contact the maintainer directly.

## License

By contributing, you agree that your contributions will be licensed under the
[MIT License](LICENSE) that covers this project.
