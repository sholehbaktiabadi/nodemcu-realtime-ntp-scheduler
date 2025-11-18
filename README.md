# Lamp Scheduler (NodeMCU + Relay + NTP + WiFi Web UI)

This project is a **WiFi-controlled smart relay scheduler** built using **NodeMCU (ESP8266)**, a mechanical relay module, and an NTP-based time system.  
The device automatically turns a relay **ON/OFF at specific scheduled times**, and also provides a simple **web dashboard** to monitor the status in real time.

---

## 🚀 Features

- ⏰ Real-time clock using **NTP (pool.ntp.org)**
- 🌏 UTC+7 (WIB) timezone support
- 🔌 Controls a **mechanical relay (SRD-05VDC-SL-C)**
- 🕒 Scheduled ON/OFF control based on hour and minute
- 🌐 Built-in WiFi Web Dashboard (auto-refresh)
- 📡 Runs on NodeMCU ESP8266
- 📄 Clean HTML UI with dynamic relay state display

---

## 🛠 Hardware Required

- **NodeMCU ESP8266**
- **5V Relay Module – SRD-05VDC-SL-C**
- USB cable
- Jumper wires

---

## 🔌 Wiring Diagram

| NodeMCU Pin | Relay Module |
|-------------|--------------|
| **VU (5V)** | **VCC** |
| **GND**     | **GND** |
| **D0** (or D1/D2 recommended) | **IN** |

> ⚠️ Important  
> - Use **VU (5V)** instead of **3.3V** — the relay requires 5V to activate strongly.  
> - Mechanical relay = **click sound** when switching; SSR modules do not click.

---

## 📜 Code Overview

Scheduling (default):

```cpp
const int ON_HOUR = 17; // will Turn on at 17:15 - UTC+7
const int ON_MINUTE = 15;

const int OFF_HOUR = 6; // will Turn off at 06:00 - UTC+7
const int OFF_MINUTE = 0;

```

## 📄 Monitor

> access the nodemcu IP address to monitor the scheduler status by check the serial monitor on the Arduino with baud 115200
> 
<img width="527" height="550" alt="Screenshot from 2025-11-18 12-44-47" src="https://github.com/user-attachments/assets/d4a6de9d-7910-4210-8275-d360e5d62c1f" />



