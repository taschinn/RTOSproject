#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <FreeRTOS.h>
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
    SensotType name;
    String value;
} DisplayMessage;

static const uint8_t display_queue_len = 15;
extern QueueHandle_t display_queue;

void displayTask(void *parameters);
void printInDisplay(DisplayMessage msg);
void printFirstTime();
void paintValue(int x, int y, String text);
void display_Init();