
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>

Servo myServo;  // Create a Servo object to control a servo motor

const int buzzerPin = D5;          // Buzzer pin

const float temperatureThreshold = 30.0;  // Threshold for temperature
const int lightThreshold = 50;            // Threshold for light intensity
const int waterLevelThreshold = 50;   
const int humidityThreshold = 50; 
bool isTooHot = false;
bool isTooHumid = false;
bool isTooBright = false;
bool waterTooHigh = false;

// WiFi credentials
const char* ssid = "Sweethome38";
const char* password = "ernyse2004";

const int redLedPin = D2;
const int greenLedPin = D3;
const int blueLedPin = D4;

void setup() {
  myServo.attach(D6);  // Attach the servo to the D2 pin (GPIO4) of NodeMCU
  myServo.write(0);    // Initialize servo to 0 position

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  Serial.begin(9600);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
   if (WiFi.status() == WL_CONNECTED) {
    fetchData();
    if(isTooHot || isTooHumid || isTooBright || waterTooHigh ){
        digitalWrite(buzzerPin, HIGH); // Turn on buzzer
        rgbLed(0,0,255);
    }else{
      digitalWrite(buzzerPin, LOW);  // Turn off buzzer
      rgbLed(255,0,0);
    }
  } else {
    Serial.println("WiFi not connected!");
  }
  delay(3000); // Wait for 2 seconds before next reading
}

void rgbLed(int redLedAmount, int greenLedAmount, int blueLedAmount){
  analogWrite(redLedPin,redLedAmount);
  analogWrite(greenLedPin,greenLedAmount);
  analogWrite(blueLedPin,blueLedAmount);
}

void fetchData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;  // Create a WiFiClient object

    // Replace <First_NodeMCU_IP> with the actual IP of the first NodeMCU
    String serverUrl = "http://192.168.1.251/";
    http.begin(client, serverUrl); // Use WiFiClient object with begin()
    http.setTimeout(5000); // Set timeout to 5 seconds

    int httpCode = http.GET(); // Make the GET request

    if (httpCode > 0) { // Check for a valid response
      String payload = http.getString(); // Get the response as a string
      // Serial.println("Response from server:");
      // Serial.println(payload);

      // Extract and act on the data
      handleTemperature(payload);
      handleLight(payload);
      handleHumidity(payload);
      handleWaterLevel(payload);
      Serial.println("------------------------------");
    } else {
      Serial.println("Error in HTTP request");
    }
    http.end(); // End the HTTP connection
  }
}


void handleTemperature(const String &payload) {
  String startToken = "The temperature preference is: ";
  String endToken = " degrees";
  int startIndex = payload.indexOf(startToken);
  if (startIndex != -1) {
    startIndex += startToken.length();
    int endIndex = payload.indexOf(endToken, startIndex);
    String valueStr = payload.substring(startIndex, endIndex);
    float temperature = valueStr.toFloat();
    
    if (temperature > temperatureThreshold) {
      isTooHot = true;
      Serial.println("Extracted temperature: " + String(temperature) + " is too hot");
    } else {
      isTooHot = false;
      Serial.println("Extracted temperature: " + String(temperature) + " is too cold");
    }
  } else {
    Serial.println("Temperature data not found in response.");
  }
}

void handleLight(const String &payload) {
  String startToken = "The light intensity is: ";
  String endToken = " lx";
  int startIndex = payload.indexOf(startToken);
  if (startIndex != -1) {
    startIndex += startToken.length();
    int endIndex = payload.indexOf(endToken, startIndex);
    String valueStr = payload.substring(startIndex, endIndex);
    int lightIntensity = valueStr.toInt();

    if (lightIntensity > lightThreshold) {
      myServo.write(90); // Rotate servo to 90 degrees
      isTooBright = true;
      Serial.println("Extracted light intensity: " + String(lightIntensity) + " is too bright");
    } else {
      myServo.write(180);  // Rotate servo to 0 degrees
      isTooBright = false;
      Serial.println("Extracted light intensity: " + String(lightIntensity) + " is too dark");
    }
  } else {
    Serial.println("Light intensity data not found in response.");
  }
}

void handleHumidity(const String &payload) {
  String startToken = "The humidity level is: ";
  String endToken = " %";
  int startIndex = payload.indexOf(startToken);
  if (startIndex != -1) {
    startIndex += startToken.length();
    int endIndex = payload.indexOf(endToken, startIndex);
    String valueStr = payload.substring(startIndex, endIndex);
    float humidity = valueStr.toFloat();

    if (humidity > humidityThreshold) { // Example threshold for high humidity
      isTooHumid = true;
      Serial.println("Extracted humidity: " + String(humidity) + " is too humid");
    } else {
      isTooHumid = false;
      Serial.println("Extracted humidity: " + String(humidity) + " not humid");
    }
  } else {
    Serial.println("Humidity data not found in response.");
  }
}

void handleWaterLevel(const String &payload) {
  String startToken = "The water level is: ";
  String endToken = " cm";
  int startIndex = payload.indexOf(startToken);
  if (startIndex != -1) {
    startIndex += startToken.length();
    int endIndex = payload.indexOf(endToken, startIndex);
    String valueStr = payload.substring(startIndex, endIndex);
    float waterLevel = valueStr.toFloat();

    if (waterLevel < waterLevelThreshold) { // Example threshold for low water level
      waterTooHigh = true;
      Serial.println("Extracted water level: " + String(waterLevel) + " is too high");
    } else {
      waterTooHigh = false;
      Serial.println("Extracted water level: " + String(waterLevel) + " is too low");
    }
  } else {
    Serial.println("Water level data not found in response.");
  }
}

