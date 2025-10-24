# 🦇 Bat Sonar Listener

**Bat Sonar:Listener** is a C application that captures ultrasonic microphone data, performs lightweight acoustic
analysis, and sends results as JSON to a remote monitor (Android app) over MQTT or AWS.  
The goal: distributed bat sonar detection — your listening device can be anywhere, and your phone visualizes the data
live.

---

## 🚀 Features

- Real-time ultrasonic audio capture via **PortAudio**
- Frequency estimation using **zero-crossing**
- RMS power calculation
- Configurable sample rate and buffer size
- JSON reporting over **MQTT**
- C17 codebase with **CMake** and **GitHub Actions** build workflow

---

## 🧠 Overview

**Architecture:**

```
[Ultrasonic Mic] → [C Listener App] → [MQTT / AWS] → [Android Monitor]
```

1. **Capture**  
   The listener opens a PortAudio stream (default: 256 kHz, 1024-sample buffers).

2. **Analyze**  
   Each buffer is processed to estimate dominant frequency and signal power.

3. **Transmit**  
   The results are packed into JSON and published to MQTT for visualization.

---

## 🧉 Example JSON Output

```json
{
  "timestamp": 1730059200,
  "device_id": 1,
  "sample_rate": 256000,
  "buffer_size": 1024,
  "frequency_hz": 48500.2,
  "power": 0.82,
  "duration_ms": 4.0
}
```

The Android app can subscribe to the same topic and plot frequency and power in real time.

---

## ⚙️ Build Instructions

### Requirements

- CMake ≥ 3.20
- GCC or Clang (C17 support)
- Libraries:
    - `portaudio19-dev`
    - `libpaho-mqtt3c-dev`
    - `libcjson-dev`
    - `libssl-dev`

### Build

```bash
git clone https://github.com/yourname/Bat-Sonar-Listener.git
cd Bat-Sonar-Listener
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Binary: `build/Bat_Sonar_Listener`

---

## 🧾 Configuration

The app reads its connection and parameter settings from a JSON config file or from an embedded default.

Example `config/defaults.json`:

```json
{
  "mqtt_topic": "bat_sonar/data",
  "aws_endpoint": "your-endpoint.amazonaws.com",
  "sample_rate": 256000,
  "buffer_size": 1024
}
```

If unavailable at runtime, built-in defaults are used.

---

## 🧮 Development

### Lint & Format

```bash
make lint
make format
```

Uses `cppcheck`, `clang-tidy`, and `clang-format` for static analysis and code style enforcement.

### GitHub Actions

Each push or pull request:

- Builds the project
- Runs linters
- Uploads the compiled binary as an artifact

---

## 🧮 Technical Notes

- Default sample rate: **256 kHz**
- Default buffer size: **1024 samples (~4 ms)**
- Frequency estimation: zero-crossing
- Power: RMS
- Later versions will add FFT-based mel-spectrograms

---

## 🦇 Roadmap

- FFT and mel-spectrogram support
- Multi-device aggregation
- Cloud dashboards
- Real-time Android frequency heatmaps

---

## 📜 License

MIT License — free to use, modify, and build upon.

---

**Author:** Ian Gabaraev
**Project:** Bat Sonar Listener
