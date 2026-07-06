# 🤖 Autonomous Food Delivery Rover
### FreeRTOS • ESP-NOW • YOLOv3 • PID Controller • ESP32-CAM

Autonomous food delivery robot built with a distributed embedded architecture using multiple ESP32 boards.

The project combines:

- 🚗 Autonomous navigation
- 📷 Computer Vision (YOLOv3)
- 📡 ESP-NOW communication
- ⚙️ FreeRTOS multitasking
- 🎯 PID distance control
- 🔐 Telegram Bot authentication
- 📺 OLED user interface

Unlike a traditional line follower, this robot performs autonomous navigation while communicating with multiple embedded nodes in real time.

---

# 📷 Project Preview

> Tambahkan foto robot di sini

```
images/
 ├── robot.jpg
 ├── rover.jpg
 ├── transmitter.jpg
 ├── esp32cam.jpg
```

---

# 🎥 Demo

Tambahkan link YouTube

https://youtube.com/......

---

# 🧠 System Architecture

Robot terdiri dari tiga node utama.

## 📱 Controller Node

ESP32

Responsibilities

- Telegram Bot
- Generate PIN
- Send command
- Receive RSSI
- ESP-NOW Master

---

## 🚗 Rover Node

ESP32 + FreeRTOS

Responsibilities

- Finite State Machine
- PID Control
- Ultrasonic
- Motor Driver
- OLED Animation
- RSSI Monitoring

---

## 📷 Vision Node

ESP32-CAM

Responsibilities

- HTTP Video Streaming
- ArUco Detection
- YOLOv3 Detection

---

# 🧩 System Topology

```mermaid
graph TD

USER[User]

TG[Telegram Bot]

CTRL[ESP32 Controller]

ROVER[ESP32 Rover]

CAM[ESP32-CAM]

YOLO[YOLOv3]

OLED[OLED Display]

SONAR[HC-SR04]

PID[PID Controller]

MOTOR[L298N Driver]

USER --> TG

TG --> CTRL

CTRL -->|ESP-NOW| ROVER

ROVER -->|RSSI| CTRL

ROVER --> SONAR

SONAR --> PID

PID --> MOTOR

ROVER --> OLED

CAM -->|HTTP Stream| YOLO
```

---

# ⚙️ Finite State Machine

```mermaid
stateDiagram-v2

[*] --> IDLE

IDLE --> MOVE : Delivery Command

MOVE --> OBSTACLE : Distance < Threshold

OBSTACLE --> MOVE : Path Clear

MOVE --> DESTINATION : Marker Detected

DESTINATION --> VERIFY_PIN

VERIFY_PIN --> RETURN : PIN Valid

RETURN --> IDLE
```

---

# 🧠 PID Control

Robot menggunakan PID Controller berbasis sensor ultrasonik.

Formula

```
error = targetDistance - currentDistance

P = Kp × error

I = Ki × totalError

D = Kd × (error - lastError)

Output = P + I + D
```

Parameter

| Parameter | Value |
|-----------|------:|
| Kp | 3.0 |
| Ki | 0.5 |
| Kd | 1.0 |

---

# ⚡ FreeRTOS Tasks

| Task | Function |
|------|----------|
| Navigation Task | FSM Navigation |
| ESP-NOW Task | Communication |
| OLED Task | Animation |
| Sensor Task | Ultrasonic |
| Queue Task | Command Processing |

---

# 📦 Queue Communication

```
Telegram

↓

ESP-NOW

↓

Queue

↓

FSM

↓

Motor
```

Queue digunakan supaya setiap command diproses secara berurutan tanpa blocking.

---

# 📡 Wireless Communication

Protocol

ESP-NOW

Channel

13

Data

- Command
- RSSI
- Navigation
- Status

---

# 👁 Computer Vision

Robot menggunakan dua metode.

## ArUco Marker

- Navigation
- Position Correction
- Marker Retry

## YOLOv3

- Object Detection
- Human Detection
- Delivery Validation

---

# 🖥 OLED Animation

Robot memiliki beberapa ekspresi.

- 😊 Senyum
- 😐 Normal
- 😮 Kaget
- 😠 Marah
- 😭 Nangis
- 🙂 Tulus

Ekspresi berubah mengikuti kondisi robot secara real-time.

---

# 🔐 Telegram Authentication

Flow

User

↓

Telegram Bot

↓

Random PIN

↓

Robot

↓

Input PIN

↓

Valid

↓

Robot Return

---

# 🛠 Hardware

## Rover

| Component | GPIO |
|-----------|------|
| L298N | 12 13 14 33 |
| PWM | 15 26 |
| HC-SR04 | 16 17 |
| OLED | SDA 21 SCL 22 |

---

## Controller

| Component | GPIO |
|-----------|------|
| Buzzer | 16 |
| Indicator LED | 12 13 14 |

---

# 💻 Software Stack

- Arduino IDE
- FreeRTOS
- ESP-NOW
- PubSubClient
- ESP32Servo
- ArduinoJson
- OpenCV
- YOLOv3
- Python

---

# 📁 Project Structure

```
Controller/
Receiver/
ESP32CAM/
YOLO/
Images/
README.md
```

---

# 🚀 Features

- ✅ ESP-NOW
- ✅ FreeRTOS
- ✅ Queue
- ✅ PID
- ✅ OLED UI
- ✅ Telegram Bot
- ✅ RSSI Monitoring
- ✅ ArUco Navigation
- ✅ YOLOv3
- ✅ Retry Marker
- ✅ Autonomous Delivery

---