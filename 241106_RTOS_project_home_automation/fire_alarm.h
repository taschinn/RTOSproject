#ifndef FIREALARM_H_
#define FIREALARM_H_

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#define FIRE_BUTTON_PIN 41 // Specifies the digital input pin for the fire button
#define FIRE_RESET_PIN 45 // Specifies the digital input pin for the reset button
#define FIRE_BUZZER_PIN 13 // Specifies the digital output pin for the buzzer

extern QueueHandle_t fire_queue;
extern SemaphoreHandle_t fire_sem;



void fire_Init();

















#endif