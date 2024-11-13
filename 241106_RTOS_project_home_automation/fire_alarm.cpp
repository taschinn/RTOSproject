#include "fire_alarm.h"

#define FIRE_PWM_FREQUENCY 10000
#define FIRE_APP_CPU 1

QueueHandle_t fire_queue;
SemaphoreHandle_t fire_sem;


void IRAM_ATTR fire_ISR() {
  // Interrupt subroutine
  xSemaphoreGiveFromISR(fire_sem)
}


void fire_Init(){
  // Configurate pins
  pinMode(FIRE_BUTTON_PIN, INPUT);
  pinMode(FIRE_BUZZER_PIN, OUTPUT);
  analogWriteFrequency(FIRE_BUZZER_PIN, FIRE_PWM_FREQUENCY);

  // Configurate ISR
  attachInterrupt(FIRE_BUTTON_PIN, fire_ISR, RISING);

  // Create task
  xTaskCreatePinnedToCore(fireTask,
                          "Fire task",
                          2048,
                          NULL,
                          3,
                          NULL,
                          FIRE_APP_CPU);
}