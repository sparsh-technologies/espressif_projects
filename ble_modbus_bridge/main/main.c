/*
 ****************************************************************************************
 * main.c
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 15-November-2019
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2019
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

typedef struct {
    int handle;
    bool finished;
} task_context_t;

task_context_t context1={};
task_context_t context2={};
task_context_t context3={};
task_context_t context4={};

TaskHandle_t ble_task, uart_task, cloud_task;

extern void ble_config_task(void *param);
extern void icom_modbus_task(void *param);
extern void icom_cloud_task(void *param);
extern void icom_init_config_subsys(void);
extern void icom_init_station_cfg(void);

void app_main(void)
{

    printf(" INFO : Starting Main Task \n");

    xTaskCreate(ble_config_task, "ble_config_task", 4*1024, &context1, 0, &ble_task);
    xTaskCreate(icom_modbus_task, "uart_modbus_task", 4*1024, &context2, 0, &uart_task);
    xTaskCreate(icom_cloud_task, "icom_cloud_task", 8*1024, &context3, 0, &cloud_task);

    /*
     * Initialize the configuration sub-system here. After this, init the Wifi sub-system
     */
    icom_init_config_subsys();

    icom_init_station_cfg();

    vTaskDelay(pdMS_TO_TICKS(1000));

    printf(" Sending READY message to other tasks \n");

    while(1) {
#if 0
        printf(" Main Task output \n");
#endif
        vTaskDelay(100);
    }
}
