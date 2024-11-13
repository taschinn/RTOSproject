#ifndef FIREALARM_H_
#define FIREALARM_H_

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

extern QueueHandle_t fire_queue;
extern SemaphoreHandle_t fire_sem;



void fire_Init();

















#endif