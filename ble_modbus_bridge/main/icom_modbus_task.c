/*
 ****************************************************************************************
 * icom_modbus_task.c
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
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "tcpip_adapter.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "peripheral.h"
#include "icom_config.h"
#include "icom_timer_api.h"
#include "icom_ipc.h"

int icom_modbus_init(int reg_count)
{
    int    i;
    ICOM_MBUS_REG_INFO   *p_mbus_reg;

    for(i=0; i<reg_count; i++) {

        p_mbus_reg = icom_get_configured_modbus_register(i);

        icom_create_modbus_register_poll_timer(p_mbus_reg);
    }

    /*
     * After creating all the timers, now start these timers.
     */
    for(i=0; i<reg_count; i++) {

        p_mbus_reg = icom_get_configured_modbus_register(i);

        icom_start_modbus_register_poll_timer(p_mbus_reg);
    }

    return (0);
}

void icom_modbus_task(void *param)
{
    int     modbus_reg_count;

    printf(" INFO : Starting ICOM-MODBUS task \n");

    icom_create_task_queue(ICOM_TASK_ID_MODBUS_MGR);

    modbus_reg_count = icom_get_configured_modbus_register_count();
    if (modbus_reg_count > 0) {
        icom_modbus_init(modbus_reg_count);
    }
    
    while (1) {

        vTaskDelay(100);

    }

    vTaskDelete(NULL);
}


