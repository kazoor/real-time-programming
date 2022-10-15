#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <timers.h>
#include <stdlib.h>
// Distande sensor
// IR remote

/*
IRrecv receiver(RECV_PIN); Where RECV_PIN is the digital pin number on Arduino. 

Then start the reciever: receiver.enableIRIn(); 
*/
#define POWER 0x00FF629D
#define A 0x00FF22DD
#define B 0x00FF02FD
#define C 0x00FFC23D
#define UP 0x00FF9867
#define DOWN 0x00FF38C7
#define LEFT 0x00FF30CF
#define RIGHT 0x00FF7A85
#define SELECT 0x00FF18E7

void setup()
{

}

void loop()
{
    
}