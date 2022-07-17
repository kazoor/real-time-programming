#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#define pdMS_TO_TICKS(xTimeInMs) ((TickType_t)(((unsigned long)(xTimeInMs) * (TickType_t)configTICK_RATE_HZ) / (TickType_t)1000))
#define MAX_LED_ALPHA 255
#define MAX_QUEUE_SIZE 5

/*
B.2.
Change the program so that received commands can be queued by task1 and task2. The commands have to be
queued in a FreeRTOS Queue; there should be 2 queues; one queue to queue commands for task1 and one that queues
commands for task2. Depending on n part of the received command, task3 puts it into one of the queues. The length of
the queues has to be limited to 5 commands. If a new command arrives while the corresponding queue is full the
command has to be thrown away. If a task (task1 or task2) tries to read a command from its corresponding command
queue while the queue is empty, the task has to block until a command is in the queue. Change the periods of task1 and
task2 to 5 seconds so that the user can manage to put multiple commands into queues.

B.3.
Use a button to reset the command queues, i.e., clear all the existing commands in the queues. The button has
to trigger an external interrupt. On Arduino Uno pins 2 and 3 are external interrupt pins. Implement an Interrupt Service
Routine (ISR) for the interrupt where the contents of queues are cleared.
*/

struct ledCommand
{
    float alpha;
};

float inputValue = 0;
QueueHandle_t task1Queue = NULL;
QueueHandle_t task2Queue = NULL;

void interruptCallback(void)
{
    Serial.println("Cleared all queues!");

    xQueueReset(task1Queue);
    xQueueReset(task2Queue);

    delay(200);
}

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
        vTaskDelay(pdMS_TO_TICKS(4000));
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
        vTaskDelay(pdMS_TO_TICKS(4000));
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
    // pinMode(2, INPUT_PULLUP);

    task1Queue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(ledCommand));
    task2Queue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(ledCommand));

    attachInterrupt(digitalPinToInterrupt(2), interruptCallback, RISING);

    xTaskCreate(ledTask1, "LedTask1", 128, NULL, 1, NULL);
    xTaskCreate(ledTask2, "LedTask2", 128, NULL, 2, NULL);
    xTaskCreate(inputTask, "inputTask", 128, NULL, 0, NULL);
}

void loop()
{
    // Unused
}
