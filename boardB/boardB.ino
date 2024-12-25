
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>
#include <Keypad.h>
Servo myServo;  // Create a Servo object to control a servo motor

const int buzzerPin = D1;          // Buzzer pin

// Define the keypad matrix for 1x3 keypad
const byte ROW_NUM    = 1; // One row
const byte COLUMN_NUM = 3; // Three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'}  // The 3 keys in the first row
};

byte pin_rows[ROW_NUM] = {D5};  // Connect Row 1 to D5 
byte pin_column[COLUMN_NUM] = {D6, D7, D8};  // Connect columns to D6,D7,D8

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

const float temperatureThreshold = 30.0;  // Threshold for temperature
const int lightThreshold = 50;            // Threshold for light intensity
const int waterLevelThreshold = 50;   

// WiFi credentials
const char* ssid = "Sweethome38";
const char* password = "ernyse2004";
String payload ="";
void setup() {
  myServo.attach(D2);  // Attach the servo to the D2 pin (GPIO4) of NodeMCU
  myServo.write(0);    // Initialize servo to 0 position

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
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
    getKeypress();

  } else {
    Serial.println("WiFi not connected!");
  }
  delay(2000); // Wait for 2 seconds before next reading


}

void getKeypress() {
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);
    if (key == '1') {
      handleTemperature(payload);
    } else if (key == '2') {
      handleHumidity(payload);
    } else if (key == '3') {
      handleLight(payload);
    }
  }
}

void fetchData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;  // Create a WiFiClient object

    // Replace <First_NodeMCU_IP> with the actual IP of the first NodeMCU
    String serverUrl = "http://192.168.1.251/";
    http.begin(client, serverUrl); // Use WiFiClient object with begin()

    int httpCode = http.GET(); // Make the GET request

    if (httpCode > 0) { // Check for a valid response
      payload = http.getString(); // Get the response as a string
      // Serial.println("Response from server:");
      // Serial.println(payload);

      // Extract and act on the data
      handleTemperature(payload);
      handleLight(payload);
      handleHumidity(payload);
      handleWaterLevel(payload);
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
    Serial.print("Extracted temperature: ");
    Serial.println(temperature);

    if (temperature > temperatureThreshold) {
      digitalWrite(buzzerPin, HIGH); // Turn on buzzer
    } else {
      digitalWrite(buzzerPin, LOW);  // Turn off buzzer
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
    Serial.print("Extracted light intensity: ");
    Serial.println(lightIntensity);

    if (lightIntensity > lightThreshold) {
      myServo.write(90); // Rotate servo to 90 degrees
      Serial.println("too bright");
    } else {
      myServo.write(180);  // Rotate servo to 0 degrees
      Serial.println("too dark");
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
    Serial.print("Extracted humidity: ");
    Serial.println(humidity);

    if (humidity > 70.0) { // Example threshold for high humidity
      digitalWrite(buzzerPin, HIGH); // Turn on buzzer
    } else {
      digitalWrite(buzzerPin, LOW);  // Turn off buzzer
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
    Serial.print("Extracted water level: ");
    Serial.println(waterLevel);

    if (waterLevel < waterLevelThreshold) { // Example threshold for low water level
      digitalWrite(buzzerPin, HIGH); // Turn on buzzer
    } else {
      digitalWrite(buzzerPin, LOW);  // Turn off buzzer
    }
  } else {
    Serial.println("Water level data not found in response.");
  }
}

