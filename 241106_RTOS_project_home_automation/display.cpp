#include "display.h"
#include "I2C_interface.h"

// Display
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define DISPLAY_APP_CPU 1

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SDA0_Pin 6  // select ESP32  I2C pins
#define SCL0_Pin 5

void displayTask(void *parameters) {
  DisplayMessage msg;  //temp (12ºC), gas (25ppm), light (L80%), ventilation (V67%), fire_alarm
  String fire_alarm = "OFF";

  // Loop forever
  while (1) {
    if (uxQueueMessagesWaiting(display_queue) == 0) {  // No new message
      if (xQueueReceive(display_queue, &msg, 0) == pdTRUE) {
        printInDisplay(msg, fire_alarm_on);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(VENT_PERIOD_MS));
  }
}

void printInDisplay(DisplayMessage msg, String fire_alarm) {
  if (msg.name == FIRE_ALARM) {
    fire_alarm = msg.value;
    if (fire_alarm.equals("OFF")) {
      printFirstTime();
    }
  }

  if (fire_alarm_on.equals("OFF")) {
    display.setTextSize(2);
    switch (msg.name) {
      case TEMP;
        paintValue(5, 5, msg.value + "C");
        break;
      case GAS:
        paintValue(SCREEN_WIDTH / 2 + 5, 5, msg.value + "ppm");
        break;
      case LIGHT:
        paintValue(5, SCREEN_HEIGHT / 2 + 5, "G" + msg.value + "%");
        break;
      case VENTILATION:
        paintValue(SCREEN_WIDTH / 2 + 5, SCREEN_HEIGHT / 2 + 5, "V" + msg.value + "%");
        break;
    }
  }
  else {
    display.clearDisplay();
    display.setTextSize(3);
    display.setCursor(32, 22);
    display.println("FIRE");
  }

  if (xSemaphoreTake(I2Cintf_mutex, portMAX_DELAY) == pdTRUE) {
    display.display();
    xSemaphoreGive(I2Cintf_mutex);
  }
}

void printFirstTime() {
  display.clearDisplay();
  display.drawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, SSD1306_WHITE);
  display.drawLine(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SSD1306_WHITE);
  paintValue(5, 5, "0C");
  paintValue(SCREEN_WIDTH / 2 + 5, 5, "0ppm");
  paintValue(5, SCREEN_HEIGHT / 2 + 5, "G0%");
  paintValue(SCREEN_WIDTH / 2 + 5, SCREEN_HEIGHT / 2 + 5, "V0%");

  if (xSemaphoreTake(I2Cintf_mutex, portMAX_DELAY) == pdTRUE) {
    display.display();
    xSemaphoreGive(I2Cintf_mutex);
  }
}

void paintValue(int x, int y, String text) {
  display.fillRect(x, y, 64, 32, SSD1306_BLACK);
  display.setCursor(x, y);
  display.println(text);
}

void display_Init() {

  // Configurate display
  Wire.begin(SDA0_Pin, SCL0_Pin);  // Set I2C-pins on esp32

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // Clean screen and set up text format values
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  display_queue = xQueueCreate(display_queue_len, sizeof(DisplayMessage));

  printFirstTime();

  // Create task
  xTaskCreatePinnedToCore(displayTask,
                          "Display task",
                          2048,
                          NULL,
                          2,
                          NULL,
                          DISPLAY_APP_CPU);
}