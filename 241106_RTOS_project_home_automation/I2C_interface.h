#ifndef I2CINTERFACE_H_
#define I2CINTERFACE_H_

#include <FreeRTOS.h>
#include <semphr.h>

#define SDA0_PIN 6  // select ESP32  I2C pins
#define SCL0_PIN 5

extern SemaphoreHandle_t I2Cintf_mutex;

void I2Cintf_Init();


#endif
