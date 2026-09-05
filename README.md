# 🚗 Smart Car Simulation System

[![Microprocessor Project](https://img.shields.io/badge/Project-Microprocessors%20Course-blue.svg)](https://github.com/Sobhankhedry/CAR-Arduino)
[![Board](https://img.shields.io/badge/Board-Arduino%20Mega%202560-green.svg)](https://www.arduino.cc/)
[![Language](https://img.shields.io/badge/Language-C%2B%2B%2FArduino-orange.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-MIT-brightgreen.svg)](LICENSE)

An advanced, interactive hardware and software smart car simulation system built on the **Arduino Mega 2560** platform[cite: 1]. This project emulates essential real-world automotive features, including ignition lifecycle, gear shifting, cruise control, ultrasonic parking assistance, smart central locking, ambient sensing, and multi-tier safety alert systems[cite: 1].

---

## 📌 Project Overview
- **Course:** Microprocessors & Embedded Systems[cite: 1]
- **Instructor:** Dr. Sajjad Nezhadhassan[cite: 1]
- **Author:** Sobhan Khedry[cite: 1]
- **Repository:** [Sobhankhedry/CAR-Arduino](https://github.com/Sobhankhedry/CAR-Arduino)

---

## 🛠 Key Features

### 1. Engine Start/Stop & Ignition Lifecycle
- **Interactive Warm-up Sequence:** Press the start button to initiate engine heating with a countdown displayed on the 7-segment display[cite: 1].
- **Custom Welcome Screen:** Plays a welcome chime via buzzer while rendering custom animations on the LED matrix and welcoming the user on the primary LCD[cite: 1].
- **Safe Shutdown Mode:** Implements a controlled "Stay Safe" engine shutdown process[cite: 1].

### 2. Gearbox & Driving Dynamics
- **Multi-Mode Transmission:** Toggle between **Drive (D)**, **Reverse (R)**, and **Neutral (N)** using an analog joystick[cite: 1].
- **Throttle & Braking:** Smooth acceleration and deceleration dynamics controlled by push buttons with real-time speed rendering on a 4-digit 7-segment display[cite: 1].
- **Brake Indicator:** Dedicated LED output lights up dynamically when the brake pedal is depressed[cite: 1].
- **Speed Limit Warning:** Triggers an audible alert if the vehicle speed exceeds 120 km/h[cite: 1].

### 3. Cruise Control System
- Activates with a dedicated push button to lock the vehicle at the target speed[cite: 1].
- Dynamically responds to driver override (manual braking or acceleration) while updating system status on the secondary information display[cite: 1].

### 4. Ultrasonic Reverse Parking Assist
- Automatically activates in **Reverse (R)** gear using an HC-SR04 ultrasonic sensor[cite: 1].
- Displays real-time distance in centimeters on the 7-segment module[cite: 1].
- **Multi-Tiered Warning Matrix:**
  - Visual proximity indicators (Green / Yellow / Red zones) on the 8x8 LED matrix[cite: 1].
  - Variable-frequency buzzer audio cues that increase in speed as distance decreases[cite: 1].

### 5. Smart Doors & Central Locking
- **Individual Servo Control:** Controls 4 independent door angles (Front-Left, Front-Right, Rear-Left, Rear-Right) using dedicated analog potentiometers and servo motors[cite: 1].
- **Central Lock System:** Locks all doors simultaneously via a central lock button[cite: 1].
- **Door Safety Interlock:** Prevents central locking if any door remains open, issuing an auditory chime and LCD alert[cite: 1].
- **Graphical Door Status:** Real-time visual and textual status of all 4 doors displayed on LCD2[cite: 1].

### 6. Safety & Environmental Monitoring
- **Climate Monitoring:** Employs a **DHT22** temperature and humidity sensor[cite: 1].
- Automated alerts for extreme hot/cold temperatures or high humidity levels broadcasted to LCDs and LED matrices[cite: 1].
- **Seatbelt Monitor:** Continuously scans driver and passenger seatbelt potentiometers, launching a 5-second acoustic alarm if the car moves unbelted[cite: 1].
- **Front Pedestrian/Obstacle Detection:** Utilizes a PIR motion sensor to detect obstacles ahead when in **Drive (D)** mode, triggering instant hazard icons on the LED matrix and LCD[cite: 1].

### 7. Lighting, Mirrors & Horn
- **Multi-Stage Headlights:** Controlled via potentiometer for **OFF**, **FOG LIGHTS**, and **FULL BEAM** modes[cite: 1].
- **Turn Signals & Hazard Lights:** Joystick left/right deflection activates left/right indicators with synchronized blinking[cite: 1].
- **Power Folding Mirrors:** Retracts and deploys side mirrors using dual servo motors[cite: 1].
- **Steering Horn:** Pressing the joystick pushbutton sounds an instant horn tone[cite: 1].

---

## 💻 Hardware Bill of Materials (BOM)

| Component | Quantity | Purpose |
| :--- | :---: | :--- |
| **Arduino Mega 2560** | 1 | Main System Microcontroller[cite: 1] |
| **I2C LCD 20x4** | 2 | Primary Dashboard (LCD1) & Door Status Monitor (LCD2)[cite: 1] |
| **TM1637 4-Digit 7-Segment** | 1 | Speedometer & Parking Distance Display[cite: 1] |
| **MAX7219 LED Matrix (2x 8x8)** | 1 | Graphical Status Icons, Warnings & Parking Grid[cite: 1] |
| **Servo Motors** | 6 | 4x Door Motors + 2x Folding Mirror Motors[cite: 1] |
| **DHT22 Sensor** | 1 | Ambient Temperature & Humidity Sensing[cite: 1] |
| **HC-SR04 Ultrasonic** | 1 | Reverse Distance Measurement[cite: 1] |
| **PIR Motion Sensor** | 1 | Front Obstacle / Pedestrian Detection[cite: 1] |
| **Analog Joystick** | 1 | Gear Selector (Y-axis), Turn Signals (X-axis), Horn (SW)[cite: 1] |
| **Potentiometers** | 7 | 4x Doors, 2x Seatbelts, 1x Headlight Knob[cite: 1] |
| **Push Buttons** | 7+ | Ignition, Gas, Brake, Cruise Control, Central Lock, Page Toggle, Mirrors[cite: 1] |
| **Buzzer & LEDs** | - | Auditory Feedback, Brake Light, Indicators, Fog/Headlights[cite: 1] |

---

## 🖥 User Interface Layout (LCD 1 Pages)

Navigate between dashboard pages on LCD 1 using the UI toggle button:

### Page 1: Main Driving Dashboard
+--------------------+
| GEAR: D   TEMP:24.5C|
| HUM : 45% LIGHT:FULL|
| SYSTEM OK / ALERTS |
|                    |
+--------------------+[cite: 1]

### Page 2: Vehicle Status & Diagnostics
+--------------------+
| MIRRORS : OPEN     |
| CRUISE  : ON 80kmh |
| S-BELTS : D:OK P:OK|
|                    |
+--------------------+[cite: 1]

---

## ⚡ Getting Started

### Prerequisites
Ensure you have the latest **Arduino IDE** installed along with the required libraries:
- `LiquidCrystal_I2C`
- `LedControl`
- `TM1637Display`
- `DHT sensor library`
- `Servo`

### Hardware Setup & Flashing
1. **Connect Pins:** Connect the components according to the pin definitions declared at the beginning of `CAR-Arduino.ino`[cite: 1].
2. **Open Project:** Clone this repository and open `CAR-Arduino.ino` in Arduino IDE[cite: 1].
   `git clone [https://github.com/Sobhankhedry/CAR-Arduino.git](https://github.com/Sobhankhedry/CAR-Arduino.git)`
3. **Select Board:** In Arduino IDE, set Board to **Arduino Mega 2560** and choose the correct serial port[cite: 1].
4. **Upload:** Click **Upload** to compile and flash the firmware onto the board[cite: 1].

---

## 📄 License
Distributed under the MIT License. See `LICENSE` for more information.
