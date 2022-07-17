#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#define pdMS_TO_TICKS(xTimeInMs) ((TickType_t)(((unsigned long)(xTimeInMs) * (TickType_t)configTICK_RATE_HZ) / (TickType_t)1000))
#define MAX_LED_ALPHA 255

struct ledParams
{
    int n;
    int led1Alpha;
    int led2Alpha;
};

float inputValue = 0;
SemaphoreHandle_t mtx;

void ledTask1(void *param)
{
    ledParams *led = (ledParams *)param;
    for (;;)
    {
        xSemaphoreTake(mtx, portMAX_DELAY);
        if (led->n == 1)
        {
            analogWrite(10, led->led1Alpha);
            Serial.println("ledTask1 called");
            led->n = -1;
        }
        xSemaphoreGive(mtx);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void ledTask2(void *param)
{
    ledParams *led = (ledParams *)param;
    for (;;)
    {
        xSemaphoreTake(mtx, portMAX_DELAY);
        if (led->n == 2)
        {
            analogWrite(11, led->led2Alpha);
            Serial.println("ledTask2 called");
            led->n = -1;
        }
        xSemaphoreGive(mtx);
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

            leds->n = (int)inputValue;

            float decimal = inputValue - leds->n;

            xSemaphoreTake(mtx, portMAX_DELAY);
            if (leds->n == 1)
            {
                leds->led1Alpha = MAX_LED_ALPHA * decimal;
            }
            else if (leds->n == 2)
            {
                leds->led2Alpha = MAX_LED_ALPHA * decimal;
            }
            xSemaphoreGive(mtx);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(11, OUTPUT);
    pinMode(10, OUTPUT);

    // Global led params used by all tasks.
    ledParams *leds = new ledParams();

    // Create the global mutex for ledparam struct.
    mtx = xSemaphoreCreateMutex();

    xTaskCreate(ledTask1, "LedTask1", 128, (void *)leds, 1, NULL);
    xTaskCreate(ledTask2, "LedTask2", 128, (void *)leds, 2, NULL);
    xTaskCreate(inputTask, "inputTask", 128, (void *)leds, 0, NULL);
}

void loop()
{
    // Not used.
}
