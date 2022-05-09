#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include <semphr.h>
#include <queue.h>

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
typedef struct Monitor{
  SemaphoreHandle_t mutex;
  SemaphoreHandle_t bufFullSem;
  SemaphoreHandle_t bufEmptySem;
  int buffer[MAX_QUEUE_SIZE];
  int index = 0;
} MyMonitor;

// kanske ska ta med index här????
void init(MyMonitor* monitor)
{
  // allocating enough memory for monitor data
  monitor->mutex=xSemaphoreCreateMutex();
  monitor->bufFullSem = xSemaphoreCreateCounting(MAX_SIZE, MAX_BSIZE); // <- sizes
  monitor->bufEmptySem = xSemaphoreCreateCounting(MAX_BUF_SIZE, 0)); // <- sizes
}

void read(MyMonitor* monitor, int* x){
  // read from monitor->data to *x
  // sema shits here
  xSemaphoreTake(monitor->bufEmptySem, portMAX_DELAY);
  xSemaphoreTake(monitor->mutex);
  *x = monitor->buffer[--(monitor->index)];
  xSemaporeGive(monitor->mutex);
  xSemaphoreGive(monitor->bufFullSem);
}

void add(MyMonitor* monitor, int x){
  // write data from x to monitor->data
  // some more sema shit here
  xSemaphoreTake(monitor->bufFullSem, portMax_DELAY);
  xSemaphoreTake(monitor->mutex, portMAX_DELAY);
  monitor->buffer[monitor->index++] = x;
  xSemaphoreGive(monitor->mutex);
  xSemaphoreGive(monitor->bufEmptySem);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {

}