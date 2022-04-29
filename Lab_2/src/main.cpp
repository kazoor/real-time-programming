#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#define pdMS_TO_TICKS(xTimeInMs) ((TickType_t)(((unsigned long)(xTimeInMs) * (TickType_t)configTICK_RATE_HZ) / (TickType_t)1000))
#define MAX_LED_ALPHA 255
/*
    3 tasks with period set to 100ms and priorities and task 3 < task 2
    t1 & t2 will control LED's using the PMW (lab1).
    t3 will read command frmo serial port. serial sent to 1 or 2 based on cmd
    each of the global vars are accesed by 2 taks and MUST be protected using synchronization
    clear it (set to -1) if there is no command meaning -1 is should ignore cmd
    program can only hold 1 cmd.

    n.m
    n = task number (t1, t2)
    m = brightnes of LED controlled by task n
    example: if command 1.25 = shining 25%.
    depending on n, t3 delivers cmd to t1 or t2. LED continues with same state
    until it is changed.
*/

struct ledParams
{
    int led1Alpha;
    int led2Alpha;
};

char incomingByte;
float inputValue = 0;
int incomingCommand = 0;

TaskHandle_t ledTask1Handle;
TaskHandle_t ledTask2Handle;

void ledTask1(void *param)
{
    ledParams *led = (ledParams *)param;
    for (;;)
    {
        analogWrite(10, 255);
        Serial.println("ledTask1 called");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void ledTask2(void *param)
{
    for (;;)
    {
        analogWrite(11, 255);
        Serial.println("ledTask2 called");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void inputTask(void *param)
{
    ledParams *leds = (ledParams *)param;
    for (;;)
    {
        if (Serial.available() > 0)
        {
            Serial.print("Input: ");
            inputValue = Serial.parseFloat();
            Serial.println(inputValue);

            int whole = (int)inputValue;
            Serial.print("Whole:  ");
            Serial.print(whole);
            Serial.print("\n");

            float decimal = inputValue - whole;
            Serial.print("Decimal: ");
            Serial.print(decimal);
            Serial.print("\n");

            int calculatedAlpha = MAX_LED_ALPHA * decimal;
            Serial.println(calculatedAlpha);

            Serial.println("----------------------------");

            // 0.25
            // Max == 255
            // -> 1.25
            // led = 1, alpha = max * 0.25
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(11, OUTPUT);
    pinMode(10, OUTPUT);

    ledParams *leds = new ledParams();

    // xTaskCreate(ledTask1, "LedTask1", 128, (void *)params, 1, NULL);
    // xTaskCreate(ledTask2, "LedTask2", 128, NULL, 2, NULL);
    xTaskCreate(inputTask, "inputTask", 128, (void *)leds, 0, NULL);
    // Queue = xQueueCreateSet();
}

void loop()
{
}