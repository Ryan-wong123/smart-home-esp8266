
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

byte pin_rows[ROW_NUM] = {D5};  // Connect Row 1 to D5 (GPIO14)
byte pin_column[COLUMN_NUM] = {D6, D7, D8};  // Connect columns to D1, D2, D3

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// WiFi credentials
const char* ssid = "Sweethome38";
const char* password = "ernyse2004";

void setup() {
  myServo.attach(D2);  // Attach the servo to the D2 pin (GPIO4) of NodeMCU

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
  // myServo.write(0);     // Move the servo to 0 degrees
  // delay(1000);          // Wait for 1 second
  // myServo.write(90);    // Move the servo to 90 degrees
  // delay(1000);          // Wait for 1 second
  // myServo.write(180);   // Move the servo to 180 degrees
  // delay(1000);          // Wait for 1 second
  
  // tone(buzzerPin, 1000);         // Turn on buzzer
  // delay(1000);          // Wait for 1 second
  // noTone(buzzerPin);  
  delay(50); // Small delay for better key detection

  getKeypress();


}

void getKeypress() {
  char key = keypad.getKey(); // Check if a key is pressed

  if (key) {
    Serial.println(key);

    // Control the servo motor or get data from the server
    if (key == '1') {
      fetchData("temperature");
    } else if (key == '2') {
      fetchData("humidity");
    } else if (key == '3') {
      fetchData("light");
    }
  }
}
void fetchData(const String &parameter) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;  // Create a WiFiClient object

    // Replace <First_NodeMCU_IP> with the actual IP of the first NodeMCU
    String serverUrl = "http://192.168.1.251/";
    http.begin(client, serverUrl); // Use WiFiClient object with begin()

    int httpCode = http.GET(); // Make the GET request

    if (httpCode > 0) { // Check for a valid response
      String payload = http.getString(); // Get the response as a string
      Serial.println("Response from server:");
      Serial.println(payload);

      if (parameter == "temperature") {
        extractAndPerformAction(payload, "The temperature preference is: ", " degrees");
      } else if (parameter == "humidity") {
        extractAndPerformAction(payload, "The humidity preference is: ", " %");
      } else if (parameter == "light") {
        extractAndPerformAction(payload, "The light intensity is: ", " lx");
      }
    } else {
      Serial.println("Error in HTTP request");
    }
    http.end(); // End the HTTP connection
  }
}


void extractAndPerformAction(const String &payload, const String &startToken, const String &endToken) {
  int startIndex = payload.indexOf(startToken);
  if (startIndex != -1) {
    startIndex += startToken.length();
    int endIndex = payload.indexOf(endToken, startIndex);
    String valueStr = payload.substring(startIndex, endIndex);
    Serial.print("Extracted value: ");
    Serial.println(valueStr);

    // Example actions based on the value
    if (startToken == "The temperature preference is: ") {
      float temperature = valueStr.toFloat();
      if (temperature > 30) {
        myServo.write(90); // Example action for temperature
      } else {
        myServo.write(0);
      }
    } else if (startToken == "The humidity preference is: ") {
      int humidity = valueStr.toInt();
      if (humidity > 70) {
        digitalWrite(buzzerPin, HIGH); // Turn on buzzer
      } else {
        digitalWrite(buzzerPin, LOW); // Turn off buzzer
      }
    }
  } else {
    Serial.println("Error: Data not found in response.");
  }
}