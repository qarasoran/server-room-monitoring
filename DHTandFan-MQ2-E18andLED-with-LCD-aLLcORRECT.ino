#include <DHT.h>               // Library for DHT22 temperature and humidity sensor
#include "MQ135.h"             // Library for MQ135 air quality sensor
#include <ESP8266WiFi.h>       // Library for WiFi connectivity
#include <LiquidCrystal_I2C.h> // Library for I2C LCD display

// ThingSpeak API and WiFi credentials
String apiKey = "Z7IPEN71YA6CHZ5I"; // ThingSpeak Write API key
const char *ssid = "Garsivaz";       // WiFi SSID
const char *pass = "hermanhe3";      // WiFi password
const char* server = "api.thingspeak.com"; // ThingSpeak server address
const int sensorPin = 0;
int air_quality;

// Pin configuration
#define DHTPIN D7  // Pin for DHT22 sensor
#define LEDPIN D6  // Pin for LED indicator
const int e18_sensor = D5; // Sensor pin for object detection
const int led_pin = D8;    // LED pin (or relay) for object detection

// Initialize sensors and LCD
DHT dht(DHTPIN, DHT22);
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27 (from DIYables LCD), 16 columns, and 2 rows

WiFiClient client;

void setup() {
    Serial.begin(115200);       // Start serial communication at 115200 baud rate
    delay(10);
    
    dht.begin();                // Initialize DHT sensor
    lcd.begin();                // Initialize LCD
    lcd.backlight();            // Turn on LCD backlight
    
    pinMode(LEDPIN, OUTPUT);    // LED for temperature indication
    pinMode(e18_sensor, INPUT); // Object detection sensor
    pinMode(led_pin, OUTPUT);   // LED (or relay) for object detection
    digitalWrite(LEDPIN, LOW);  // Initially turn off LED
    digitalWrite(led_pin, LOW); // Initially turn off LED for object detection
    
    Serial.println("Connecting to WiFi");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
}

void loop() {
    // Read temperature and humidity
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    // Read air quality from MQ135 sensor
    MQ135 gasSensor = MQ135(A0);
    air_quality = gasSensor.getPPM();
    
    // Read object detection sensor
    int sensorValue = digitalRead(e18_sensor);
    
    // Display temperature and humidity on LCD
    lcd.clear();
    if (isnan(t) || isnan(h)) {
        lcd.setCursor(0, 0);
        lcd.print("Failed to read DHT");
    } else {
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(t);
        lcd.print(" C");

        lcd.setCursor(0, 1);
        lcd.print("Humi: ");
        lcd.print(h);
        lcd.print("%");
    }

    // Control LED based on temperature
    if (t > 30) {
        digitalWrite(LEDPIN, HIGH);
    } else {
        digitalWrite(LEDPIN, LOW);
    }

    // Control LED (or relay) based on object detection
    if (sensorValue == HIGH) {
        digitalWrite(led_pin, HIGH); // Turn on the LED (or relay)
        Serial.println("Object detected");
    } else {
        digitalWrite(led_pin, LOW);  // Turn off the LED (or relay)
        Serial.println("No object detected");
    }

    // Send data to ThingSpeak
    if (client.connect(server, 80)) {
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(t);
        postStr += "&field2=";
        postStr += String(h);
        postStr += "&field3=";
        postStr += String(air_quality);
        postStr += "&field4=";
        postStr += String(sensorValue);
        postStr += "\r\n\r\n";

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);

        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.print(" C, Humidity: ");
        Serial.print(h);
        Serial.print("%, Air Quality: ");
        Serial.print(air_quality);
        Serial.print(" PPM, Object Detection: ");
        Serial.println(sensorValue);
    }
    client.stop();

    Serial.println("Waiting...");

    delay(1000); // Minimum delay between updates to ThingSpeak
}
