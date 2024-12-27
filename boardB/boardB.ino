//ESP8266 Library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//Servo motor library
#include <Servo.h>

//Initialise servo library
Servo myServo; 

//Initialise buzzer pin
const int buzzerPin = D5;

//Set the threshold levels for the sensors
const int temperatureThreshold = 30;
const int humidityThreshold = 50; 
const int lightThreshold = 200;
const int waterLevelThreshold = 20;

//Set all flags for sensor readings
bool isTooHot = false;
bool isTooHumid = false;
bool isTooBright = false;
bool isWaterTooHigh = false;

//Wifi ssid and password (replace with own wifi ssid and password)
const char* ssid = "<wifi_SSID>";
const char* password = "<Wifi_password>";

//Set the pins for the RGB LED
const int redLedPin = D2;
const int greenLedPin = D3;
const int blueLedPin = D4;

void setup() {
  //Initialise the servo motor
  myServo.attach(D6); 
  //Set the servo motor to rotate the original position
  myServo.write(0);

  //Set the buzzer as an output
  pinMode(buzzerPin, OUTPUT);
  //Set the buzzer to silent by default
  digitalWrite(buzzerPin, LOW);

  //Set all the RGB lights as output
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  
  //Set the baud rate as 9600 for serial monitor
  Serial.begin(9600);

  //Connect to the wifi
  WiFi.begin(ssid, password);

  //Try to reconnect to wifi
  while (WiFi.status() != WL_CONNECTED) {  
      delay(500);
      Serial.println("Connecting");
  }

  //Print the IP address of the connected wifi
  Serial.print("Connected to : ");
  Serial.println(WiFi.localIP());  
}

void loop() {
    //Check if connected to wifi
    if (WiFi.status() == WL_CONNECTED) {
      //Fetch the readings data from the sensor
      fetchData();
      
      //Give audio and visual feedback if any of the readings from the sensor exceed the respective threshold level
      if(isTooHot || isTooHumid || isTooBright || isWaterTooHigh ){
          //Beep the buzzer
          digitalWrite(buzzerPin, HIGH);
          //Set the rgb led to blue color
          rgbLed(0,0,255);
      }else{
        //Mute the buzzer
        digitalWrite(buzzerPin, LOW);
        //Set the rgb led to green
        rgbLed(255,0,0);
      }
    } else {
      Serial.println("No connection");
    }
    delay(3000);
}

//Function to set the values for respective led color
void rgbLed(int redLedValue, int greenLedValue, int blueLedValue){
  analogWrite(redLedPin,redLedValue);
  analogWrite(greenLedPin,greenLedValue);
  analogWrite(blueLedPin,blueLedValue);
}

//Function to fetch data readings from the server
void fetchData() {
  //Initialise the http and wifi client
  HTTPClient http;
  WiFiClient client;

  //URL link to connect to the server (replace with master board IP address)
  String serverUrl = "http://192.168.1.251/";
  //Connect to the server
  http.begin(client, serverUrl);
  //Set timeout to 5 seconds for the server 
  http.setTimeout(5000);

  //Get the response code from the server
  int httpCode = http.GET(); 

  //If response code is successful then fetch the data 
  if (httpCode > 0) { 
    //Get all data readings from the server
    String payload = http.getString();

    
    //Get individual readings
    int temperature = ExtractReading(payload, "The temperature level is: ", " degrees");
    int lightIntensity = ExtractReading(payload, "The light intensity is: ", " lx");
    int humidity = ExtractReading(payload, "The humidity level is: ", " %");
    int waterLevel = ExtractReading(payload, "The water level is: ", " cm");

    //Check if the temperature is over the threshold value
    if (temperature > temperatureThreshold) {
      //Set the isTooHot boolean to true to activate feedback
      isTooHot = true;
      Serial.println("Temperature: " + String(temperature) + " is too hot");
    } else {
      //Set the isTooHot boolean to false to off feedback
      isTooHot = false;
      Serial.println("Temperature: " + String(temperature) + " is normal");
    }

    //Check if the light intensity is over the threshold value
    if (lightIntensity > lightThreshold) {
      //Rotate the servo motor to 90 degrees to activate shelter
      myServo.write(90); 
      //Set the isTooBright boolean to true to activate feedback
      isTooBright = true;
      Serial.println("Light intensity: " + String(lightIntensity) + " is too bright");
    } else {
      //Rotate the servo motor to 180 degrees back to normal
      myServo.write(180); 
      //Set the isTooBright boolean to false to off feedback
      isTooBright = false;
      Serial.println("Light intensity: " + String(lightIntensity) + " is normal");
    }

    //Check if the humidity is over the threshold value
    if (humidity > humidityThreshold) {
      //Set the isTooHumid boolean to true to activate feedback
      isTooHumid = true;
      Serial.println("Humidity: " + String(humidity) + " is too humid");
    } else {
      //Set the isTooHumid boolean to false to off feedback
      isTooHumid = false;
      Serial.println("Humidity: " + String(humidity) + " is normal");
    }

    //Check if the water level is over the threshold value
    if (waterLevel < waterLevelThreshold) {
      //Set the isWaterTooHigh boolean to true to activate feedback
      isWaterTooHigh = true;
      Serial.println("Water level: " + String(waterLevel) + " is too high");
    } else {
      //Set the isWaterTooHigh boolean to false to off feedback
      isWaterTooHigh = false;
      Serial.println("Water level: " + String(waterLevel) + " is normal");
    }
    Serial.println("------------------------------------");
  } else {
    Serial.println("Error fetching data");
  }
  http.end();
}

//Function to extract the reading value only
int ExtractReading(const String &payload, const String &startToken, const String &endToken) {
  //Set the start of the token position
  int startIndex = payload.indexOf(startToken);

  //Check if starting token is available
  if (startIndex != -1) {
    //Move the index to the end of the start token
    startIndex += startToken.length();
    //Get the index position of the end token
    int endIndex = payload.indexOf(endToken, startIndex);

    //Return the reading value
    return payload.substring(startIndex, endIndex).toInt();
  } else {
    //Return -1 to show unable to get reading
    Serial.println("Reading not available");
    return -1;
  }
}