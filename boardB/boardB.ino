#include <Servo.h>

Servo myServo;  // Create a Servo object to control a servo motor

const int buzzerPin = D1;          // Buzzer pin

void setup() {
  myServo.attach(D2);  // Attach the servo to the D2 pin (GPIO4) of NodeMCU

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

}

void loop() {
  myServo.write(0);     // Move the servo to 0 degrees
  delay(1000);          // Wait for 1 second
  myServo.write(90);    // Move the servo to 90 degrees
  delay(1000);          // Wait for 1 second
  myServo.write(180);   // Move the servo to 180 degrees
  delay(1000);          // Wait for 1 second
  
  tone(buzzerPin, 1000);         // Turn on buzzer
    delay(1000);          // Wait for 1 second
    noTone(buzzerPin);  
}
