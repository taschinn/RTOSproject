#include "projdefs.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "I2C_interface.h"
#include "temperature_sensor.h"
#include "display.h"

#define DHTTYPE DHT22  // DHT 22 (AM2302)

#define TEMP_APP_CPU 1
#define TEMP_PERIOD_MS 2000  // Not faster than 0.5Hz

static DHT dht(TEMP_PIN, DHTTYPE);

void tempTask(void *parameters) {
  float temp, hum;
  DisplayMessage msg;

  // Wait until the sensor gets ready
  vTaskDelay(pdMS_TO_TICKS(500));

  // Loop forever
  while (1) { 
    temp = dht.readTemperature();
    hum = dht.readHumidity();

    // Send temperature value to other modules
    xQueueSend(temp_queue, &temp, pdMS_TO_TICKS(50));

    // Send value to display
    msg.name = TEMP;
    msg.value = String(temp, 1);
    xQueueSend(display_queue, &msg, pdMS_TO_TICKS(50));

    vTaskDelay(pdMS_TO_TICKS(TEMP_PERIOD_MS));
  }
}

void temp_Init() {
  // Sensor initialization
  dht.begin();

  // Create task
  xTaskCreatePinnedToCore(tempTask,
                          "Temp task",
                          2048,
                          NULL,
                          2,
                          NULL,
                          TEMP_APP_CPU);

  // Create queue
  temp_queue = xQueueCreate(temp_queue_len, sizeof(float));

}
