#ifndef MAIN_H
#define MAIN_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern QueueHandle_t* Get_simhub_data_queue(void);

#endif