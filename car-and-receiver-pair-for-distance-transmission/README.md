# 📶 Distance Transmission Pair

This folder contains a two-device ESP32 setup used to **transmit real-time distance measurements** over ESP-NOW. It tests how well a moving robotic car (with ultrasonic sensing and Bluetooth control) can broadcast sensor data to a stationary ESP32 that monitors the environment.

## 📁 Overview

| File | Description |
|------|-------------|
| `car_espNOW.ino` | Runs on the robotic car. Measures distance using an ultrasonic sensor and sends this data via ESP-NOW while being joystick-controlled through Dabble. |
| `receiver.ino` | Runs on a stationary ESP32 connected to a PC. Receives distance data and activates an LED warning when the car is too close to an obstacle. |

---

## 🔄 Communication Workflow

### 🚗 `car_espNOW.ino` (Car-side)
- Equipped with:
  - **Ultrasonic sensor** for obstacle detection.
  - **Dabble Gamepad** for Bluetooth joystick control.
  - **Motors** controlled via PWM.
- Logic:
  - Maps joystick input to differential motor speed for driving.
  - Continuously measures distance using ultrasonic sensor.
  - Sends `DistanceData` struct over ESP-NOW every 100 ms.
  - Prevents forward motion if obstacle is too close (within 30 cm).
  - Sample struct format:
    ```cpp
    typedef struct {
      float distance;
    } DistanceData;
    ```

### 🧠 `receiver.ino` (Stationary-side)
- Connected to Serial Monitor (for debugging) and onboard LED.
- Behavior:
  - Listens for incoming `DistanceData` via ESP-NOW.
  - Logs the current distance in cm to the Serial Monitor.
  - If the reported distance is **below 30 cm**, the onboard LED starts blinking.
  - If data is **not received for over 1 second**, LED is turned off for safety.
  
---

## 💡 Features

- ✅ Real-time ESP-NOW distance data transmission.
- ✅ Built-in collision prevention logic on the car.
- ✅ Onboard LED alert on the receiver when obstacle is near.
- ✅ Timeout logic to handle lost data signals.
- ✅ Fully non-blocking control for smooth and responsive operation.

---

## 🗂️ Folder Purpose

This setup was built to **test reliable, real-time sensor broadcasting from a moving ESP32 robot to a monitoring unit**. It validates that:
1. ESP-NOW can be used alongside Bluetooth joystick control.
2. Environmental sensor data can be effectively shared peer-to-peer.
3. Alerts can be triggered remotely based on proximity thresholds.

> This pair served as the **third step in development**, laying the foundation for more advanced V2V communication based on sensor broadcasting.

---
