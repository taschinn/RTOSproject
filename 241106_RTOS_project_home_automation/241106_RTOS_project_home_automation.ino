#include "I2C_interface.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "light.h"
#include "display.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("------------------------");
  I2Cintf_Init();
  Serial.println("I2C initialization done");
  // temp_Init();
  // gas_Init();
  // light_Init();
  display_Init();
  Serial.println("Display initialization done");
}

void loop() {
  // put your main code here, to run repeatedly:

  DisplayMessage test;
  test.name = (SensorType)random(0, 4);
  test.value = "18.9";

  Serial.print("test.name=");
  Serial.print(test.name);
  Serial.print(" test.value=");
  Serial.println(test.value);

  Serial.flush();

  xQueueSend(display_queue, &test, portMAX_DELAY);
  vTaskDelay(pdMS_TO_TICKS(500));

  test.name = FIRE_ALARM;
  test.value = "ON";

  xQueueSend(display_queue, &test, portMAX_DELAY);

  vTaskDelay(pdMS_TO_TICKS(3000));

  test.value = "OFF";

  xQueueSend(display_queue, &test, portMAX_DELAY);

  vTaskDelay(pdMS_TO_TICKS(500));

}
