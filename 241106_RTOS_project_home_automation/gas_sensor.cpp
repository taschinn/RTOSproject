#include "freertos/portmacro.h"
#include "FreeRTOSConfig.h"
#include "projdefs.h"
#include "I2C_interface.h"
#include "gas_sensor.h"
#include "Adafruit_SGP40.h"
#include "fire_alarm.h"
#include "display.h"

#define GAS_ADDRESS 0x59

#define GAS_APP_CPU 1
#define GAS_PERIOD_MS 500

static Adafruit_SGP40 sgp;

void gasTask(void *parameters) {
  uint16_t voc;
  float temp = 25.0f;
  float hum = 30.0f;
  DisplayMessage msg;

  // Loop forever
  while (1) {
    // Get temperature and humidity values for compensated measurement.


    // Take mutex prior to critical section
    if (xSemaphoreTake(I2Cintf_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      // voc = sgp.measureRaw();
      voc = sgp.measureVocIndex(temp, hum);
      // voc = 123;

      // Give mutex after critical section
      xSemaphoreGive(I2Cintf_mutex);
    } else {
      vTaskDelay(pdMS_TO_TICKS(GAS_PERIOD_MS));
      continue;
    }

    // Serial.print("GAS_SENSOR: ");
    // Serial.print(voc);
    // Serial.println("ppm");

    // Send value to display
    msg.name = GAS;
    msg.value = voc;
    xQueueSend(display_queue, &msg, pdMS_TO_TICKS(50));

    // Set fire alarm if voc is bigger than the limit
    if (voc >= gas_limitFireAlarm) {
      xSemaphoreGive(fire_sem);
    }

    vTaskDelay(pdMS_TO_TICKS(GAS_PERIOD_MS));
  }
}

void gas_Init() {
  // Sensor initialization
  if (!sgp.begin()) {
    Serial.println("GAS_SENSOR: Failed to initialize SGP40 sensor.");
    return;
  }

  // Create task
  xTaskCreatePinnedToCore(gasTask,
                          "GAS task",
                          2048,
                          NULL,
                          3,
                          NULL,
                          GAS_APP_CPU);

}
