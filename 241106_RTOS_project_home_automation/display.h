#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <FreeRTOS.h>
#include <Adafruit_SSD1306.h>
#include <queue.h>
#include <semphr.h>

enum SensorType {
    TEMP,
    GAS,
    LIGHT,
    VENTILATION,
    FIRE_ALARM,
};

// Queue message structure
typedef struct {
    SensorType name;
    String value;
} DisplayMessage;

static const uint8_t display_queue_len = 15;
extern QueueHandle_t display_queue;

void displayTask(void *parameters);
void printInDisplay(DisplayMessage msg, String fire_alarm);
void printFirstTime();
void paintValue(int x, int y, String text);
void display_Init();

#endif