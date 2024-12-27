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
bool waterTooHigh = false;

//Wifi ssid and password (replace with own wifi ssid and password)
const char* ssid = "Sweethome38";
const char* password = "ernyse2004";

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
      if(isTooHot || isTooHumid || isTooBright || waterTooHigh ){
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

    //Get the temperature reading
    GetTemperatureReading(payload);
    //Get the light intensity reading
    GetLightReading(payload);
    //Get humidity reading
    GetHumidityReading(payload);
    //Get distance reading
    GetWaterLevelReading(payload);
    Serial.println("------------------------------------");
  } else {
    Serial.println("Error fetching code");
  }
  http.end();
}

//Function to get temperature reading
void GetTemperatureReading(const String &payload) {
  //Set the start of the token position
  String startToken = "The temperature level is: ";
  //Set the ending token position
  String endToken = " degrees";

  //Get the index position of the start token
  int startIndex = payload.indexOf(startToken);

  //Check if starting token is available
  if (startIndex != -1) {
    //Move the index to the end of the start token
    startIndex += startToken.length();
    //Get the index position of the end token
    int endIndex = payload.indexOf(endToken, startIndex);
    //Get the temperature reading
    int temperature = payload.substring(startIndex, endIndex).toInt();

    //Check if the temperature reading has exceeded the threshold
    if (temperature > temperatureThreshold) {
      //Set the isTooHot boolean to true to activate the feedback
      isTooHot = true;
      Serial.println("Temperature: " + String(temperature) + " is too hot");
    } else {
      //Set the isTooHot boolean to false
      isTooHot = false;
      Serial.println("Temperature: " + String(temperature) + " is normal");
    }
  } else {
    Serial.println("Unable to get temperature");
  }
}

//Function to get light reading
void GetLightReading(const String &payload) {
  //Set the start of the token position
  String startToken = "The light intensity is: ";
  //Set the ending token position
  String endToken = " lx";

  //Get the index position of the start token
  int startIndex = payload.indexOf(startToken);

  //Check if starting token is available
  if (startIndex != -1) {
    //Move the index to the end of the start token
    startIndex += startToken.length();
    //Get the index position of the end token
    int endIndex = payload.indexOf(endToken, startIndex);
    //Get the light reading
    int lightIntensity = payload.substring(startIndex, endIndex).toInt();

    //Check if the light reading has exceeded the threshold
    if (lightIntensity > lightThreshold) {
      //Rotate the servo motor to 90 degrees to provide shelter
      myServo.write(90);
      //Set the isTooBright boolean to true to activate the feedback
      isTooBright = true;
      Serial.println("Light intensity: " + String(lightIntensity) + " is too bright");
    } else {
      //Rotate the servo motor to 180 degrees back to normal
      myServo.write(180); 
      //Set the isTooBright boolean to false 
      isTooBright = false;
      Serial.println("Light intensity: " + String(lightIntensity) + " is normal");
    }
  } else {
    Serial.println("Unable to get light reading");
  }
}

//Function to get humidity reading
void GetHumidityReading(const String &payload) {
  //Set the start of the token position
  String startToken = "The humidity level is: ";
  //Set the ending token position
  String endToken = " %";

  //Get the index position of the start token
  int startIndex = payload.indexOf(startToken);

  //Check if starting token is available
  if (startIndex != -1) {
    //Move the index to the end of the start token
    startIndex += startToken.length();
    //Get the index position of the end token
    int endIndex = payload.indexOf(endToken, startIndex);
    //Get the humidity reading
    int humidity = payload.substring(startIndex, endIndex).toInt();

    //Check if the humidity reading has exceeded the threshold
    if (humidity > humidityThreshold) { 
      //Set the isTooHumid boolean to true to activate the feedback
      isTooHumid = true;
      Serial.println("Humidity: " + String(humidity) + " is too humid");
    } else {
      //Set the isTooHumid boolean to false
      isTooHumid = false;
      Serial.println("Humidity: " + String(humidity) + " not humid");
    }
  } else {
    Serial.println("Unable to get humidity reading");
  }
}

//Function to get distance reading
void GetWaterLevelReading(const String &payload) {
  //Set the start of the token position
  String startToken = "The water level is: ";
  //Set the ending token position
  String endToken = " cm";

  //Get the index position of the start token
  int startIndex = payload.indexOf(startToken);

  //Check if starting token is available
  if (startIndex != -1) {
    //Move the index to the end of the start token
    startIndex += startToken.length();
    //Get the index position of the end token
    int endIndex = payload.indexOf(endToken, startIndex);
    //Get the distance reading
    int waterLevel = payload.substring(startIndex, endIndex).toInt();

    //Check if the distance reading has exceeded the threshold
    if (waterLevel < waterLevelThreshold) {
      //Set the waterTooHigh boolean to true to activate the feedback
      waterTooHigh = true;
      Serial.println("Water level: " + String(waterLevel) + " is too high");
    } else {
      //Set the waterTooHigh boolean to false
      waterTooHigh = false;
      Serial.println("Water level: " + String(waterLevel) + " is normal");
    }
  } else {
    Serial.println("Unable to get distance reading");
  }
}

