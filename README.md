
```mermaid
# 🤖 Food Delivery Rover with RTOS, PID Control & Edge Vision

A highly reliable, fault-tolerant autonomous food delivery robot. Built with a distributed architecture using three ESP32 nodes communicating via ESP-NOW. Features FreeRTOS for multitasking, a custom Ultrasonic PID controller for smooth braking, Telegram Bot integration for secure PIN verification, and Edge-to-Server Computer Vision using YOLOv3.

---

## 🌟 Core Architecture

Sistem ini dipecah menjadi 3 *node* terpisah untuk memastikan kestabilan dan membagi beban komputasi:

1. **📱 Transmitter Node (Controller):** Mengelola interaksi *user* via Telegram Bot, men-*generate* PIN acak untuk keamanan pengambilan makanan, dan mengirim perintah secara nirkabel.
2. **🏎️ Rover Node (Receiver):** Otak penggerak robot yang menjalankan **FreeRTOS** untuk mengeksekusi navigasi FSM. Dilengkapi dengan **Ultrasonic PID Control** (`kp = 3.0, ki = 0.5, kd = 1.0`) untuk pengereman yang *smooth*, pengaturan layar OLED, dan pemantauan sinyal RSSI.
3. **👁️ Vision Node (ESP32-CAM):** Bertindak sebagai *Edge Device* yang melakukan *streaming* video latensi rendah ke laptop/server untuk diproses oleh model AI YOLOv3.

---

## 🧠 System Topology

```mermaid
graph TD
    subgraph User Control
        TG[Telegram Bot]
    end

    subgraph Transmitter Node
        TX[ESP32 Controller]
    end

    subgraph Rover Node
        RX[ESP32 Rover & FreeRTOS]
        PID[PID Braking System]
        OLED[OLED Faces]
        MOTOR[Motor Driver]
        SONAR[Ultrasonic Sensor]
    end
    
    subgraph Vision System
        CAM[ESP32-CAM]
        YOLO[Laptop / YOLOv3 AI]
    end

    TG -- Command / PIN --> TX
    TX -- Telegram API --> TG
    TX -- ESP-NOW / Channel 13 --> RX
    RX -- ESP-NOW / RSSI Status --> TX
    
    RX --> PID
    PID --> MOTOR
    RX --> OLED
    RX --> SONAR

    CAM -- HTTP Video Stream --> YOLO

```
## ✨ Advanced Features
 * **⚡ FreeRTOS Integration:** Sensor pembacaan, navigasi FSM, dan komunikasi ESP-NOW berjalan di *task* independen menggunakan Queue (xQueueSend & xQueueReceive) untuk arsitektur *non-blocking*.
 * **🎛️ Ultrasonic PID Closed-Loop:** Mengimplementasikan kalkulasi *Proportional-Integral-Derivative* (PID) murni berbasis jarak target untuk menghasilkan akselerasi dan pengereman yang presisi tanpa *rotary encoder*. Dilengkapi juga dengan mekanisme *Anti-Windup*.
 * **⚙️ Hardware Compensation Logic:** Menerapkan kalibrasi *offset* dinamis (pwm2 = basePwm + 30) pada motor B untuk mengompensasi kelemahan traksi fisik, memastikan laju robot tetap lurus.
 * **🛜 Ultra-Low Latency ESP-NOW:** Komunikasi langsung antar *node* (Rover & Transmitter) di Channel 13 tanpa bergantung pada *router* WiFi eksternal.
 * **🔐 Secure PIN Validation:** Robot akan menghasilkan 4 digit PIN acak ketika sampai di tujuan. Pengguna harus memasukkan PIN via Telegram untuk memvalidasi pengantaran.
 * **🤖 Dynamic UI/UX:** Dilengkapi layar OLED yang menampilkan ekspresi robot (*Senyum, Marah, Datar, Tulus*, dll) secara *real-time* sesuai dengan kondisinya.
## 🛠️ Hardware & Pin Mapping
### 1. Rover Node (Receiver)
| Component | Pin ESP32 | Function |
|---|---|---|
| **Motor L298N** | 12, 13, 14, 33 | IN1, IN2, IN3, IN4 |
| **Motor PWM** | 26, 15 | ENA, ENB |
| **Ultrasonic HC-SR04** | 16, 17 | TRIG, ECHO |
| **OLED 128x64** | 21, 22 | SDA, SCL (I2C) |
### 2. Transmitter Node (Controller)
| Component | Pin ESP32 | Function |
|---|---|---|
| **7-Segment/LED** | 12, 13, 14 | Indikator Status |
| **Buzzer** | 16 | Alarm Notifikasi |
## ⚙️ FSM States (Rover)
Navigasi robot diatur dengan konsep *Finite State Machine* (FSM):
 1. **IDLE:** Menunggu instruksi (berwajah datar), motor mati.
 2. **JALAN:** Robot bergerak maju (berwajah senyum) dikendalikan kalkulasi PID berdasarkan jarak halangan.
 3. **HALANGAN:** Terdeteksi halangan < 20cm, robot melakukan manuver menghindar (berwajah kaget).
 4. **SAMPAI:** Mencapai tujuan (berdasarkan RSSI), meminta input PIN (berwajah tulus).
 5. **SELESAI:** PIN benar, robot berputar arah dan kembali ke posisi *idle*.
```

```
