#include "projdefs.h"
#include "esp32-hal.h"
#include "ventilation.h"
#include "fire_alarm.h"
#include "temperature_sensor.h"

#define VENT_APP_CPU 1
#define VENT_PERIOD_MS 500

#define VENT_PWM_FREQUENCY 25000

static const uint8_t minSpeed = 10;  // Minimal dutycycle for PWM in %
static const uint8_t maxSpeed = 60;  // Maximal dutycycle for PWM in % (normal mode, no fire)
static const float minTemp = 20;     // Minimal temperature to adjust minSpeed at
static const float maxTemp = 28;     // Maximal temperature to adjust maxSpeed at
// Between the two temperatures the speed is adjusted linearly

int vent_GetSpeedFromTemp(float temp) {
  if (minSpeed > maxSpeed || minSpeed < 0 || maxSpeed > 100) {
    // Error: Wrong values
    return -1;
  }

  float diffTemp = maxTemp - minTemp;
  uint8_t diffSpeed = maxSpeed - minSpeed;
  int speed = (int)((temp - minTemp) / diffTemp) * (int)diffSpeed + (int)minSpeed;
  return speed;
}

void ventTask(void *parameters) {
  float temp;
  int speed;

  // Loop forever
  while (1) {
    if (uxQueueMessagesWaiting(fire_queue) == 0) {  // No fire alarm
      if (xQueueReceive(temp_queue, &temp, 0) == pdTRUE) {
        // Calculate and write speed to ventilator
        speed = vent_GetSpeedFromTemp(temp);
        analogWrite(VENTILATION_PIN, speed * 255 / 100);
      }
    } else {  // Fire alarm
      analogWrite(VENTILATION_PIN, 255);
    }
    vTaskDelay(pdMS_TO_TICKS(VENT_PERIOD_MS));
  }
}

void vent_Init() {

  // Configurate pins
  pinMode(VENTILATION_PIN, OUTPUT);
  analogWriteFrequency(VENTILATION_PIN, VENT_PWM_FREQUENCY);

  // Create task
  xTaskCreatePinnedToCore(ventTask,
                          "Ventilation task",
                          2048,
                          NULL,
                          2,
                          NULL,
                          VENT_APP_CPU);

  displayQueue = xQueueCreate(10, sizeof(DisplayMessage));
}
