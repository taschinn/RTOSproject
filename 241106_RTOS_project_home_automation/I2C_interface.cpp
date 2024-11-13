#include "I2C_interface.h"

#include <Wire.h>

SemaphoreHandle_t I2Cintf_mutex;

void I2Cintf_Init() {

  // Initialize I2C-hardware
  Wire.begin(SDA0_PIN, SCL0_PIN);  // Set I2C-pins on esp32

  // Create mutex before starting tasks
  I2Cintf_mutex = xSemaphoreCreateMutex();
}