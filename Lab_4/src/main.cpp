#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <timers.h>
#include <stdlib.h>

TimerHandle_t handle;
static int pins[] = {3, 5, 6, 9, 10, 11};
static int size = sizeof(pins) / sizeof(pins[0]);
enum ledMode
{
    OFF,
    FORWARD,
    BOTH
};

static ledMode mode = FORWARD;

void ledTask()
{
   for (;;)
    {
        if (mode == FORWARD)
        {
            for (int i = 0; i < size; i++)
            {
                digitalWrite(pins[i], HIGH);
                vTaskDelay(10);
                digitalWrite(pins[i], LOW);
            }
        }
        else if(mode == BOTH) 
        {
            for (int i = 0; i < size; i++)
            {
                digitalWrite(pins[i], HIGH);
                vTaskDelay(10);
                digitalWrite(pins[i], LOW);
            }

            for (int i = size - 1; i >= 0; i--)
            {
                digitalWrite(pins[i], HIGH);
                vTaskDelay(10);
                digitalWrite(pins[i], LOW);
            }
        }
        else 
        {
            for (int i = 0; i < size; i++)
            {
                digitalWrite(pins[i], LOW);
            }
        }
    }
}

void ledTask2()
{
}
/// Method to check if an interrupt is being performed and we discovered that the arduino gets an input 
/// even when we do not press the buttons
void testInterrup()
{
    mode = mode == FORWARD ? BOTH : FORWARD;
    Serial.println("INTERRUPTED");
}

void setup()
{
    Serial.begin(9600);

    for (int i = 0; i < size; i++)
    {
        pinMode(pins[i], OUTPUT);
    }

    handle = xTimerCreate((char *)"OneShot", 10, pdTRUE, 0, (TimerCallbackFunction_t)ledTask);

    xTimerStart(handle, 0);

    attachInterrupt(digitalPinToInterrupt(2), testInterrup, RISING);
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
    // digitalWrite(11, HIGH);   // turn the LED on (HIGH is the voltage level)
    // delay(1000);                       // wait for a second
    // digitalWrite(11, LOW);    // turn the LED off by making the voltage LOW
    // delay(1000);                       // wait for a second
}