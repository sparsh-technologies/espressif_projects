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

esp_err_t save_run_time(void)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Read the size of memory space required for blob
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS
    err = nvs_get_blob(my_handle, "run_time", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    // Read previously saved blob if available
    uint32_t* run_time = malloc(required_size + sizeof(uint32_t));
    if (required_size > 0) {
        err = nvs_get_blob(my_handle, "run_time", run_time, &required_size);
        if (err != ESP_OK) {
            free(run_time);
            return err;
        }
    }

    // Write value including previously saved blob if available
    required_size += sizeof(uint32_t);
    run_time[required_size / sizeof(uint32_t) - 1] = xTaskGetTickCount() * portTICK_PERIOD_MS;
    err = nvs_set_blob(my_handle, "run_time", run_time, required_size);
    free(run_time);

    if (err != ESP_OK) return err;

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // Close
    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t print_what_saved(void)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Read restart counter
    int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
    err = nvs_get_i32(my_handle, "restart_conter", &restart_counter);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
    printf("Restart counter = %d\n", restart_counter);

    // Read run time blob
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS
    // obtain required memory space to store blob being read from NVS
    err = nvs_get_blob(my_handle, "run_time", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
    printf("Run time:\n");
    if (required_size == 0) {
        printf("Nothing saved yet!\n");
    } else {
        uint32_t* run_time = malloc(required_size);
        err = nvs_get_blob(my_handle, "run_time", run_time, &required_size);
        if (err != ESP_OK) {
            free(run_time);
            return err;
        }
        for (int i = 0; i < required_size / sizeof(uint32_t); i++) {
            printf("%d: %d\n", i + 1, run_time[i]);
        }
        free(run_time);
    }

    // Close
    nvs_close(my_handle);
    return ESP_OK;
}

void icom_config_read_task(void *param)
{

    gpio_pad_select_gpio(GPIO_NUM_0);
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_DEF_INPUT);

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
