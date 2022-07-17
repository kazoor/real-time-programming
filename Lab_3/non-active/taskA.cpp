#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#define MAX_QUEUE_SIZE 10

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
  xSemaphoreTake(monitor->bufEmptySem, portMAX_DELAY);
  xSemaphoreTake(monitor->mutex, portMAX_DELAY);

  // Fix the indexes in the buffer after deleting them.
  int indexToDelete = monitor->index;
  *x = monitor->buffer[--(monitor->index)];
  for (int i = indexToDelete + 1; i < uxSemaphoreGetCount(monitor->bufEmptySem); ++i)
  {
    monitor->buffer[i] = monitor->buffer[i - 1];
  }

  xSemaphoreGive(monitor->mutex);
  xSemaphoreGive(monitor->bufFullSem);
}

void add(MyMonitor *monitor, int x)
{
  // write data from x to monitor->data
  xSemaphoreTake(monitor->bufFullSem, portMAX_DELAY);
  xSemaphoreTake(monitor->mutex, portMAX_DELAY);
  monitor->buffer[monitor->index++] = x;
  xSemaphoreGive(monitor->mutex);
  xSemaphoreGive(monitor->bufEmptySem);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop()
{
}