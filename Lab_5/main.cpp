#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <timers.h>
#include <stdlib.h>
#include <IRremote.h>

// IR remote defines
#define POWER 0x00FF629D
#define A 0x00FF22DD
#define B 0x00FF02FD
#define C 0x00FFC23D
#define UP 0x00FF9867
#define DOWN 0x00FF38C7
#define LEFT 0x00FF30CF
#define RIGHT 0x00FF7A85
#define SELECT 0x00FF18E7

// IR sensor pin
#define Signal_PIN 3 // green

// distance sensor HC-SR04 HY-SRF05
#define TRIG_PIN 13 // red
#define ECHO_PIN 12 // orange

// Left motor direction pins
#define IN1_PIN 9
#define IN2_PIN 8

// Right motor direction pins
#define IN3_PIN 4
#define IN4_PIN 7

// Motor PWM pins
#define MOTOR_LEFT_PIN 6
#define MOTOR_RIGHT_PIN 5

#define MAX_SPEED 255.0f

SemaphoreHandle_t mutex;
long duration = 0, cm = 0;
static float speed = 0.0f;
IRrecv irrecv(Signal_PIN);
decode_results results;
static bool forward = false;

// handles the distance sensor HC-SR04 HY-SRF05, if the distance is less or equal to 20 the engines will stop
// and will also block the forward movement until the distance is less than 20.
void distanceSensorTask(void *param)
{
    for (;;)
    {
        // code for the sensor to even work.
        long distance;
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(5);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);

        pinMode(ECHO_PIN, INPUT);
        duration = pulseIn(ECHO_PIN, HIGH);

        cm = (duration / 2) / 29.1;

        // check distance, if its less or equal than 20 block forward movement and stop engines.
        // also take the semaphore to make sure nothing else changes the values.
        xSemaphoreTake(mutex, portMAX_DELAY);
        if (cm <= 20 && forward)
        {
            speed = 0.0f;
            digitalWrite(IN1_PIN, LOW);
            digitalWrite(IN2_PIN, LOW);
            digitalWrite(IN3_PIN, LOW);
            digitalWrite(IN4_PIN, LOW);

            analogWrite(MOTOR_LEFT_PIN, 0);
            analogWrite(MOTOR_RIGHT_PIN, 0);
            Serial.println("DISTANCE CRITICAL.");
        }

        // give back semaphore
        xSemaphoreGive(mutex);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// this task handles all the IR input and if an input is received send it to the translateIR function that handles the input.
void irSensorTask(void *param)
{
    for (;;)
    {
        if (irrecv.decode(&results))
        {
            translateIR();
            irrecv.resume();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void setup()
{
    Serial.begin(9600);
    irrecv.enableIRIn();
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    pinMode(IN3_PIN, OUTPUT);
    pinMode(IN4_PIN, OUTPUT);
    pinMode(MOTOR_LEFT_PIN, OUTPUT);
    pinMode(MOTOR_RIGHT_PIN, OUTPUT);

    // create mutex
    mutex = xSemaphoreCreateMutex();

    xTaskCreate(distanceSensorTask, "Distance_sensor", 100, nullptr, 2, NULL);
    xTaskCreate(irSensorTask, "IR_sensor", 100, nullptr, 1, NULL);
}

void loop()
{
    
}

// This controls the speed of the car's wheels based on the user input with the controller
void translateIR()
{
    xSemaphoreTake(mutex, portMAX_DELAY); // take semaphore
    
    //stop engines when power button is pressed.
    if(results.value == POWER)
    {
        digitalWrite(IN1_PIN, LOW);
        digitalWrite(IN2_PIN, LOW);
        digitalWrite(IN3_PIN, LOW);
        digitalWrite(IN4_PIN, LOW);

        analogWrite(MOTOR_LEFT_PIN, 0);
        analogWrite(MOTOR_RIGHT_PIN, 0);
    }
    else if(results.value == UP) // forward movement
    {
        digitalWrite(IN1_PIN, HIGH);
        digitalWrite(IN2_PIN, LOW);
        digitalWrite(IN3_PIN, LOW);
        digitalWrite(IN4_PIN, HIGH);

        analogWrite(MOTOR_LEFT_PIN, speed);
        analogWrite(MOTOR_RIGHT_PIN, speed);
        Serial.println("UP");
        forward = true;
    }
    else if(results.value == DOWN) // backward movement
    {        
        digitalWrite(IN1_PIN, LOW);
        digitalWrite(IN2_PIN, HIGH);
        digitalWrite(IN3_PIN, HIGH);
        digitalWrite(IN4_PIN, LOW);

        analogWrite(MOTOR_LEFT_PIN, speed);
        analogWrite(MOTOR_RIGHT_PIN, speed);
        forward = false;
    }
    else if(results.value == RIGHT) // right movement
    {
        digitalWrite(IN1_PIN, LOW);
        digitalWrite(IN2_PIN, HIGH);
        digitalWrite(IN3_PIN, LOW);
        digitalWrite(IN4_PIN, HIGH);

        analogWrite(MOTOR_LEFT_PIN, speed);
        analogWrite(MOTOR_RIGHT_PIN, speed);
        forward = false;
    }
    else if(results.value == LEFT) // left movement
    {
        digitalWrite(IN1_PIN, HIGH);
        digitalWrite(IN2_PIN, LOW);
        digitalWrite(IN3_PIN, HIGH);
        digitalWrite(IN4_PIN, LOW);

        analogWrite(MOTOR_LEFT_PIN, speed);
        analogWrite(MOTOR_RIGHT_PIN, speed);
        forward = false;
    }
    else if(results.value == A) // set speed to 50%
    {
        speed = MAX_SPEED / 2.0;
    }
    else if(results.value == B) // set speed to 75%
    {
        speed = MAX_SPEED * 0.75f;
    }
    else if(results.value == C) // set max speed (255)
    {
        speed = MAX_SPEED;
    }
   
    xSemaphoreGive(mutex); // give back semaphore
    
    delay(100); // to not get immediate repeat
}