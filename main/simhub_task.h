#ifndef SIMHUB_TASK_H
#define SIMHUB_TASK_h

#include "freertos/FreeRTOS.h"

extern void simhub_task(void *arg);
extern QueueHandle_t* get_simhub_data_queue(void);

#endif //SIMHUB_TASK_H