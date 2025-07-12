# 🔄 BSM V2V Pair

This folder contains the final, most advanced stage of the project: a **bidirectional Vehicle-to-Vehicle (V2V)** communication system using ESP-NOW and Bluetooth joystick control. Both ESP32-powered cars exchange real-time BSM (Basic Safety Message) data while driving independently and react if either detects a nearby obstacle.

## 📁 Overview

| File | Description |
|------|-------------|
| `car1.ino` | Program for Car 1. Sends its own BSM, receives BSM from Car 2, and reacts with LED alerts if necessary. |
| `car2.ino` | Identical logic to `car1.ino`, but with unique Bluetooth name and peer MAC address. Meant to be flashed on the second car. |

> ⚠️ Both files contain the **same core logic**, differing only in Bluetooth name and the target peer MAC address. Each car must be flashed with a unique combination to establish proper communication.

---

## 🚘 Communication & Control Logic

### 🛜 ESP-NOW
- Each car sends out a full `BSM` struct every 100ms, containing:
  - Type (always `"BSM"`)
  - Distance to obstacle (from ultrasonic sensor)
  - Speed (joystick Y-axis input)
  - Acceleration (computed from change in joystick speed)
  - Timestamp

- At the same time, each car **receives the peer's BSM** and updates internal state based on that data.

### 🕹️ Driving & Obstacle Detection
- Cars are driven manually via Dabble Bluetooth joystick.
- Speed is translated into motor PWM values with proportional control.
- Local ultrasonic sensor is used to detect frontal obstacles.
- If the **local** or the **peer's** BSM indicates an obstacle within 30 cm, the car:
  - **Stops forward motion** (locally),
  - **Blinks onboard LED** to warn of close contact (based on peer data).

### 🧠 Collision-Aware Cooperation
- Both cars act **independently** but share data **symmetrically**.
- If **either** car detects danger, it:
  - Sends a warning via BSM,
  - The peer receives it and can trigger local alert systems (LED, logs),
  - This simulates real-world V2V safety systems where vehicles share status to enhance mutual awareness.

---

## 💡 Features

- ✅ Fully bidirectional BSM exchange using ESP-NOW.
- ✅ Joystick control using Dabble via Bluetooth.
- ✅ Real-time distance sensing and transmission.
- ✅ Acceleration computation for each vehicle.
- ✅ LED blinking alert system based on received peer distance.
- ✅ Data timeout logic ensures LED turns off if messages stop.

---

## 🗂️ Folder Purpose

This folder contains the **final working prototype** of a basic V2V safety communication system. It merges all prior developments—manual driving, sensor feedback, ESP-NOW messaging, and structured BSM transmission—into a robust, bidirectional communication loop between two autonomous vehicles.

> With both cars running this firmware (with unique IDs), they become fully aware of each other’s position and driving behavior, enabling cooperative collision avoidance.

---
