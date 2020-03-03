/*
 ****************************************************************************************
 * icom_config_storage.c
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 5-Feb-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2019
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"

#define STORAGE_NAMESPACE "mbus_config"

//static nvs_handle_t mbus_cfg_file_handle;
static unsigned int mbus_cfg_file_handle;

int icom_init_mbus_config_file()
{
    int     err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &mbus_cfg_file_handle);

    if (err != ESP_OK)
        return (err);

    return (0);
}

void icom_config_read_task(void *param)
{

    while (1) {
        if (gpio_get_level(GPIO_NUM_0) == 0) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            if(gpio_get_level(GPIO_NUM_0) == 0) {

#if 0
                err = save_run_time();
                if (err != ESP_OK) printf("Error (%s) saving run time blob to NVS!\n", esp_err_to_name(err));
#endif
                printf("Restarting ESP Module...\n");
                fflush(stdout);
                esp_restart();
            }
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
