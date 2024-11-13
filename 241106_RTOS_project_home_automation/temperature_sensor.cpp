#include "I2C_interface.h"
#include "temperature_sensor.h"

#define TEMP_ADDRESS 0x00

#define TEMP_APP_CPU 1
#define TEMP_PERIOD_MS 500

void tempTask(void *parameters) {

  float temp_localvar;

  // Loop forever
  while (1) {

    // Take mutex prior to critical section

    if (xSemaphoreTake(I2Cintf_mutex, 10) == pdTRUE) {

      for (;;)
        ;  // todo read sensor value

      // Give mutex after critical
      xSemaphoreGive(I2Cintf_mutex);

    } else {
      // Do something else
    }

    xQueueSend(temp_queue, (void *)&temp_localvar, 10);

    vTaskDelay(pdMS_TO_TICKS(TEMP_PERIOD_MS));
  }
}

void temp_Init() {

  //Task 1
  xTaskCreatePinnedToCore(tempTask,
                          "Temp task",
                          2048,
                          NULL,
                          2,
                          NULL,
                          TEMP_APP_CPU);


  temp_queue = xQueueCreate(temp_queue_len, sizeof(float));
}
