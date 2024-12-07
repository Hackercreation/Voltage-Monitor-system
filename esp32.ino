#define BLYNK_TEMPLATE_ID "TMPL38s6_7vfZ"
#define BLYNK_TEMPLATE_NAME "Voltage Monitor System"
#define BLYNK_AUTH_TOKEN "PU8Fg_vF5RFebgN8vwniavYEC09HVik1"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <U8g2lib.h>

// OLED Initialization
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define Sensor 34 // ADC pin on ESP32
float vOUT = 0.0;
float vIN = 0.0;
float R1 = 30000.0; // 30k ohms resistor
float R2 = 7500.0;  // 7.5k ohms resistor

// Wi-Fi Credentials
char ssid[] = "ROBOAIHUB";  // Replace with your Wi-Fi SSID
char pass[] = "ROBOAIHUB";   // Replace with your Wi-Fi password

BlynkTimer timer;

// Function to read the voltage and calculate the actual voltage
float readVoltage() {
  float total = 0;
  for (int i = 0; i < 10; i++) { // Average 10 samples
    total += analogRead(Sensor);
    delay(10); // Small delay between readings
  }
  float value = total / 10;

  vOUT = (value * 3.3) / 4095.0; // Scale to 3.3V (for ESP32)
  vIN = vOUT / (R2 / (R1 + R2)); // Voltage divider calculation
  
  return vIN;
}

// Function to send voltage data to Blynk
void sendVoltageToBlynk() {
  float voltage = readVoltage();
  Blynk.virtualWrite(V0, voltage);  // Send voltage to Blynk Virtual Pin V0
}

// Function to display voltage on OLED
void displayVoltageOnOLED() {
  u8g2.clearBuffer();
  u8g2.setCursor(5, 15);
  u8g2.print("Voltage Monitor");
  u8g2.drawLine(0, 20, 128, 20);
  u8g2.setCursor(5, 40);
  u8g2.print(vIN, 2);
  u8g2.print(" V");
  u8g2.sendBuffer();
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Initialize OLED
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB14_tr);

  // Configure ADC resolution
  analogReadResolution(12); // ESP32 ADC has 12-bit resolution

  // Set up a timer to send voltage data every second
  timer.setInterval(1000L, sendVoltageToBlynk);  // Update Blynk every second
}

void loop() {
  // Run Blynk
  Blynk.run();
  timer.run();

  // Read and calculate voltage
  vIN = readVoltage();  // Update vIN using the shared function

  // Display voltage on OLED
  displayVoltageOnOLED();

  delay(1000); // Update OLED display every second to match Blynk data
}

