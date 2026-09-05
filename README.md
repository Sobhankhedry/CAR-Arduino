# 🚗 Smart Car Simulation System

[![Microprocessor Project](https://img.shields.io/badge/Project-Microprocessors%20Course-blue.svg)](https://github.com/Sobhankhedry/CAR-Arduino)
[![Board](https://img.shields.io/badge/Board-Arduino%20Mega%202560-green.svg)](https://www.arduino.cc/)
[![Language](https://img.shields.io/badge/Language-C%2B%2B%2FArduino-orange.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-MIT-brightgreen.svg)](LICENSE)

An advanced, interactive hardware and software smart car simulation system built on the **Arduino Mega 2560** platform. This project emulates essential real-world automotive features, including ignition lifecycle, gear shifting, cruise control, ultrasonic parking assistance, smart central locking, ambient sensing, and multi-tier safety alert systems.

---

## 📌 Project Overview
- **Course:** Microprocessors & Embedded Systems
- **Instructor:** Dr. Sajjad Nezhadhassan
- **Author:** Sobhan Khedry
- **Repository:** [Sobhankhedry/CAR-Arduino](https://github.com/Sobhankhedry/CAR-Arduino)

---

## 🛠 Key Features

### 1. Engine Start/Stop & Ignition Lifecycle
- **Interactive Warm-up Sequence:** Press the start button to initiate engine heating with a countdown displayed on the 7-segment display.
- **Custom Welcome Screen:** Plays a welcome chime via buzzer while rendering custom animations on the LED matrix and welcoming the user on the primary LCD.
- **Safe Shutdown Mode:** Implements a controlled "Stay Safe" engine shutdown process.

### 2. Gearbox & Driving Dynamics
- **Multi-Mode Transmission:** Toggle between **Drive (D)**, **Reverse (R)**, and **Neutral (N)** using an analog joystick.
- **Throttle & Braking:** Smooth acceleration and deceleration dynamics controlled by push buttons with real-time speed rendering on a 4-digit 7-segment display.
- **Brake Indicator:** Dedicated LED output lights up dynamically when the brake pedal is depressed.
- **Speed Limit Warning:** Triggers an audible alert if the vehicle speed exceeds 120 km/h.

### 3. Cruise Control System
- Activates with a dedicated push button to lock the vehicle at the target speed.
- Dynamically responds to driver override (manual braking or acceleration) while updating system status on the secondary information display.

### 4. Ultrasonic Reverse Parking Assist
- Automatically activates in **Reverse (R)** gear using an HC-SR04 ultrasonic sensor.
- Displays real-time distance in centimeters on the 7-segment module.
- **Multi-Tiered Warning Matrix:**
  - Visual proximity indicators (Green / Yellow / Red zones) on the 8x8 LED matrix.
  - Variable-frequency buzzer audio cues that increase in speed as distance decreases.

### 5. Smart Doors & Central Locking
- **Individual Servo Control:** Controls 4 independent door angles (Front-Left, Front-Right, Rear-Left, Rear-Right) using dedicated analog potentiometers and servo motors.
- **Central Lock System:** Locks all doors simultaneously via a central lock button.
- **Door Safety Interlock:** Prevents central locking if any door remains open, issuing an auditory chime and LCD alert.
- **Graphical Door Status:** Real-time visual and textual status of all 4 doors displayed on LCD2.

### 6. Safety & Environmental Monitoring
- **Climate Monitoring:** Employs a **DHT22** temperature and humidity sensor.
- Automated alerts for extreme hot/cold temperatures or high humidity levels broadcasted to LCDs and LED matrices.
- **Seatbelt Monitor:** Continuously scans driver and passenger seatbelt potentiometers, launching a 5-second acoustic alarm if the car moves unbelted.
- **Front Pedestrian/Obstacle Detection:** Utilizes a PIR motion sensor to detect obstacles ahead when in **Drive (D)** mode, triggering instant hazard icons on the LED matrix and LCD.

### 7. Lighting, Mirrors & Horn
- **Multi-Stage Headlights:** Controlled via potentiometer for **OFF**, **FOG LIGHTS**, and **FULL BEAM** modes.
- **Turn Signals & Hazard Lights:** Joystick left/right deflection activates left/right indicators with synchronized blinking.
- **Power Folding Mirrors:** Retracts and deploys side mirrors using dual servo motors.
- **Steering Horn:** Pressing the joystick pushbutton sounds an instant horn tone.

---

## 💻 Hardware Bill of Materials (BOM)

| Component | Quantity | Purpose |
| :--- | :---: | :--- |
| **Arduino Mega 2560** | 1 | Main System Microcontroller |
| **I2C LCD 20x4** | 2 | Primary Dashboard (LCD1) & Door Status Monitor (LCD2) |
| **TM1637 4-Digit 7-Segment** | 1 | Speedometer & Parking Distance Display |
| **MAX7219 LED Matrix (2x 8x8)** | 1 | Graphical Status Icons, Warnings & Parking Grid |
| **Servo Motors** | 6 | 4x Door Motors + 2x Folding Mirror Motors |
| **DHT22 Sensor** | 1 | Ambient Temperature & Humidity Sensing |
| **HC-SR04 Ultrasonic** | 1 | Reverse Distance Measurement |
| **PIR Motion Sensor** | 1 | Front Obstacle / Pedestrian Detection |
| **Analog Joystick** | 1 | Gear Selector (Y-axis), Turn Signals (X-axis), Horn (SW) |
| **Potentiometers** | 7 | 4x Doors, 2x Seatbelts, 1x Headlight Knob |
| **Push Buttons** | 7+ | Ignition, Gas, Brake, Cruise Control, Central Lock, Page Toggle, Mirrors |
| **Buzzer & LEDs** | - | Auditory Feedback, Brake Light, Indicators, Fog/Headlights |

---

## 🖥 User Interface Layout (LCD 1 Pages)

Navigate between dashboard pages on LCD 1 using the UI toggle button:

### Page 1: Main Driving Dashboard
+--------------------+
| GEAR: D   TEMP:24.5C|
| HUM : 45% LIGHT:FULL|
| SYSTEM OK / ALERTS |
|                    |
+--------------------+

### Page 2: Vehicle Status & Diagnostics
+--------------------+
| MIRRORS : OPEN     |
| CRUISE  : ON 80kmh |
| S-BELTS : D:OK P:OK|
|                    |
+--------------------+

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
1. **Connect Pins:** Connect the components according to the pin definitions declared at the beginning of `CAR-Arduino.ino`.
2. **Open Project:** Clone this repository and open `CAR-Arduino.ino` in Arduino IDE.
   `git clone [https://github.com/Sobhankhedry/CAR-Arduino.git](https://github.com/Sobhankhedry/CAR-Arduino.git)`
3. **Select Board:** In Arduino IDE, set Board to **Arduino Mega 2560** and choose the correct serial port.
4. **Upload:** Click **Upload** to compile and flash the firmware onto the board.

---

## 📄 License
Distributed under the MIT License. See `LICENSE` for more information.
