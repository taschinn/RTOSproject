#ifndef GASSENSOR_H_
#define GASSENSOR_H_

#include <FreeRTOS.h>
#include <queue.h>

static const uint16_t gas_limitFireAlarm = 500;

void gas_Init();

#endif