#include <ESP8266WiFi.h>

String apiKey = "35W987LLK7FKU288"; // Enter your Write API key from ThingSpeak
const char *ssid = "Garsivaz"; // replace with your wifi ssid and wpa2 key
const char *pass = "hermanhe3";
const char* server = "api.thingspeak.com";
WiFiClient client;

const int e18_sensor = D3; // Define the sensor pin
const int led_pin = D4; // Define the LED pin

void setup() {
  Serial.begin(9600); // Start the serial communication at 9600 baud rate
  pinMode(led_pin, OUTPUT); // Set the LED pin as an OUTPUT
  pinMode(e18_sensor, INPUT); // Set the sensor pin as an INPUT
  digitalWrite(led_pin, LOW); // Initially turn off the LED
  Serial.println("E18-D80NK initialized"); // Print initialization message
}

void loop() {
  if (digitalRead(e18_sensor) == HIGH) {
    // If the sensor detects an obstacle (HIGH signal)
    digitalWrite(led_pin, HIGH); // Turn on the LED
    Serial.println("1"); // Print "1" to Serial Monitor
  } else {
    // If the sensor does not detect any obstacle (LOW signal)
    digitalWrite(led_pin, LOW); // Turn off the LED
    Serial.println("0"); // Print "0" to Serial Monitor
  }
  delay(100); // Small delay to prevent serial monitor overflow
}
