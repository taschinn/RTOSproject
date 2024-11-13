#include "I2C_interface.h"
#include "gas_sensor.h"

#define GAS_ADDRESS 0x59

#define GAS_APP_CPU 1
#define GAS_PERIOD_MS 500

void gasTask(void *parameters) {

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

    vTaskDelay(pdMS_TO_TICKS(GAS_PERIOD_MS));
  }
}

void gas_Init() {

  //Task 1
  xTaskCreatePinnedToCore(gasTask,
                          "GAS task",
                          2048,
                          NULL,
                          3,
                          NULL,
                          GAS_APP_CPU);
}
