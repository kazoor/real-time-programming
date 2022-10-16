#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <timers.h>
#include <stdlib.h>
#include <IRremote.h>

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

// distance sensor HC-SR04 HY-SRF05
#define TRIG_PIN 13 // red
#define ECHO_PIN 12 // orange

// IR sensor
#define Signal_PIN 11 // green

long duration = 0, cm = 0;
IRrecv irrecv(Signal_PIN);
decode_results results;

void distanceSensorTask(void *param)
{
    for (;;)
    {
        long distance;
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(5);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);

        pinMode(ECHO_PIN, INPUT);
        duration = pulseIn(ECHO_PIN, HIGH);

        cm = (duration / 2) / 29.1;

        if (cm <= 10)
        {
            Serial.println(cm);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void irSensorTask(void *param)
{
    for (;;)
    {
        if (irrecv.decode(&results))
        {
            translateIR();
            irrecv.resume();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void setup()
{
    Serial.begin(9600);
    irrecv.enableIRIn();
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    irrecv.enableIRIn();

    xTaskCreate(distanceSensorTask, "Distance_sensor", 100, nullptr, 2, NULL);
    xTaskCreate(irSensorTask, "IR_sensor", 100, nullptr, 1, NULL);
}

void loop()
{
}

void translateIR()
{
    switch (results.value)
    {
    case POWER:
        Serial.println("POWER");
        break;

    case UP:
        Serial.println("UP");
        break;

    case DOWN:
        Serial.println("DOWN");
        break;

    case SELECT:
        Serial.println("SELECT");
        break;

    case LEFT:
        Serial.println("LEFT");
        break;
    
    case RIGHT:
        Serial.println("RIGHT");
        break;
    
    default:
        break;
    }

    delay(100); // to not get immediate repeat
}