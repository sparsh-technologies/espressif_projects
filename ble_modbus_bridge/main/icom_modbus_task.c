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
#include "icom_task.h"
#include "modbus.h"

#define MODBUS_TTY_PORT         "/dev/uart/2"

static modbus_t          *p_modbus_rtu_ctx = NULL;
static ICOM_TIMER_HNDL   *p_modbus_rtu_client_timer = NULL;
static unsigned char     modbus_client_connect_flag = 0;
ICOM_MBUS_RT_REG_INFO    mbus_reg_rt_info[MAX_CONFIGURABLE_REGISTERS];
QueueHandle_t            uart_queue;

uart_config_t uart_config = {
    .baud_rate = 38400,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

int icom_modbus_connect_to_client()
{
    if (modbus_connect(p_modbus_rtu_ctx) == -1) {
    
        printf(" ERROR : Cannot connect to the client device \n");
        return (1);
    }

    modbus_client_connect_flag = 1;

    return (0);
}

int icom_modbus_client_connect_timer_callback(void *p_arg)
{
    ICOM_TIMER_HNDL    *p_timer;

    p_timer = (ICOM_TIMER_HNDL *)p_arg;

    /*
     * Try connecting to the client. If it succeeds, then we can delete the timer. If not,
     * this timer gets fired again and again till the connection is opened.
     */

    if (icom_modbus_connect_to_client() == 0) {
        p_timer->repeat_flag = 0;
    }

    return (0);
}

int icom_create_modbus_rtu_poll_timers()
{
    int                  modbus_reg_count, i;
    ICOM_MBUS_REG_INFO   *p_mbus_reg;

    modbus_reg_count = icom_get_configured_modbus_register_count();

    printf(" INFO : Creating MODBUS RTU polling timers \n");

    if (modbus_reg_count > 0) {

        printf(" INFO : Creating %d timers for MODBUS RTU polling \n", modbus_reg_count);

        /*
         * Create timers for each and every configured registers. Also, copy relevent
         * information to run-time-db of MODBUS registers.
         */

        for(i=0; i < modbus_reg_count; i++) {

            p_mbus_reg                      = icom_get_configured_modbus_register(i);
            mbus_reg_rt_info[i].reg_address = p_mbus_reg->reg_address;

            icom_create_modbus_register_poll_timer(p_mbus_reg);
        }

        /*
         * After creating all the timers, now start these timers.
         */
        for(i=0; i<modbus_reg_count; i++) {

            p_mbus_reg = icom_get_configured_modbus_register(i);

            icom_start_modbus_register_poll_timer(p_mbus_reg);
        }

    }

    return (0);
}

/*
 * API to initialize the MODBUS module
 */

int icom_modbus_init()
{

    /*
     * Install UART driver 
     */

    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, 17, 16, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_2, AKBI_UART_BUFFER_SZ, AKBI_UART_BUFFER_SZ, 10, &uart_queue, 0);

    p_modbus_rtu_ctx = modbus_new_rtu(MODBUS_TTY_PORT, 9600, 'N', 8, 1);
    printf(" INFO : MODBUS RTU Context : %p \n", p_modbus_rtu_ctx);

    /*
     * Now try connecting to the client. If the connection fails, then create a timer
     * so that we can retry this again.
     */

    if (icom_modbus_connect_to_client() != 0) {

        p_modbus_rtu_client_timer = icom_create_timer(HEALTH_PING_TIMER_ID, 
                                                      icom_modbus_client_connect_timer_callback);
        if (p_modbus_rtu_client_timer == NULL) {
            printf(" ERROR : Cannot create modbus-client-connect timer \n");
            return (1);
        }

        icom_start_timer(p_modbus_rtu_client_timer, 5, 1);

        return (1);
    }

    return (0);
}

void icom_modbus_task(void *param)
{

    printf(" INFO : Starting ICOM-MODBUS task \n");

    icom_create_task_queue(ICOM_TASK_ID_MODBUS_MGR);

    icom_modbus_init();
    
    while (1) {

        vTaskDelay(100);

    }

    vTaskDelete(NULL);
}


