#include "DHT.h"

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
  pinMode(photoSensor, INPUT);


  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  Serial.begin(9600);              // Start serial communication at 9600 baud rate
  dht.begin();                     // Initialize DHT sensor
}

void loop() {
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
