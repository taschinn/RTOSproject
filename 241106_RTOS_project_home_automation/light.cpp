#include "esp32-hal.h"
#include "projdefs.h"
#include "pins_arduino.h"
#include "esp32-hal-rgb-led.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal-adc.h"
#include "light.h"
#include "fire_alarm.h"
#include "display.h"

#define LIGHT_APP_CPU 1
#define LIGHT_PERIOD_MS 200

volatile bool state = false;  // Remembers the actual state of the lighting
// There is a critical section needed to access this variable, because it can be accessed out of an ISR or a task running on a separate core.
// If just one core is used now critical section is needed on the esp32 because a boolean operation is always atomic!

// Define a mutex for the critical section
portMUX_TYPE muxState = portMUX_INITIALIZER_UNLOCKED;

void lightTask(void *parameters) {
  int analogValue = 0;
  int brightness;
  DisplayMessage msg;
  msg.name = LIGHT;

  analogWrite(LIGHT_OUTPUT_PIN, 255);

  // Wait until the the module gets ready
  vTaskDelay(pdMS_TO_TICKS(500));

  analogWrite(LIGHT_OUTPUT_PIN, 0);

  // Loop forever
  while (1) {
    if (uxQueueMessagesWaiting(fire_queue) == 0) {  // No fire alarm
      if (state == true) {                          // Light on
        analogValue = analogRead(LIGHT_POTI_PIN);
        brightness = max(analogValue * 100 / 4095, 1); // Minimal light value when turned on is 20%
        // Serial.print("LIGHT: brightness=");
        // Serial.print(brightness);
        // Serial.print(" analogValue=");
        // Serial.println(analogValue);

      } else {  // Light off
        brightness = 0;
        // neopixelWrite(RGB_BUILTIN, 0, 0, 0);
      }
      // neopixelWrite(RGB_BUILTIN, brightness, brightness, brightness);
      analogWrite(LIGHT_OUTPUT_PIN, brightness * 255 / 100);

      // Send value to display
      msg.value = brightness;
      xQueueSend(display_queue, &msg, pdMS_TO_TICKS(50));
    } else {  // Fire alarm
      // neopixelWrite(RGB_BUILTIN, 100, 100, 100);
      analogWrite(LIGHT_OUTPUT_PIN, 255);
    }

    vTaskDelay(pdMS_TO_TICKS(LIGHT_PERIOD_MS));
  }
}

void light_TriggerState() {
  taskENTER_CRITICAL(&muxState);  // Critical section to protect shared variable 'state'
  if(state == false) {
    state = true;
  } else {
    state = false;
  }
  // state != state;
  Serial.print("LIGHT: state=");
  Serial.println(state);
  taskEXIT_CRITICAL(&muxState);
}

void light_SetState(bool stateNew) {
  state = stateNew;  // No critical section needed because write a boolean variable on an esp32 is an atomic action.
}

// Button debounce
static volatile unsigned long lastPressTime = 0;
static const unsigned long debounceDelayMs = 200;

void IRAM_ATTR light_ISR() {
  // Interrupt subroutine
  // Debounce
  unsigned long currentTime = millis();
  if (currentTime - lastPressTime > debounceDelayMs) {
    lastPressTime = currentTime;
    Serial.println("LIGHT: ISR enwoked");
    light_TriggerState();
  }
}

void light_Init() {
  // Set the resolution to 12 bits (0-4095)
  analogReadResolution(12);

  // Configurate pins
  pinMode(LIGHT_BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(LIGHT_OUTPUT_PIN, OUTPUT);

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
