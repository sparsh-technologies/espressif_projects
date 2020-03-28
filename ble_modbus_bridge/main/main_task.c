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
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "icom_ipc.h"
#include "icom_gpio.h"
#include "icom_task.h"
#include "icom_timer_api.h"

typedef struct {
    int handle;
    bool finished;
} task_context_t;

task_context_t context1={};
task_context_t context2={};
task_context_t context3={};
task_context_t context4={};

TaskHandle_t ble_task, uart_task, cloud_task, modbus_task;

static ICOM_TIMER_HNDL *p_main_task_timer = NULL;

extern void ble_config_task(void *param);
extern void icom_serial_task(void *param);
extern void icom_cloud_task(void *param);
extern void icom_modbus_task(void *param);

extern void icom_init_config_subsys(void);
extern void icom_init_station_cfg(void);

int icom_healthping_timer_callback(void *p_arg)
{
    ICOM_IPC_MSG    ipc_msg;

    printf(" INFO : PING Timer fired \n");

    memset(&ipc_msg, 0x00, sizeof(ICOM_IPC_MSG));
    ipc_msg.opcode = IPC_OPCODE_PING;
    icom_send_ipc_buffer(ICOM_TASK_ID_CLOUD_MGR , &ipc_msg);

    return (0);
}

int icom_main_task_init()
{
    p_main_task_timer = icom_create_timer(HEALTH_PING_TIMER_ID, 
                                          icom_healthping_timer_callback, NULL);
    if (p_main_task_timer == NULL) {
        printf(" ERROR : Cannot create timer in main-task \n");
        return (1);
    }
    
    printf(" INFO : Created and started Ping timer : %p \n", icom_healthping_timer_callback);
    printf(" INFO : Ping timer address : %p \n", &icom_healthping_timer_callback);
    icom_start_timer(p_main_task_timer, 30, 1);

    return (0);
}

void app_main(void)
{

    printf(" INFO : Starting Main Task \n");

    icom_create_task_queue(ICOM_TASK_ID_MAIN);

    icom_main_task_init();

    /*
     * Initializing IPC subsystem
     */
    icom_ipc_init();

//    icom_configure_gpio_port();

    /*
     * Initialize the configuration sub-system here. After this, init the Wifi sub-system
     */
    icom_init_config_subsys();

    /*
     * Now create all worker threads here.
     */

    xTaskCreate(ble_config_task, "BLE Task", 4*1024, &context1, 0, &ble_task);
//    xTaskCreate(icom_serial_task, "Serial Port Task", 4*1024, &context2, 0, &uart_task);
    xTaskCreate(icom_cloud_task, "Cloud Task", 8*1024, &context3, 0, &cloud_task);
    xTaskCreate(icom_modbus_task, "MODBUS RTU Task", 4*1024, &context4, 0, &modbus_task);

//    icom_init_station_cfg();

    vTaskDelay(pdMS_TO_TICKS(1000));

    printf(" Sending READY message to other tasks \n");

    while(1) {
#if 0
        printf(" Main Task output \n");
#endif
        vTaskDelay(100);
    }
}
