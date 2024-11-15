#include "freertos/portmacro.h"
#include <Arduino.h>
#include "fire_alarm.h"
#include "display.h"

#define FIRE_APP_CPU 1

QueueHandle_t fire_queue;
SemaphoreHandle_t fire_sem;

// Button debounce
static volatile unsigned long buttonLastPressTime = 0;
static volatile unsigned long resetLastPressTime = 0;
static const unsigned long debounceDelayMs = 200;

void IRAM_ATTR fire_buttonISR() {
  // Interrupt subroutine
  BaseType_t higherPriorityTaskWoken = pdFALSE;

  // Debounce
  unsigned long currentTime = millis();
  if (currentTime - buttonLastPressTime > debounceDelayMs) {
    buttonLastPressTime = currentTime;
    // Serial.println("FIRE: ISR Fire enwoked");
    xSemaphoreGiveFromISR(fire_sem, &higherPriorityTaskWoken);  // Fire alarm from button
    Serial.println("FIRE: Semaphore given (Fire button pressed");
    Serial.println();

    // Allow a context switch if a task with a higher priority has been invoked due to the xSemaphoreGive operation
    if (higherPriorityTaskWoken == pdTRUE) {
      portYIELD_FROM_ISR();
    }
  }
}

void IRAM_ATTR fire_resetISR() {
  // Interrupt subroutine
  BaseType_t higherPriorityTaskWoken = pdFALSE;

  // Debounce
  unsigned long currentTime = millis();
  if (currentTime - resetLastPressTime > debounceDelayMs) {
    resetLastPressTime = currentTime;
    Serial.println("FIRE: ISR Reset enwoked");
    void* ptr = NULL;
    xQueueReceiveFromISR(fire_queue, &ptr, &higherPriorityTaskWoken);
    Serial.print("FIRE: # elements in queue=");
    Serial.println(uxQueueMessagesWaiting(fire_queue));
    Serial.println();

    // Allow a context switch if a task with a higher priority has been invoked due to the xSemaphoreGive operation
    if (higherPriorityTaskWoken == pdTRUE) {
      portYIELD_FROM_ISR();
    }
  }
}

void fireTask(void *parameters) {
  DisplayMessage msg;
  msg.name = FIRE_ALARM;

  // Loop forever
  while (1) {
    // Wait for fire alarm from button / gassensor
    xSemaphoreTake(fire_sem, portMAX_DELAY);
    Serial.println("FIRE: semaphore taken");

    // Set firealarm
    void* ptr = NULL;
    xQueueSend(fire_queue, &ptr, portMAX_DELAY);
    Serial.println("FIRE: firealarm set");
    Serial.print("FIRE: # elements in queue=");
    Serial.println(uxQueueMessagesWaiting(fire_queue));
    Serial.println();

    // Start buzzer

    // Send value to display
    msg.value = "ON";
    xQueueSend(display_queue, &msg, portMAX_DELAY);

    // Wait until fire alarm is resetted
    while (uxQueueMessagesWaiting(fire_queue) != 0) {}

    // Reset buzzer

    // Send value to display
    msg.value = "OFF";
    xQueueSend(display_queue, &msg, portMAX_DELAY);
  }
}

void fire_Init() {
  // Configurate pins
  pinMode(FIRE_BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(FIRE_RESET_PIN, INPUT_PULLDOWN);

  // Configurate ISRs
  attachInterrupt(FIRE_BUTTON_PIN, fire_buttonISR, RISING);
  attachInterrupt(FIRE_RESET_PIN, fire_resetISR, RISING);

  // Create ressources
  fire_queue = xQueueCreate(1, sizeof(void*));
  fire_sem = xSemaphoreCreateBinary();

  // Create task
  xTaskCreatePinnedToCore(fireTask,
                          "fire task",
                          2048,
                          NULL,
                          3,
                          NULL,
                          FIRE_APP_CPU);
}
