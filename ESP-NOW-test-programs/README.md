# 📡 ESP-NOW Test Programs

This folder contains early experiments with **ESP-NOW**, a low-power, peer-to-peer wireless protocol for ESP32 boards. The goal was to test the reliability of ESP-NOW for communication between two ESP32s and assess its compatibility with Bluetooth (used for Dabble joystick input).

## 📁 Overview

| File | Description |
|------|-------------|
| `A2B.ino` | Tests bidirectional ESP-NOW communication by exchanging randomly generated BSM (Basic Safety Message) data between two ESP32s. |
| `A2B_joystick.ino` | Sends joystick data from one ESP32 (connected to Dabble via Bluetooth) to another ESP32 using ESP-NOW. Demonstrates that ESP-NOW works alongside Bluetooth. |
| `findMAC.ino` | Utility script to print the MAC address of an ESP32 board. Useful for peer registration during ESP-NOW setup. |

---

## 🔄 Communication Tests

### 🔹 `A2B.ino`
- Implements a **BSM struct** that holds:
  - Type (always `"BSM"`)
  - X & Y positions
  - Speed & acceleration
  - Timestamp
- One board generates **randomized BSM data** every 2 seconds and sends it via ESP-NOW.
- The peer board receives and prints this data to the Serial Monitor.
- ✅ Tests **bidirectional ESP-NOW** data transfer.

### 🔸 `A2B_joystick.ino`
- Sends joystick values (`x`, `y`) from one ESP32 (with Dabble Bluetooth enabled) to another using ESP-NOW.
- Demonstrates that **ESP-NOW can coexist with Bluetooth**, which is crucial for combining sensor feedback and remote control.

---

## 🔍 Utility

### 🛠️ `findMAC.ino`
- Prints the ESP32’s unique MAC address to the Serial Monitor.
- ⚙️ Used during setup to correctly configure peer-to-peer communication by setting the correct target MAC address.

---

## 📦 Folder Purpose

This folder acts as a **sandbox for ESP-NOW research and proof-of-concept trials**. The scripts here were written to answer two core questions:

1. Can two ESP32 boards communicate bidirectionally using ESP-NOW?
2. Can ESP-NOW function correctly alongside Dabble's Bluetooth gamepad module?

> Both questions were successfully validated in this folder before integrating ESP-NOW into more complex robotic systems.

---
