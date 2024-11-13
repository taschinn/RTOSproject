#include "I2C_interface.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "light.h"
#include "display.h"

// #include "esp_task_wdt.h"
#include <Adafruit_SGP40.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); } // Wait for serial console to open!
  Serial.println("------------------------");

  // Deactivate watchdog timer (for test purpose only)
  // esp_task_wdt_deinit();

  // Initialize modules
  I2Cintf_Init();
  gas_Init();
  temp_Init();
  // light_Init();
  display_Init();
  Serial.println("Initializations done");


}

void loop() {
  // test_display();

  

}

// --------------
// Test functions
// --------------
void test_display() {
  DisplayMessage test;
  test.name = (SensorType)random(0, 4);
  test.value = "18.9";

  // Serial.print("test.name=");
  // Serial.print(test.name);
  // Serial.print(" test.value=");
  // Serial.println(test.value);

  // Serial.flush();

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
