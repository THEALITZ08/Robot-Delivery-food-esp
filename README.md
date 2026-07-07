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

# 🧠 System Architecture

Robot terdiri dari tiga node utama.

## 📱 Controller Node

ESP32 devkit v4

Responsibilities

- Telegram Bot
- Generate PIN
- Send command
- Receive RSSI
- ESP-NOW Master

---

## 🚗 Rover Node

ESP32 devkit v1 + FreeRTOS

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
- YOLOv3 Detection
- Otomatisasi deteksi objek menggunakan YOLOv3 dijalankan pada laptop. ESP32-CAM hanya berfungsi sebagai perangkat yang mengirimkan video secara real-time melalui HTTP stream menuju laptop untuk diproses oleh model YOLOv3

---

# 🧩 System Topology

```mermaid

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

MOVE --> DESTINATION : Object Detected

DESTINATION --> VERIFY_PIN

VERIFY_PIN --> RETURN : PIN Valid

RETURN --> IDLE
```

---

# 🎯 PID Control
Robot menggunakan algoritma Proportional-Integral-Derivative (PID) untuk mengontrol kecepatan berdasarkan jarak yang dibaca sensor ultrasonik.

### Proportional (P)

Formula

`P = Kp × e(t)`

Keterangan:
- Kp = proportional gain
- e(t) = error

Menghasilkan respon yang sebanding dengan besar error.

---

### Integral (I)

Formula

`I = Ki × ∫e(t)dt`

Implementasi diskrit:

`I = Ki × Σe`

Mengurangi steady-state error dengan mengakumulasi error.

---

### Derivative (D)

Formula

`D = Kd × (de/dt)`

Implementasi diskrit:

`D = Kd × (error - lastError) / dt`

Memprediksi perubahan error sehingga mengurangi overshoot.

---

### PID Output

`Output = P + I + D`

atau

`u(t) = Kp·e(t) + Ki∫e(t)dt + Kd(de(t)/dt)`
 
---

## Parameter PID

| Parameter | Value |
|-----------|------:|
| Kp | 3.72 |
| Ki | 2.58 |
| Kd | 1.67 |

---

## PID Flow

```text
Target Distance
        │
        ▼
Calculate Error
        │
        ▼
P + I + D
        │
        ▼
PWM Motor
        │
        ▼
Robot Movement
        │
        ▼
Ultrasonic Feedback
        │
        └──────────────┐
                       ▼
               Calculate Error
```
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

Robot menggunakan metode:

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

## Receiver

| Component | GPIO |
|-----------|------|
| L298N | 12 13 14 33 |
| PWM | 15 26 |
| HC-SR04 | 16 17 |
| OLED | SDA 21 SCL 22 |

---

## Trasnmitter

| Component | GPIO |
|-----------|------|
| Buzzer | 16 |
| Indicator LED | 12 13 14 |

---

# 💻 Software Stack

- Visual Studio Code
- PlatformIO Extension
- FreeRTOS
- Queue
- ESP-NOW
- OpenCV
- YOLOv3
- Python

---

# 🚀 Features

- ✅ ESP-NOW
- ✅ FreeRTOS
- ✅ Queue
- ✅ PID
- ✅ OLED UI
- ✅ Telegram Bot
- ✅ RSSI Monitoring
- ✅ YOLOv3
- ✅ Autonomous Delivery

---