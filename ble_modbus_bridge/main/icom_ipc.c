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
#include "icom_task.h"

static ICOM_IPC_MSG  *p_ipc_free_pool       = NULL;
static xQueueHandle  main_task_queue        = NULL;
static xQueueHandle  icom_cloud_task_queue       = NULL;
static xQueueHandle  ble_config_task_queue  = NULL;
static xQueueHandle  icom_modbus_task_queue = NULL;
static xQueueHandle  icom_serial_task_queue = NULL;

int icom_ipc_init()
{
    ICOM_IPC_MSG    *p_ipc_msg, *p_prev_ipc_msg;
    unsigned char   *p_payload;
    int             i;

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
    return (0);
}

ICOM_IPC_MSG *icom_alloc_ipc_buffer()
{
    ICOM_IPC_MSG *p_msg;

    if (p_ipc_free_pool == NULL) {
        return (NULL);
    }

    p_msg = p_ipc_free_pool;
    p_ipc_free_pool = p_ipc_free_pool->next;

    return (p_msg);
}

void icom_free_ipc_buffer(ICOM_IPC_MSG *p_msg)
{
    p_msg->next =  p_ipc_free_pool;
    p_ipc_free_pool = p_msg;
}

int icom_send_ipc_buffer(int task_id, ICOM_IPC_MSG *p_msg)
{
    unsigned int    msg_address;

    msg_address = (unsigned int)p_msg;


    if (task_id == ICOM_TASK_ID_MAIN) {

        printf(" INFO : Sending Address: %x to Main task \n", msg_address);
        if (xQueueSend(main_task_queue, &msg_address, 10 / portTICK_RATE_MS) != pdTRUE) {
            printf(" ERROR : xQueue send failed \n");
            return (1);
        }

    } else if (task_id == ICOM_TASK_ID_BLE_MGR ) {

        printf(" INFO : Sending Address: %x to BLE task \n", msg_address);
        if (xQueueSend(ble_config_task_queue, &msg_address, 10 / portTICK_RATE_MS) != pdTRUE) {
            printf(" ERROR : xQueue send failed \n");
            return (1);
        }

    } else if (task_id == ICOM_TASK_ID_MODBUS_MGR ) {

        printf(" INFO : Sending Address: %x to Modbus task \n", msg_address);
        if (xQueueSend(icom_modbus_task_queue, &msg_address, 10 / portTICK_RATE_MS) != pdTRUE) {
            printf(" ERROR : xQueue send failed \n");
            return (1);
        }

    } else if (task_id == ICOM_TASK_ID_CLOUD_MGR ) {

        printf(" INFO : Sending Address: %p to Cloud task \n", p_msg);
        if (xQueueSend(icom_cloud_task_queue, p_msg, 100) != pdTRUE) {
            printf(" ERROR : xQueue send failed \n");
            return (1);
        }

    }
    return (0);
}

int icom_recv_ipc_buffer(ICOM_IPC_MSG *p_ipc_msg, int task_id)
{
    unsigned int    msg_address;
    ICOM_IPC_MSG    ipc_msg;

    if (task_id == ICOM_TASK_ID_MAIN) {

        if (pdPASS != xQueueReceive(main_task_queue, &msg_address,
                                    (portTickType)portMAX_DELAY)) {
            printf(" ERROR : xQueue receive failed in Main task \n");
            return (1);
        }

    } else if (task_id == ICOM_TASK_ID_BLE_MGR ) {

        if (pdPASS != xQueueReceive(ble_config_task_queue, &msg_address,
                                    (portTickType)portMAX_DELAY)) {
            printf(" ERROR : xQueue receive failed in BLE task \n");
            return (1);
        }

    } else if (task_id == ICOM_TASK_ID_MODBUS_MGR ) {

        if (pdPASS != xQueueReceive(icom_modbus_task_queue, &msg_address,
                                    (portTickType)portMAX_DELAY)) {
            printf(" ERROR : xQueue receive failed in Modbus task \n");
            return (1);
        }

    } else if (task_id == ICOM_TASK_ID_CLOUD_MGR ) {

        if (pdPASS != xQueueReceive(icom_cloud_task_queue, p_ipc_msg, 
                                   (portTickType)portMAX_DELAY)) {
            printf(" ERROR : xQueue receive failed in cloud task \n");
            return (1);
        }

    } else if (task_id == ICOM_TASK_ID_SERIAL_MGR ) {

        if (pdPASS != xQueueReceive(icom_serial_task_queue, &msg_address,
                                   (portTickType)portMAX_DELAY)) {
            printf(" ERROR : xQueue receive failed in serial task \n");
            return (1);
        }

    }

    return (0);
}

int icom_create_task_queue(int task_id)
{

    if (task_id == ICOM_TASK_ID_MAIN) {

        main_task_queue = xQueueCreate(10, sizeof(ICOM_IPC_MSG));
        printf(" INFO : Creating Queue : %p for Main task \n", main_task_queue);

    } else if (task_id == ICOM_TASK_ID_BLE_MGR ) {

        ble_config_task_queue = xQueueCreate(10, sizeof(ICOM_IPC_MSG));
        printf(" INFO : Creating Queue : %p for BLE task \n", ble_config_task_queue);

    } else if (task_id == ICOM_TASK_ID_MODBUS_MGR ) {

        icom_modbus_task_queue = xQueueCreate(10, sizeof(ICOM_IPC_MSG));
        printf(" INFO : Creating Queue : %p for Modbus task \n", icom_modbus_task_queue);

    } else if (task_id == ICOM_TASK_ID_CLOUD_MGR ) {

        icom_cloud_task_queue = xQueueCreate(10, sizeof(ICOM_IPC_MSG));
        printf(" INFO : Creating Queue : %p for Cloud task \n", icom_cloud_task_queue);

    } else if (task_id == ICOM_TASK_ID_SERIAL_MGR ) {

        icom_serial_task_queue = xQueueCreate(10, sizeof(ICOM_IPC_MSG));
        printf(" INFO : Creating Queue : %p for Serial task \n", icom_serial_task_queue);

    }

    return (0);
}
