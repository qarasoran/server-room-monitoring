#include <DHT.h>      // Including library for dht
#include <LiquidCrystal.h> // Including library for LCD
#include "MQ135.h"
#include <ESP8266WiFi.h>

String apiKey = "Z7IPEN71YA6CHZ5I";   // Enter your Write API key from ThingSpeak
const char *ssid =  "Garsivaz";       // replace with your wifi ssid and wpa2 key
const char *pass =  "hermanhe34";
const char* server = "api.thingspeak.com";
WiFiClient client;

#define DHTPIN D7    // Connect to GPIO 2 in NodeMCU Board

DHT dht(DHTPIN, DHT22);
MQ135 gasSensor(A0); // Define gas sensor pin

// Define LCD pins
const int rs = D5, en = D6, d4 = D1, d5 = D2, d6 = D3, d7 = D4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int fan_relay = D8;  // Define relay pin for fan control

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();
  lcd.begin(16, 2);  // Initialize the LCD with 16 columns and 2 rows

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  pinMode(fan_relay, OUTPUT);  // Set fan relay pin as OUTPUT
  digitalWrite(fan_relay, LOW);  // Initially turn off the fan
  
  lcd.setCursor(0, 0);
  lcd.print("DHT22 & MQ135");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int air_quality = gasSensor.getPPM();

  if (WiFi.status() == WL_CONNECTED) {
    // Send data to ThingSpeak
    if (client.connect(server, 80)) {
      String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(t);
      postStr += "&field2=";
      postStr += String(h);
      postStr += "&field3=";
      postStr += String(air_quality);

      client.println("POST /update HTTP/1.1");
      client.println("Host: api.thingspeak.com");
      client.println("Connection: close");
      client.println("X-THINGSPEAKAPIKEY: " + apiKey);
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(postStr.length());
      client.println();
      client.print(postStr);

      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" °C, Humidity: ");
      Serial.print(h);
      Serial.print(" %, Air Quality: ");
      Serial.print(air_quality);
      Serial.println(" PPM. Sent to ThingSpeak.");

      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(t);
      lcd.print(" C");

      lcd.setCursor(0, 1);
      lcd.print("Humidity: ");
      lcd.print(h);
      lcd.print(" %");

      // Control fan based on temperature
      if (t > 30.0) {
        digitalWrite(fan_relay, HIGH);  // Turn on fan
        lcd.setCursor(13, 0);
        lcd.print("*");  // Blink indication
      } else {
        digitalWrite(fan_relay, LOW);   // Turn off fan
        lcd.setCursor(13, 0);
        lcd.print(" ");  // Clear blink indication
      }
    }
    client.stop();
  } else {
    Serial.println("WiFi not connected.");
  }

  delay(15000);  // Delay for 15 seconds before next update
}
