#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

struct ledParams
{
    int led1Alpha;
    int led2Alpha;
};

void ledTask1(void *param)
{
    ledParams *led = (ledParams *)param;
    for (;;)
    {
        analogWrite(11, led->led1Alpha);
    }
}

void ledTask2(void *param)
{
    for (;;)
    {
    }
}

void ledTask3(void *param)
{
    for (;;)
    {
    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(11, OUTPUT);
    pinMode(10, OUTPUT);

    ledParams *params = new ledParams();
    params->led1Alpha = 255;

    xTaskCreate(ledTask1, "LedTask1", 128, (void *)params, 0, NULL);
    // xTaskCreate(ledTask2, "LedTask2", 128, NULL, 1, NULL);
    // xTaskCreate(ledTask3, "LedTask3", 128, NULL, 2, NULL);
}

void loop()
{
}