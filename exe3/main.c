#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <stdio.h>
#include "data.h"

QueueHandle_t xQueueData;

void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));
    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int total_samples = 11;
    int samples_received = 0;
    int buffer[5] = {0};
    int idx = 0, sum = 0, data = 0;
    while (samples_received < total_samples) {
        if (xQueueReceive(xQueueData, &data, pdMS_TO_TICKS(100))) {
            samples_received++;
            if (samples_received <= 5) {
                buffer[idx] = data;
                sum += data;
                idx = (idx + 1) % 5;
                if (samples_received == 5) {
                    printf("%d\n", sum / 5);
                }
            } else {
                sum -= buffer[idx];
                buffer[idx] = data;
                sum += data;
                idx = (idx + 1) % 5;
                printf("%d\n", sum / 5);
            }
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
    exit(0);
}

int main() {
    stdio_init_all();
    xQueueData = xQueueCreate(64, sizeof(int));
    xTaskCreate(data_task, "Data task", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);
    vTaskStartScheduler();
    while (true);
}
