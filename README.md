# 🚗 V2V Prototype Using ESP32

This repository contains a progressive series of embedded systems programs developed to prototype a **Vehicle-to-Vehicle (V2V) communication system** using **ESP32 microcontrollers**, **ESP-NOW wireless protocol**, **ultrasonic sensors**, and **Bluetooth joystick control** via the **Dabble app**.

The core idea: two ESP32-powered toy cars that can **communicate real-time driving information** to each other and trigger **collision warnings**, all while being manually driven.

---

## 📁 Repository Structure

| Folder | Purpose |
|--------|---------|
| [`standalone-car-programs`](./standalone-car-programs) | Base programs for manual driving via Bluetooth gamepad using Dabble. Introduces motor control and collision avoidance. |
| [`ESP-NOW-test-programs`](./ESP-NOW-test-programs) | Initial experiments with ESP-NOW protocol: BSM testing, joystick transmission, and MAC discovery. |
| [`car-and-receiver-pair-for-distance-transmission`](./car-and-receiver-pair-for-distance-transmission) | Unidirectional sensor broadcasting: one ESP32 sends live distance data to another. |
| [`BSM-transmission-pair`](./BSM-transmission-pair) | Car sends full BSM (Basic Safety Message) structs including speed, distance, acceleration, and timestamp. |
| [`BSM-V2V-pair`](./BSM-V2V-pair) | Final prototype: two cars exchange BSMs in both directions and alert each other on potential collision. |

Each folder contains its own `README.md` for detailed explanation of its code and logic.

---

## 🧰 IDE & Library Setup

To work with this project, set up your development environment as follows:

### 💻 IDE
- **Arduino IDE** (1.8+ or 2.x)
- Boards Manager URL (for ESP32):  
  `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`

### 📦 Required Libraries
Install the following libraries via Arduino Library Manager:

- **DabbleESP32** 

> ⚠️ Make sure to select the correct board:  
> `ESP32 Wrover Module`

---

## 🔌 Hardware Requirements

- 🧠 **2× ESP32 Dev boards**  
- 🛞 **2× L298N motor drivers**  
- 🔋 **2× battery-powered toy car chassis**  
- 🧭 **2× HC-SR04 ultrasonic distance sensors**  
- 🔌 Jumper wires, USB cables  

---

## 🕹️ Dabble Bluetooth Controller Setup

Dabble is a mobile app that enables Bluetooth control of microcontroller-based devices.

### 📲 Steps to Use:
1. Install the **Dabble app** from the Play Store.
2. Each ESP32 sketch must start Dabble with a **unique Bluetooth name**, like this:  
   `Dabble.begin("Car1");`
3. Connect your phone to the ESP32 via the app.
4. Use the Gamepad module (switch to joystick controls) inside the app to control speed and direction.

Joystick values are read over serial and converted to PWM motor speeds inside the sketch.

---

## 🔄 Final V2V System Summary

The final system, located in the [`BSM-V2V-pair`](./BSM-V2V-pair) folder, features:

- 📡 **Bidirectional communication** between two moving cars.
- 🧠 Each car:
  - Measures obstacle distance using an ultrasonic sensor.
  - Sends a full BSM (Basic Safety Message) every 100 ms.
  - Receives the peer's BSM and evaluates for collision risk.
- 🚨 If a car detects its peer is too close:
  - It triggers an onboard LED alert.
  - It prevents forward movement based on local distance too.

Each BSM message includes:
- Type (`"BSM"`)
- Distance to obstacle
- Joystick speed input
- Acceleration (based on joystick delta)
- Timestamp

> The result is a decentralized, cooperative driving demo — a fundamental building block for real-world V2V systems.

---

## 📌 Future Enhancements

- Add support for GPS or real-time localization.
- Expand to mesh networks for multi-vehicle coordination.
- Implement message signing/authentication for security.
- Visualize driving behavior in real-time on a dashboard.

---

## 📄 License

This project is licensed under the [MIT License](./LICENSE).

---

Happy prototyping! 🛠️  
Feel free to fork, extend, or adapt for your own robotics or ITS (Intelligent Transportation Systems) experiments.
