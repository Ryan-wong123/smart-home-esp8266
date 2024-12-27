//ESP8266 wifi library
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//DHT11 library
#include <DHT.h>
#include <DHT_U.h>

//Initialise webserver port number
ESP8266WebServer server(80);

//Wifi ssid and password (replace with own wifi ssid and password)
const char* ssid = "<Wifi_SSID>";
const char* password =  "<Wifi_password>"; 

//Initialise Light sensor pin
const int lightSensor = A0;     
//Set light intensity reading to 0 as default
int lightIntensity = 0;

//Initialise the trigger pin of ultrasonic sensor
const int triggerPin  = D4;
//Initialise the echo pin of the ultrasonic sensor
const int echoPin = D8;
//Set the duration taken to get the ultrasonic wave from the echo pin
long durationTakenForDistance = 0;
//Set the distance reading from the ultrasonic sensor to 0 as default
int distance = 0;

//Initialise Humidity pin
const int humidityPin = D6;       
//Set temperature and humidity readings to 0 as default
int temperature = 0;
int humidity = 0;
//Initialise the DHT11
DHT dht(humidityPin, DHT11);      

void setup() {
  //Connect to the wifi
  WiFi.begin(ssid, password); 

  //Set the light sensor pin as input
  pinMode(lightSensor, INPUT);

  //Set the trigger pin as output to emit ultrasonic wave
  pinMode(triggerPin,OUTPUT);
  //Set the echo pin as the input to reach the ultrasonic wave reading
  pinMode(echoPin,INPUT);

  //Set the baud rate as 9600 for serial monitor
  Serial.begin(9600);             
  
  //Start the DHT11 sensor
  dht.begin();                    

  //Try to reconnect to wifi
  while (WiFi.status() != WL_CONNECTED) {  
      delay(500);
      Serial.println("Connecting");
  }

  //Print the IP address of the connected wifi
  Serial.print("Connected to : ");
  Serial.println(WiFi.localIP());  

  //set the route to to fetch data to web server
  server.on("/", fetchData); 
  
  //Start the server
  server.begin(); 
}

void loop() {
  //Fetch data to and from server
  server.handleClient(); 

  //Monitor light reading
  GetLightReading();
  //Monitor ultrasonic reading                  
  GetDistanceReading();
  //Monitor Temperature and humidity reading
  GetTemperatureAndHumidityReading();  
}

//Function to get light reading
void GetLightReading() {
  //Store the light reading
  lightIntensity = analogRead(lightSensor); 
  //Print output from light sensor to serial monitor
  Serial.println("Light level: " + String(lightIntensity));
}

//Function to get the distance reading from ultrasonic sensor
void GetDistanceReading(){
  //Set trigger pin to default state and wait for 2 microseconds to reset the trigger pin
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  //Emit ultrasonic wave from trigger pin and wait for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);

  //Stop the emission of ultrasonic wave
  digitalWrite(triggerPin, LOW);

  //Get the ultrasonic wave from echo pin
  durationTakenForDistance = pulseIn(echoPin, HIGH);

  //Calculate the distance between ultrasonic sensor and target object
  distance = (durationTakenForDistance * 0.034) / 2;

  //Print the output distance in the serial monitor
  Serial.println("Distance: " + String(distance) + "cm");
}

//Function to get the temperature and humidity reading
void GetTemperatureAndHumidityReading() {
  //Set delay of 2 seconds to get accurate reading
  delay(2000);
  
  //Store the temperature and humidity reading
  temperature = dht.readTemperature();   
  humidity = dht.readHumidity();        

  //Check if the reading from DHT11 is accurate
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to get correct reading from DHT11");
    return;
  }
  //Print output from DHT11 sensor to serial monitor
  Serial.println("Temperature: "+ String(temperature));
  Serial.println("Humidity: " + String(humidity));
}

//Function to post data to server with readings from all sensors
void fetchData() {

  //Display the readings from sensors in the html page
  String html ="<!DOCTYPE html> <html> ";
  html += "<head><meta http-equiv=\"refresh\" content=\"2\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
  html += "<body> <h1>Home environmental monitor system</h1>";
  html += "<div> <p> <strong> The temperature level is: ";
  html += temperature;
  html +="</strong> degrees. </p>";
  html += "<div> <p> <strong> The humidity level is: ";
  html += humidity;
  html +="</strong> %. </p>";
  html += "<div> <p> <strong> The light intensity is: ";
  html += lightIntensity;
  html +="</strong> lx . </p>";
  html += "<div> <p> <strong> The water level is: ";
  html += distance;
  html +="</strong> cm. </p>";
  html +="</body> </html>";
  
  //Send the data over with 200 response code to server
  server.send(200, "text/html", html);
}
