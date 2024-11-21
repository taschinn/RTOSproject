#include "freertos/portmacro.h"
#include "projdefs.h"
#include "esp32-hal.h"
#include "ventilation.h"
#include "fire_alarm.h"
#include "temperature_sensor.h"
#include "display.h"
#include <Arduino.h>

#define VENT_APP_CPU 1
#define VENT_PERIOD_MS 500

#define VENT_PWM_FREQUENCY 25000

static const uint8_t minSpeed = 40;  // Minimal dutycycle for PWM in %
static const uint8_t maxSpeed = 70;  // Maximal dutycycle for PWM in % (normal mode, no fire)
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
  DisplayMessage msg;

  vTaskDelay(pdMS_TO_TICKS(500));

  // Loop forever
  while (1) {
    if (uxQueueMessagesWaiting(fire_queue) == 0) {  // No fire alarm
      if (xQueueReceive(temp_queue, &temp, pdMS_TO_TICKS(50)) == pdTRUE) {
        // Calculate and write speed to ventilator
        speed = vent_GetSpeedFromTemp(temp);
      }
    } else {  // Fire alarm
      speed = 100;
    }
    analogWrite(VENTILATION_PIN, speed * 255 / 100);

    // Send value to display
    msg.name = VENTILATION;
    msg.value = speed;
    xQueueSend(display_queue, &msg, pdMS_TO_TICKS(50));

    vTaskDelay(pdMS_TO_TICKS(VENT_PERIOD_MS));
  }
}

void vent_Init() {
  // Configurate pins
  pinMode(VENTILATION_PIN, OUTPUT);
  analogWriteFrequency(VENTILATION_PIN, VENT_PWM_FREQUENCY);
  analogWriteResolution(VENTILATION_PIN, 8);

  // Create task
  xTaskCreatePinnedToCore(ventTask,
                          "Ventilation task",
                          2048,
                          NULL,
                          2,
                          NULL,
                          VENT_APP_CPU);

  float temp = 0;
  xQueueReceive(temp_queue, &temp, pdMS_TO_TICKS(50));

}
