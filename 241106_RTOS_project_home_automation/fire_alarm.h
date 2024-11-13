#ifndef FIREALARM_H_
#define FIREALARM_H_

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#define FIRE_BUZZER_PIN 4  // Specifies the digital output pin for the buzzer
#define FIRE_BUTTON_PIN 5  // Specifies the digital input pin for the button

extern QueueHandle_t fire_queue;
extern SemaphoreHandle_t fire_sem;

void fire_Init();


#endif