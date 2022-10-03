#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <timers.h>
#include <stdlib.h>

TimerHandle_t handle;
static int pins[] = {3, 5, 6, 9, 10, 11};
static int size = sizeof(pins) / sizeof(pins[0]);

void ledTask()
{
    for(;;)
    {
        for(int i = 0; i < size; i++)
        {
            digitalWrite(pins[i], HIGH);
            vTaskDelay(10);
            digitalWrite(pins[i], LOW);
        }

        for(int i = size - 1; i >= 0; i--)
        {
            digitalWrite(pins[i], HIGH);
            vTaskDelay(10);
            digitalWrite(pins[i], LOW);
        }
    }
}

void setup()
{
    Serial.begin(9600);

    for(int i = 0; i < size; i++)
    {
        pinMode(pins[i], OUTPUT);
    }
    
    handle = xTimerCreate((char*)"OneShot", 10 , pdTRUE, 0, (TimerCallbackFunction_t)ledTask);

    xTimerStart(handle, 0);
}

void loop()
{
    /*
    int value = digitalRead(13);
    print(value);
    for (int i = 0; i < size; i++)
    {
        digitalWrite(numb[i], HIGH);
        delay(20);
        digitalWrite(numb[i], LOW);
        delay(20);
    }

    for (int i = size - 1; i >= 0; i--)
    {
        digitalWrite(numb[i], HIGH);
        delay(20);
        digitalWrite(numb[i], LOW);
        delay(20);
    }*/
    //digitalWrite(11, HIGH);   // turn the LED on (HIGH is the voltage level)
    //delay(1000);                       // wait for a second
    //digitalWrite(11, LOW);    // turn the LED off by making the voltage LOW
    //delay(1000);                       // wait for a second
}