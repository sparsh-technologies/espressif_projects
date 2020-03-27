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
#include "icom_ipc.h"
#include "icom_task.h"

void icom_cloud_task(void *param)
{
    ICOM_IPC_MSG      ipc_msg;

    printf(" INFO : Starting cloud task \n");

    icom_create_task_queue(ICOM_TASK_ID_CLOUD_MGR);

    while (1) {

        memset(&ipc_msg, 0x00, sizeof(ICOM_IPC_MSG));
        icom_recv_ipc_buffer(&ipc_msg, ICOM_TASK_ID_CLOUD_MGR);

        switch(ipc_msg.opcode)
        {

        default :
            printf(" INFO : Rcvd unknown IPC opcode in cloud task : %d \n", ipc_msg.opcode);
            break;
        }

        vTaskDelay(100);

    }

    vTaskDelete(NULL);
}


