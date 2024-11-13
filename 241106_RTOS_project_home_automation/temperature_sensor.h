#ifndef TEMPERATURESENSOR_H_
#define TEMPERATURESENSOR_H_

#include <FreeRTOS.h>
#include <queue.h>

static const uint8_t temp_queue_len = 5;
static QueueHandle_t temp_queue;

void temp_Init();
















#endif