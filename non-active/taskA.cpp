#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#define pdMS_TO_TICKS(xTimeInMs) ((TickType_t)(((unsigned long)(xTimeInMs) * (TickType_t)configTICK_RATE_HZ) / (TickType_t)1000))

void ledTask1(void *params)
{
  for (;;)
  {
    digitalWrite(11, HIGH);
    vTaskDelay(pdMS_TO_TICKS(2000));
    digitalWrite(11, LOW);
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

void ledTask2(void *params)
{
  for (;;)
  {
    digitalWrite(10, HIGH);
    vTaskDelay(pdMS_TO_TICKS(2000));
    digitalWrite(10, LOW);
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);

  xTaskCreate(ledTask1, "LedTask1", 128, NULL, 0, NULL);
  xTaskCreate(ledTask2, "LedTask2", 128, NULL, 1, NULL);
}

void loop()
{
}