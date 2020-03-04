/*
 ****************************************************************************************
 * icom_ipc.c
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
#include "icom_ipc.h"

static ICOM_IPC_MSG  *p_ipc_free_pool = NULL;

int icom_ipc_init()
{
    ICOM_IPC_MSG    *p_ipc_msg, p_prev_ipc_msg;
    unsigned char   *p_payload;

    p_ipc_msg = (ICOM_IPC_MSG *)malloc (sizeof(ICOM_IPC_MSG) * ICOM_IPC_MAX_MSG_SZ);

    if (p_ipc_msg == NULL) {
        printf(" ERROR : Not enough memory for IPC MSG buffers \n");
        return (1);
    }
  
    /*
     * Chain all the allocated blocks here.
     */
 
    p_ipc_free_pool = p_ipc_msg;
    p_prev_ipc_msg  = p_ipc_msg;
    p_ipc_msg++;
 
    for(i = 0; i < (ICOM_IPC_MAX_MSG_SZ - 1); i++) {

        p_prev_ipc_msg->next = p_ipc_msg;
        p_prev_ipc_msg       = p_ipc_msg;
        p_ipc_msg++;
    }
    p_prev_ipc_msg->next = NULL;

    /*
     * Allocate memory for packet contents here.
     */
    p_payload = (unsigned char *)malloc (ICOM_IPC_MAX_MSG_SZ * ICOM_IPC_MAX_PAYLOAD_SZ);

    if (p_payload == NULL) { 
        printf(" ERROR : Not enough memory for IPC payload buffers \n");
        free(p_ipc_free_pool);
        p_ipc_free_pool = NULL;
        return (1);
    }

    /*
     * Allocate packet buffers into MSG buffers here
     */
    p_ipc_msg = p_ipc_free_pool;

    for(i = 0; i < ICOM_IPC_MAX_MSG_SZ; i++) {

        p_ipc_msg->data_ptr = p_payload;
        p_payload           = p_payload + ICOM_IPC_MAX_PAYLOAD_SZ;
        p_ipc_msg++;
    }

    printf(" INFO : Create MSG and packet buffers successfully \n");

}

ICOM_IPC_MSG *icom_alloc_ipc_buffer()
{

    if (p_ipc_free_pool == NULL) {
        return (NULL);
    }

    

}

void icom_free_ipc_buffer(ICOM_IPC_MSG *p_msg)
{


}
