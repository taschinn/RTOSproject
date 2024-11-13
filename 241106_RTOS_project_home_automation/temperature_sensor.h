#ifndef TEMPERATURESENSOR_H_
#define TEMPERATURESENSOR_H_

#include <FreeRTOS.h>
#include <queue.h>

#define TEMP_PIN 7

static const uint8_t temp_queue_len = 5;
static QueueHandle_t temp_queue;

void temp_Init();
















#endif