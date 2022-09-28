#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <time.h>
#include <stdlib.h>

void setup()
{
    Serial.begin(9600);
}

void loop(){
    digitalWrite(11, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(11, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);                       // wait for a second
}