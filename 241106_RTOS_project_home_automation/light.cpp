#include "projdefs.h"
#include "pins_arduino.h"
#include "esp32-hal-rgb-led.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal-adc.h"
#include "light.h"
#include "fire_alarm.h"
#include "display.h"

#define LIGHT_APP_CPU 1
#define LIGHT_PERIOD_MS 500

volatile bool state = false;  // Remembers the actual state of the lighting
// There is a critical section needed to access this variable, because it can be accessed out of an ISR or a task running on a separate core.
// If just one core is used now critical section is needed on the esp32 because a boolean operation is always atomic!

// Define a mutex for the critical section
portMUX_TYPE muxState = portMUX_INITIALIZER_UNLOCKED;

void lightTask(void *parameters) {
  int analogValue = 0;
  DisplayMessage msg;
  msg.name = SensorType.LIGHT;

  // Loop forever
  while (1) {
    if (uxQueueMessagesWaiting(fire_queue) == 0) { // No fire alarm
      if (state == true) {  // Light on
        analogValue = analogRead(LIGHT_POTI_PIN);
        neopixelWrite(RGB_BUILTIN, analogValue, analogValue, analogValue);
      } else {  // Light off
        neopixelWrite(RGB_BUILTIN, 0, 0, 0);
      }

      msg.value = analogValue;
      xQueueSend(display_queue, &msg, portMAX_DELAY);
    } else {  // Fire alarm
      neopixelWrite(RGB_BUILTIN, 100, 100, 100);
    }
    vTaskDelay(pdMS_TO_TICKS(LIGHT_PERIOD_MS));
  }
}

void light_TriggerState() {
  taskENTER_CRITICAL(&muxState);  // Critical section to protect shared variable 'state'
  state != state;
  taskEXIT_CRITICAL(&muxState);
}

void light_SetState(bool stateNew) {
  state = stateNew;  // No critical section needed because write a boolean variable on an esp32 is an atomic action.
}

void IRAM_ATTR light_ISR() {
  // Interrupt subroutine
  light_TriggerState();
}

void light_Init() {

  // Set the resolution to 12 bits (0-4095)
  analogReadResolution(12);

  // Configurate pins
  pinMode(LIGHT_BUTTON_PIN, INPUT);

  // Configurate ISR
  attachInterrupt(LIGHT_BUTTON_PIN, light_ISR, RISING);

  // Create task
  xTaskCreatePinnedToCore(lightTask,
                          "Light task",
                          2048,
                          NULL,
                          2,
                          NULL,
                          LIGHT_APP_CPU);
}
