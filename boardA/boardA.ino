#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

// Set the PORT for the web server
ESP8266WebServer server(80);

// The WiFi details 
const char* ssid = "Sweethome38";
const char* password =  "ernyse2004"; 

const int photoSensor = A0;        // Light sensor
int photoValue = 0;
int photoThreshold = 50;            // Threshold for light

const int humidityPin = D6;        // DHT sensor pin
int temperature = 0;
int humidity = 0;
DHT dht(humidityPin, DHT11);       // Initialize DHT sensor with DHT11 model

const int trigPin  = D4;
const int echoPin = D8;

long duration = 0;
int distance = 0;

void setup() {
  //Connect to the WiFi network
  WiFi.begin(ssid, password); 

  pinMode(photoSensor, INPUT);


  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  Serial.begin(9600);              // Start serial communication at 9600 baud rate
  dht.begin();                     // Initialize DHT sensor

    // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {  
      delay(500);
      Serial.println("Waiting to connect...");
  }

  //Print the board IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  

  server.on("/", get_index); // Get the index page on root route 
  //server.on("/setBuzzerStatus", setBuzzerStatus); // Get the setBuzzer page
  
  server.begin(); //Start the server
  Serial.println("Server listening");
}

void loop() {
  // This will keep the server and serial monitor available 
  Serial.println("Server is running");

  //Handling of incoming client requests
  server.handleClient(); 

  if (isSunny()) {
    Serial.println("Too bright");

  } else {
    Serial.println("Too dim");
  }

  readTempHum();                   // Read temperature and humidity

  distanceCentimeter(); // read distance
}

bool isSunny() {
  photoValue = analogRead(photoSensor);  // Read light sensor value
  Serial.print("Light level: ");
  Serial.println(photoValue);

  if (photoValue > photoThreshold) {
    return true;  // It's sunny
  } else {
    return false; // It's dim
  }
}

void readTempHum() {
  delay(2000); // Wait 2 seconds for the next reading

  temperature = dht.readTemperature();   // Read temperature
  humidity = dht.readHumidity();         // Read humidity

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return; // Skip if the sensor reading is invalid
  }

  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
}

void distanceCentimeter(){
  //clear the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  //set the trigger pin on higher state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  //clears the trigger pin
  digitalWrite(trigPin, LOW);

  //reads the echo pin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // calculate distance in cm
  distance = (duration * 0.034) / 2;

  Serial.print(distance);
  Serial.println("cm");
}

void get_index() {

  String html ="<!DOCTYPE html> <html> ";
  html += "<head><meta http-equiv=\"refresh\" content=\"2\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
  html += "<body> <h1>Home environmental monitor system</h1>";
  html += "<div> <p> <strong> The temperature preference is: ";
  html += temperature;
  html +="</strong> degrees. </p>";
  html += "<div> <p> <strong> The humidity preference is: ";
  html += humidity;
  html +="</strong> %. </p>";
  html += "<div> <p> <strong> The light intensity is: ";
  html += photoValue;
  html +="</strong> lx . </p>";
  html += "<div> <p> <strong> The water level is: ";
  html += distance;
  html +="</strong> cm. </p>";
  html +="</body> </html>";
  
  //Print a welcoming message on the index page
  server.send(200, "text/html", html);
  
}
