#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <time.h>
#include <stdlib.h>

#define MAX_QUEUE_SIZE 10
/*
 * A. Monitor
Implement a monitor that contains a data structure (an array) which can contain a limited number of
integer numbers. Adding and reading the numbers form the data structure has to be in queue manner, i.e.,
First In First Out (FIFO). Notice that it is not allowed to use FreeRTOS queues.

• The monitor has to provide 3 functions;  init(), read() and add().
• init()  initializes the monitor, e.g.,  initialize semaphores.
• read() reads and removes the number from the top of the queue if it is not empty. If the queue is empty
the caller task will block.
• add() adds a number to the end of the queue if it is not full. If the queue is full the caller task will block.
• No more than one task is allowed to add to or read from the queue at the same time.
• The protection and synchronization has to be wrapped in read() and add() functions.
 */
typedef struct Monitor
{
  SemaphoreHandle_t mutex;
  SemaphoreHandle_t bufFullSem;
  SemaphoreHandle_t bufEmptySem;
  int buffer[MAX_QUEUE_SIZE];
  int index = 0;
} MyMonitor;

void init(MyMonitor *monitor)
{
  // allocating enough memory for monitor data
  monitor->mutex = xSemaphoreCreateMutex();
  monitor->bufFullSem = xSemaphoreCreateCounting(MAX_QUEUE_SIZE, MAX_QUEUE_SIZE); // <- sizes
  monitor->bufEmptySem = xSemaphoreCreateCounting(MAX_QUEUE_SIZE, 0);             // <- sizes
}

void read(MyMonitor *monitor, int *x)
{
  // read from monitor->data to *x
  // sema shits here
  xSemaphoreTake(monitor->bufEmptySem, portMAX_DELAY);
  xSemaphoreTake(monitor->mutex, portMAX_DELAY);

  *x = monitor->buffer[0];

  for (int i = 1; i < MAX_QUEUE_SIZE; ++i)
  {
    monitor->buffer[i - 1] = monitor->buffer[i];
  }

  monitor->index--;

  xSemaphoreGive(monitor->mutex);
  xSemaphoreGive(monitor->bufFullSem);
}

void add(MyMonitor *monitor, int x)
{
  // write data from x to monitor->data
  // some more sema shit here
  xSemaphoreTake(monitor->bufFullSem, portMAX_DELAY);
  xSemaphoreTake(monitor->mutex, portMAX_DELAY);

  monitor->buffer[monitor->index++] = x;

  xSemaphoreGive(monitor->mutex);
  xSemaphoreGive(monitor->bufEmptySem);
}

void producer1(void *param)
{
  Monitor *monitor = (Monitor *)param;
  for (;;)
  {
    int r = rand();
    Serial.print("Producer1: ");
    Serial.println(r);
    add(monitor, r);

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void producer2(void *param)
{
  Monitor *monitor = (Monitor *)param;
  for (;;)
  {
    int r = rand();
    Serial.print("Producer2: ");
    Serial.println(r);
    add(monitor, r);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void consumer1(void *param)
{
  Monitor *monitor = (Monitor *)param;
  for (;;)
  {
    int num;
    read(monitor, &num);
    Serial.print("Consumer1: ");
    Serial.print(num);
    Serial.println(" ");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void consumer2(void *param)
{
  Monitor *monitor = (Monitor *)param;
  for (;;)
  {
    int num;
    read(monitor, &num);
    Serial.print("Consumer2: ");
    Serial.print(num);
    Serial.println(" ");

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Monitor *monitor = new Monitor();
  srand(time(NULL));

  init(monitor);
  xTaskCreate(consumer1, "consumer1", 100, (void *)monitor, 1, NULL);
  xTaskCreate(consumer2, "consumer2", 100, (void *)monitor, 1, NULL);
  xTaskCreate(producer1, "producer1", 100, (void *)monitor, 2, NULL);
  xTaskCreate(producer2, "producer2", 100, (void *)monitor, 2, NULL);
}

void loop()
{
}
