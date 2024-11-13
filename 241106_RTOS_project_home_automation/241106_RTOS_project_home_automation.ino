#include "I2C_interface.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "light.h"
#include "display.h"

void setup() {
  // put your setup code here, to run once:
  I2Cintf_Init();
  temp_Init();
  gas_Init();
  light_Init();
  display_Init();
}

void loop() {
  // put your main code here, to run repeatedly:

}
