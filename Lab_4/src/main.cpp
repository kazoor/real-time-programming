#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <timers.h>
#include <stdlib.h>

TimerHandle_t handle;
static int pins[] = {3, 5, 6, 9, 10, 11};
static int size = sizeof(pins) / sizeof(pins[0]);
// Enum used for controlling modes involving different led patterns
enum ledMode
{
    OFF,
    FORWARD,
    BOTH
};

static ledMode mode = FORWARD;

// Handles the different led patterns based on which mode is selected by input button
void ledTask()
{
   for (;;)
    {
        if (mode == FORWARD)
        { // Leds shine in a forward sequence going from first to last led.
            for (int i = 0; i < size; i++)
            {
                digitalWrite(pins[i], HIGH);
                vTaskDelay(10);
                digitalWrite(pins[i], LOW);
            }
        }
        else if(mode == BOTH) 
        { // Leds shine back and forth after one another. 
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
        { // Turns off all leds. 
            for (int i = 0; i < size; i++)
            {
                digitalWrite(pins[i], LOW);
            }
        }
    }
}

/// Method to check if an interrupt is being performed and new mode assigned.
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
}