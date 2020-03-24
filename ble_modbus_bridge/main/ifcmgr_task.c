/*
 ****************************************************************************************
 * serial_port.c
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
#include "include/peripheral.h"
#include "include/serial_port.h"

extern unsigned char  dio_port;

void icom_modbus_task(void *param)
{
    int     current_state;

    printf(" INFO : Starting IfcMgr task \n");

    while (1) {

        if (dio_port == 1) {
            printf(" INFO : Setting to high \r\n");
            icom_enable_rs485_tx();
        } else if(dio_port == 0){
            printf(" INFO : Setting to low \r\n");
            icom_disable_rs485_tx();
		}
        vTaskDelay(200);

    }

    vTaskDelete(NULL);
}


