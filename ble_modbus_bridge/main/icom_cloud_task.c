/*
 ****************************************************************************************
 * icom_cloud_task.c
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

void icom_cloud_task(void *param)
{
    int     s;
    fd_set  rfds;
    struct timeval  tv;

    printf(" INFO : Starting cloud task \n");

    while (1) {

        vTaskDelay(100);

    }

    vTaskDelete(NULL);
}


