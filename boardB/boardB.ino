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

void setup() {
  myServo.attach(D2);  // Attach the servo to the D2 pin (GPIO4) of NodeMCU

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  Serial.begin(9600);
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


void getKeypress(){
  char key = keypad.getKey(); // Check if a key is pressed

  if (key) {
    // If a key is pressed, print the key value
    Serial.println(key);

    // Control the servo motor based on the key press
    if (key == '1') {
      myServo.write(0);   // Move to 0 degrees
    } 
    else if (key == '2') {
      myServo.write(90);  // Move to 90 degrees
    } 
    else if (key == '3') {
      myServo.write(180); // Move to 180 degrees
    }
  }
}