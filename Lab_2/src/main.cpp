#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#define pdMS_TO_TICKS(xTimeInMs) ((TickType_t)(((unsigned long)(xTimeInMs) * (TickType_t)configTICK_RATE_HZ) / (TickType_t)1000))

struct ledParams
{
    int led1Alpha;
    int led2Alpha;
};

char incomingByte;
int answer = 0;
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
            incomingByte = Serial.read();
            answer = incomingByte - '0';
            Serial.println(answer);
            if (answer == 1)
            {
                xTaskCreate(ledTask1, "LedTask1", 128, (void *)leds, 1, &ledTask1Handle);
            }
            else if (answer == 2)
            {
                xTaskCreate(ledTask2, "LedTask2", 128, (void *)leds, 2, &ledTask2Handle);
            }
            else if (answer < 0)
            {
                analogWrite(10, 0);
                analogWrite(11, 0);
                if (ledTask1Handle)
                {
                    vTaskDelete(ledTask1Handle);
                }
                else if (ledTask2Handle)
                {
                    vTaskDelete(ledTask2Handle);
                }
            }
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