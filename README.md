# Real-Time IoT Water Quality Monitoring System

## Overview
This project implements a real-time IoT-based water quality monitoring system using an ESP32 microcontroller. It measures key water parameters and predicts contamination using rule-based logic.

## Features
- Real-time monitoring of:
  - Total Dissolved Solids (TDS)
  - Turbidity
  - pH
- Noise filtering using moving average
- Sensor calibration
- Rule-based water classification
- Cloud integration using ThingSpeak
- Low-cost and scalable design

## Hardware Requirements
- ESP32
- TDS Sensor
- Turbidity Sensor
- pH Sensor
- Jumper wires
- Breadboard
- Power supply

## Pin Configuration
| Sensor       | ESP32 Pin |
|-------------|----------|
| TDS         | GPIO 34  |
| Turbidity   | GPIO 35  |
| pH          | GPIO 32  |

## Software Requirements
- Arduino IDE
- ESP32 Board Package
- Libraries:
  - WiFi.h
  - HTTPClient.h

## Cloud Setup (ThingSpeak)
1. Create a ThingSpeak account
2. Create a new channel
3. Add fields:
   - Field1 → TDS
   - Field2 → Turbidity
   - Field3 → pH
   - Field4 → Status
4. Copy Write API Key
5. Replace in code:
   ```cpp
   String apiKey = "YOUR_API_KEY";
