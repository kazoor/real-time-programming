#include <Arduino.h>
#include <math.h>

int counter = 0;
int buttonState = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(11, OUTPUT);  
  pinMode(12, INPUT_PULLUP);
}

void loop() {

  buttonState = digitalRead(12);

  buttonState = digitalRead(12);
 
  
  if (buttonState == LOW) // light the LED
  {
    counter++;
    delay(150);
    Serial.println("Pressed");
  }

  if (counter == 0)
  {
    digitalWrite(11, LOW);
  }
  
  else if (counter == 1)
  {
    digitalWrite(11, HIGH);
  }

  else if (counter == 2)
  {
    digitalWrite(11, LOW);
  }

  else if (counter == 3)
  {
    digitalWrite(11, LOW);
  }

  else
  {
    counter = 0;
  }

   Serial.println(counter);
}