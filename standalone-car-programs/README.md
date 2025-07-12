# 🚗 Standalone Car Control Programs

This folder contains the foundational programs used to get the ESP32-based robotic car up and running using the **Dabble app** as a Bluetooth gamepad controller.

## 📁 Overview

These programs mark the **first stage of development** for the car's movement logic. They implement direct control via the Dabble Gamepad module and handle motor movement using PWM signals.

| File | Description |
|------|-------------|
| `car_digital.ino` | Implements basic directional control using digital button presses (Up, Down, Left, Right). |
| `car_joystick_map.ino` | Enables smooth, analog-like movement using joystick axes from the Dabble app for a more realistic driving experience. |

---

## 🕹️ Control Schemes

### 🔹 `car_digital.ino`
- **Input**: Dabble Gamepad arrow buttons.
- **Behavior**:
  - Forward, backward, left, and right movement.
  - Basic collision avoidance using ultrasonic distance sensor.
  - Simple ON/OFF directional logic.

### 🔸 `car_joystick_map.ino`
- **Input**: Dabble Gamepad joystick.
- **Behavior**:
  - Proportional speed and turning based on joystick axis values.
  - Joystick values are **mapped to motor PWM values** to allow smoother, more intuitive control.
  - Improved collision avoidance: stops only if forward movement is detected toward an obstacle.

---

## 🧠 Features

- ✅ PWM-based motor speed control.
- ✅ Ultrasonic collision detection (HC-SR04 or compatible).
- ✅ Bluetooth-based wireless control using Dabble.
- ✅ Joystick mapping for realistic movement (in `car_joystick_map.ino`).

---

## 🗂️ Folder Purpose

This folder serves as the **base layer** for later V2V (Vehicle-to-Vehicle) communication experiments. The focus here was purely on getting the ESP32 car running and controlled reliably using manual inputs.

> These standalone programs were developed before introducing ESP-NOW communication and are ideal for testing motor behavior and tuning movement responsiveness.

---
