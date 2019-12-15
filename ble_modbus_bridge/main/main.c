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
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/param.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_vfs.h"

extern void ble_config_task(void *param);
extern void uart_modbus_task(void *param);

/*
 * This is the first function that gets invoked as part of ESP32 initialization.
 */
void app_main()
{
    xTaskCreate(uart_modbus_task, "uart_modbus_task", 4*1024, NULL, 5, NULL);
    xTaskCreate(ble_config_task, "ble_config_task", 4*1024, NULL, 5, NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
