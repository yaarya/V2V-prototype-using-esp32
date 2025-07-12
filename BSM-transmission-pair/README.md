# 📤 BSM Transmission Pair

This folder contains a two-device ESP32 system where the robotic car sends a **full BSM (Basic Safety Message)** struct over ESP-NOW. It builds upon the previous step (`distance-transmission-pair`) by transmitting a richer data packet that simulates real-world V2V communication messages.

## 📁 Overview

| File | Description |
|------|-------------|
| `car_BSM.ino` | Runs on the ESP32 car. Collects driving state data (distance, speed, acceleration, timestamp) and transmits a complete BSM struct via ESP-NOW. |
| `BSM_receiver.ino` | Runs on a receiving ESP32. Parses and displays full BSM data in the Serial Monitor and triggers LED warning if distance falls below a threshold. |

---

## 🔄 Communication Workflow

### 🚗 `car_BSM.ino` (Transmitting Car)
- Equipped with:
  - Ultrasonic sensor for obstacle detection.
  - Dabble Gamepad for joystick-based driving.
  - Dual motors with PWM speed control.
- Sends a `BSM` (Basic Safety Message) struct every 100 ms, which includes:
  - Type (always `"BSM"`)
  - Distance to obstacle
  - Current joystick Y-axis speed value
  - Acceleration (computed from change in speed over time)
  - Timestamp in milliseconds
- Real-time **acceleration is calculated** based on the change in joystick input.
- Transmission is non-blocking and coexists with smooth motion and collision avoidance.

### 📥 `BSM_receiver.ino` (Stationary Receiver)
- Connected to Serial Monitor and onboard LED.
- Behavior:
  - Receives and parses full BSM messages via ESP-NOW.
  - Logs all fields to the Serial Monitor for analysis.
  - Blinks onboard LED when `distance < 30 cm` to signal a nearby vehicle.
  - Turns off LED if no data is received for over 1 second (timeout safety mechanism).

---

## 🔍 Key Difference from Previous Step

While the `distance-transmission-pair` setup only transmits a single float (distance), this program sends a **complete BSM message** containing multiple parameters that describe the vehicle’s current state. This is a more realistic simulation of actual V2V communication used in connected car systems.

---

## 💡 Features

- ✅ Full BSM struct transmission via ESP-NOW.
- ✅ Real-time acceleration computation using joystick data.
- ✅ Bluetooth-based joystick driving with collision logic.
- ✅ Non-blocking communication loop.
- ✅ LED alert logic based on proximity and message freshness.

---

## 🗂️ Folder Purpose

This setup represents the **fourth development milestone**, introducing structured messaging that mirrors real-world vehicle communication standards. It validates the concept of broadcasting comprehensive vehicle behavior to nearby ESP32 peers.

> This serves as a **foundation for building safety-aware, cooperative V2V systems** using ESP-NOW and sensor fusion.

---
