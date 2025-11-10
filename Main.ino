#define BLYNK_TEMPLATE_ID "TMPL3y3wl5QD3"
#define BLYNK_TEMPLATE_NAME "ESP32 Sensor Logge"
#define BLYNK_AUTH_TOKEN "J0ZFJaB_mLKJ9ianKhArYBi2LoB9IYDP"

#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_INA219.h>

// WiFi
char ssid[] = "5GLAB_2.4G";
char pass[] = "Anurag 1234";

// Sensor objects
Adafruit_MPU6050 mpu;
Adafruit_INA219 ina219;

// Pins
const int PWM_INPUT_PIN = 15;  
const int PWM_OUTPUT_PIN = 4;  

// Data variables
float vibration, temperature, voltage, current;
String vibrationStatus, tempStatus, voltageStatus, currentStatus;
unsigned long lastSendTime = 0;

// Latency
unsigned long latencyStart = 0;
unsigned long latency = 0;

// Google Sheets Script URL
String sheetsScriptURL = "https://script.google.com/macros/s/AKfycbx-2_40gj9DGcJqXUVpuXbHiK5wo9ky0RimI4i2YFTn085DJBlF4w7bGmwVmXE9NEV16w/exec";

void setup() {
  Serial.begin(9600);
  Wire.begin(21, 22);  

  pinMode(PWM_INPUT_PIN, INPUT);
  pinMode(PWM_OUTPUT_PIN, OUTPUT);

  WiFi.begin(ssid, pass);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found");
    while (1) delay(10);
  }

  if (!ina219.begin()) {
    Serial.println("INA219 not found");
    while (1) delay(10);
  }
}

void loop() {
  Blynk.run();

  unsigned long pulseWidth = pulseIn(PWM_INPUT_PIN, HIGH, 25000);
  if (pulseWidth >= 1000 && pulseWidth <= 2000) {
    digitalWrite(PWM_OUTPUT_PIN, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(PWM_OUTPUT_PIN, LOW);
  }

  if (millis() - lastSendTime >= 10000) {
    lastSendTime = millis();

    sensors_event_t a, g, tempEvent;
    mpu.getEvent(&a, &g, &tempEvent);

    vibration = sqrt(
      a.acceleration.x * a.acceleration.x +
      a.acceleration.y * a.acceleration.y +
      a.acceleration.z * a.acceleration.z
    );
    temperature = tempEvent.temperature;
    voltage = ina219.getBusVoltage_V();
    current = ina219.getCurrent_mA() / 1000.0;

    vibrationStatus = (vibration > 20.0) ? "Vibration High" : "";
    voltageStatus   = (voltage > 12.0) ? "Voltage High"   : "";
    tempStatus      = (temperature > 35.0) ? "Temp High"  : "";
    currentStatus   = (current > 20.0) ? "Current High"   : "";

    String status;
    if (vibrationStatus != "") status += vibrationStatus + ", ";
    if (voltageStatus   != "") status += voltageStatus   + ", ";
    if (tempStatus      != "") status += tempStatus      + ", ";
    if (currentStatus   != "") status += currentStatus   + ", ";
    if (status == "") status = "Normal";
    else status.remove(status.length() - 2);

    latencyStart = millis();
    Blynk.virtualWrite(V10, latencyStart);
    latency = millis() - latencyStart;
    Blynk.virtualWrite(V10, latency);

    Blynk.virtualWrite(V0, vibration);
    Blynk.virtualWrite(V1, voltage);
    Blynk.virtualWrite(V2, temperature);
    Blynk.virtualWrite(V3, current);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(sheetsScriptURL);
      http.addHeader("Content-Type", "application/json");

      String postData = "{";
      postData += "\"vibration\":" + String(vibration, 2) + ",";
      postData += "\"voltage\":"   + String(voltage, 2)   + ",";
      postData += "\"temperature\":"+ String(temperature, 2) + ",";
      postData += "\"current\":"   + String(current, 2)   + ",";
      postData += "\"status\":\""  + status + "\"";
      postData += "}";

      int response = http.POST(postData);
      Serial.print("Sheets POST → ");
      Serial.println(response);
      http.end();
    }
  }
}
