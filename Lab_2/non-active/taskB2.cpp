#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#define pdMS_TO_TICKS(xTimeInMs) ((TickType_t)(((unsigned long)(xTimeInMs) * (TickType_t)configTICK_RATE_HZ) / (TickType_t)1000))
#define MAX_LED_ALPHA 255
#define MAX_QUEUE_SIZE 5

struct ledCommand
{
    float alpha;
};

float inputValue = 0;
SemaphoreHandle_t mtx;
QueueHandle_t task1Queue;
QueueHandle_t task2Queue;

void ledTask1(void *param)
{
    ledCommand *cmd = new ledCommand();
    for (;;)
    {
        if (uxQueueMessagesWaiting(task1Queue) != 0)
        {
            xQueueReceive(task1Queue, (void *)cmd, portMAX_DELAY);
            analogWrite(10, cmd->alpha);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void ledTask2(void *param)
{
    ledCommand *cmd = new ledCommand();
    for (;;)
    {
        if (uxQueueMessagesWaiting(task2Queue) != 0)
        {
            xQueueReceive(task2Queue, (void *)cmd, portMAX_DELAY);
            analogWrite(11, cmd->alpha);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void inputTask(void *param)
{
    ledCommand *cmd = new ledCommand();
    int n = 0; // Selected LED in the input.

    for (;;)
    {
        if (Serial.available() > 0)
        {
            Serial.print("Input: ");
            inputValue = Serial.parseFloat();
            Serial.println(inputValue);

            n = (int)inputValue;

            float decimal = inputValue - n;

            if (uxQueueMessagesWaiting(task1Queue) == MAX_QUEUE_SIZE)
            {
                Serial.println("task1Queue is full!");
            }
            else if (uxQueueMessagesWaiting(task2Queue) == MAX_QUEUE_SIZE)
            {
                Serial.println("task2Queue is full!");
            }
            else
            {
                if (n == 1)
                {
                    cmd->alpha = MAX_LED_ALPHA * decimal;
                    xQueueSend(task1Queue, (void *)cmd, pdMS_TO_TICKS(20));
                }
                else if (n == 2)
                {
                    cmd->alpha = MAX_LED_ALPHA * decimal;
                    xQueueSend(task2Queue, (void *)cmd, pdMS_TO_TICKS(20));
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(11, OUTPUT);
    pinMode(10, OUTPUT);

    // Create the global mutex for ledparam struct.
    mtx = xSemaphoreCreateMutex();

    task1Queue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(ledCommand));
    task2Queue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(ledCommand));

    xTaskCreate(ledTask1, "LedTask1", 128, NULL, 1, NULL);
    xTaskCreate(ledTask2, "LedTask2", 128, NULL, 2, NULL);
    xTaskCreate(inputTask, "inputTask", 128, NULL, 0, NULL);
}

void loop()
{
    // Not used.
}
