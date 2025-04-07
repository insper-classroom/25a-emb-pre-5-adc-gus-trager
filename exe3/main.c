#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

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
    int total_samples = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    int total_averages = total_samples - 5 + 1;
    int averages_printed = 0;

    int buffer[5] = {0};
    int idx = 0;  
    int sum = 0;   
    int count = 0; 

    int data = 0;
    while (averages_printed < total_averages) {
        if (xQueueReceive(xQueueData, &data, pdMS_TO_TICKS(100))) {
            if (count < 5) {
                buffer[idx] = data;
                sum += data;
                idx = (idx + 1) % 5;
                count++;
                if (count == 5) { 
                    int media = sum / 5;
                    printf("%d\n", media);
                    averages_printed++;
                }
            }
            else {
                sum -= buffer[idx];
                buffer[idx] = data;
                sum += data;
                idx = (idx + 1) % 5;
                int media = sum / 5;
                printf("%d\n", media);
                averages_printed++;
            }
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
    vTaskDelete(NULL);
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
