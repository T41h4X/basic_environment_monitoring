#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>  // Time management library
#include <DHT.h>      // Include the DHT library


#define MQ2_PIN A0   // Pin for the MQ2 gas sensor
#define DHT_PIN 3    // Pin for the DHT sensor (connected to D3 on Arduino)


#define DHT_TYPE DHT11  // Change to DHT22 if using that sensor
DHT dht(DHT_PIN, DHT_TYPE);  // Initialize DHT sensor


// OLED setup
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);


// Time variables
unsigned long previousMillis = 0;  // Store last time update
const long interval = 1000;         // Interval to update time every 1 second


int hours = 3;     // Set initial time to 3:00:00
int minutes = 3;
int seconds = 0;


void setup() {
  Serial.begin(9600);
 
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }


  dht.begin();  // Initialize the DHT sensor


  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.display();
}


void loop() {
  // Read current time every second
  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;


    // Update the time every second
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
    }
    if (minutes >= 60) {
      minutes = 0;
      hours++;
    }
    if (hours >= 24) {
      hours = 0;
    }
  }


  // Read the DHT sensor data
  float temperature = dht.readTemperature();  // Temperature in Celsius
  float humidity = dht.readHumidity();        // Humidity in percentage


  // If reading fails, display error
  if (isnan(temperature) || isnan(humidity)) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("Failed to read from DHT sensor"));
    display.display();
    return;
  }


  int gasSensorValue = analogRead(MQ2_PIN);
  String airQuality = getAirQuality(gasSensorValue);


  display.clearDisplay();
  display.setCursor(0, 0);
 
  // Display air quality
  display.print("Air quality: ");
  display.println(airQuality);


  // Display temperature
  display.print("Temperature: ");
  display.print(temperature);
  display.println(" C");


  // Display RH%
  display.print("RH%: ");
  display.print(humidity);
  display.println("%");


  // Display real-time (HH:MM:SS)
  display.print("Time: ");
  display.print(hours);
  display.print(":");
  display.print(minutes);
  display.print(":");
  display.println(seconds);


  display.display();


  // Print gas sensor value to serial monitor
  Serial.print("Gas Sensor Value: ");
  Serial.println(gasSensorValue);


  delay(100);  // Small delay to avoid overwhelming the display
}


// Function to determine air quality based on MQ2 sensor value
String getAirQuality(int value) {
  if (value < 185) return "Very Good";
  else if (value < 190) return "Good";
  else if (value < 195) return "Bad";
  else return "Very Bad";
}
