#include <Arduino.h>
#include "fire_alarm.h"

QueueHandle_t fire_queue;
SemaphoreHandle_t fire_sem;

void fire_Init() {
  fire_queue = xQueueCreate(1, sizeof(void));
  Serial.println("FIRE: Queue created");
}
