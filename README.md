# ESP32 Predictive Maintenance Logger

This project implements an ESP32‑based predictive maintenance system that logs **vibration**, **temperature**, **voltage**, and **current** data using an MPU6050 and INA219 sensor. The system sends real‑time data to **Blynk** and logs all readings to **Google Sheets** through an Apps Script endpoint.

## Features

- Vibration monitoring using MPU6050
- Temperature measurement
- Voltage and current sensing with INA219
- Servo‑tester PWM passthrough to ESC
- Data upload to Google Sheets (HTTP POST)
- Real‑time display on Blynk dashboard
- Status classification for:
  - Vibration
  - Voltage
  - Temperature
  - Current
- Latency measurement for ESP32 ↔ Blynk communication
- Automatic periodic logging every 10 seconds

## Hardware Used

- ESP32 development board  
- MPU6050 accelerometer + gyro sensor  
- INA219 voltage/current sensor  
- Servo tester (PWM input)  
- ESC (PWM output)  
- 2.4 GHz WiFi network  
- Google Sheets (Apps Script endpoint)  

## Pin Configuration

| Function | Pin |
|---------|------|
| PWM Input (Servo Tester) | GPIO 15 |
| PWM Output (ESC)         | GPIO 4  |
| I2C SDA                  | GPIO 21 |
| I2C SCL                  | GPIO 22 |

## Software and Libraries

Required Arduino libraries:

- `WiFi.h`
- `HTTPClient.h`
- `BlynkSimpleEsp32.h`
- `Adafruit_MPU6050`
- `Adafruit_Sensor`
- `Adafruit_INA219`

## How It Works

### 1. PWM Passthrough
The ESP32 reads PWM signals from a servo tester and directly outputs them to the ESC while still logging motor data.

### 2. Sensor Data Acquisition
- Vibration = magnitude of acceleration vector  
- Temperature from MPU6050  
- Bus voltage and current from INA219  

### 3. Status Classification
Thresholds determine whether a parameter is normal or high.

### 4. Data Upload
Every 10 seconds, the ESP32 sends:
- vibration  
- voltage  
- temperature  
- current  
- status  

To:
- Blynk Cloud  
- Google Sheets  

## Google Sheets Logging

The project uses a Google Apps Script Web App endpoint.  
Data is POSTed in JSON format.

Example JSON:
```
{
  "vibration": 12.5,
  "voltage": 11.8,
  "temperature": 32.5,
  "current": 1.9,
  "status": "Normal"
}
```

## Setup Instructions

1. Install the required Arduino libraries.
2. Replace WiFi credentials, Blynk token, and Script URL in the code.
3. Upload the firmware to ESP32.
4. Open Blynk dashboard to monitor live data.
5. Check Google Sheets for continuous logging.

## Author

Anurag Deshmukh  
Embedded Systems • IoT • Predictive Maintenance
