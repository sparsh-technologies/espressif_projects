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
#include "modbus-private.h"

#define ICOM_UART_BUFFER_SZ                 (2 * 1024)
#define MODBUS_TTY_PORT                     "/dev/uart/2"
#define ICOM_MODBUS_REG_COUNT               1

static modbus_t          *p_modbus_rtu_ctx = NULL;
static ICOM_TIMER_HNDL   *p_modbus_rtu_client_timer = NULL;
static unsigned char     modbus_client_connect_flag = 0;
ICOM_MBUS_RT_REG_INFO    mbus_reg_rt_info[MAX_CONFIGURABLE_REGISTERS];
QueueHandle_t            uart_queue;

uart_config_t modbus_uart_config = {
    .baud_rate = 38400,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

int icom_modbus_connect_to_client()
{
    if (modbus_connect(p_modbus_rtu_ctx) == -1) {
    
        printf(" MODBUS-TASK : ERROR - Cannot connect to the client device \n");
        return (1);
    }

    modbus_client_connect_flag = 1;
    printf(" MODBUS-TASK : Connected to MODBUS-RTU port \n");

    return (0);
}

int icom_modbus_client_connect_timer_callback(void *p_arg)
{
    ICOM_TIMER_HNDL    *p_timer;

    p_timer = (ICOM_TIMER_HNDL *)p_arg;
    printf(" MODBUS-TASK : ERROR - Cannot connect to the client device \n");

    /*
     * Try connecting to the client. If it succeeds, then we can delete the timer. If not,
     * this timer gets fired again and again till the connection is opened.
     */

    if (icom_modbus_connect_to_client() == 0) {
        p_timer->repeat_flag = 0;
    }

    return (0);
}

int icom_modbus_rtu_reg_poll_callback(void *p_arg)
{
    int                    reg_count ;
    ICOM_MBUS_REG_INFO     *p_mbus_conf_reg = NULL;
    ICOM_MBUS_RT_REG_INFO  *p_mbus_rt_reg = NULL;

    printf(" MODBUS-TIMER : Invoked MODBUS polling timer routine \n");

    p_mbus_rt_reg = (ICOM_MBUS_RT_REG_INFO *)p_arg;
    if (p_mbus_rt_reg == NULL) {
        printf(" ERROR : Cannot find a valid RT info \n");
        return (1);
    }

    p_mbus_conf_reg = p_mbus_rt_reg->p_mbus_conf_reg;
    if (p_mbus_conf_reg == NULL) {
        printf(" ERROR : Cannot find a valid CFG info \n");
        return (2);
    }

    /*
     * Now based on what type of register it is, we need to read the data from the 
     * MODBUS client.
     */
#if 1
    switch(p_mbus_conf_reg->reg_type) {

    case ICOM_MBUS_COIL_STATUS_REG :
        printf(" INFO : Performing COIL Read \n");
        reg_count = modbus_read_bits(p_modbus_rtu_ctx, 
                                     p_mbus_conf_reg->reg_address, 
                                     ICOM_MODBUS_REG_COUNT, 
                                     &(p_mbus_rt_reg->reg_bits));

        if (reg_count != ICOM_MODBUS_REG_COUNT) {
            printf(" ERROR : Failed to read COIL Registers \n");
        }

    case ICOM_MBUS_INPUT_STATUS_REG :
        printf(" INFO : Performing STATUS Read \n");
        reg_count = modbus_read_input_bits(p_modbus_rtu_ctx, 
                                           p_mbus_conf_reg->reg_address, 
                                           ICOM_MODBUS_REG_COUNT, 
                                           &(p_mbus_rt_reg->reg_bits));

        if (reg_count != ICOM_MODBUS_REG_COUNT) {
            printf(" ERROR : Failed to read STATUS Registers \n");
        }

    case ICOM_MBUS_HOLDING_REG :
        printf(" INFO : Performing HOLDING Read \n");
        reg_count = modbus_read_registers(p_modbus_rtu_ctx, p_mbus_conf_reg->reg_address, 
                                          ICOM_MODBUS_REG_COUNT, 
                                          &(p_mbus_rt_reg->reg_value));

        if (reg_count != ICOM_MODBUS_REG_COUNT) {
            printf(" ERROR : Failed to read HOLDING Registers \n");
        }
        break;

    case ICOM_MBUS_INPUT_REG :
        printf(" INFO : Performing INPUT Read \n");
        reg_count = modbus_read_input_registers(p_modbus_rtu_ctx, 
                                                p_mbus_conf_reg->reg_address, 
                                                ICOM_MODBUS_REG_COUNT, 
                                                &(p_mbus_rt_reg->reg_value));

        if (reg_count != ICOM_MODBUS_REG_COUNT) {
            printf(" ERROR : Failed to read INPUT Registers \n");
        }
        break;
    }
#endif
    return (0);
}

int icom_create_modbus_rtu_poll_timers()
{
    int                  modbus_reg_count, i;
    ICOM_MBUS_REG_INFO   *p_mbus_reg;

    modbus_reg_count = icom_get_configured_modbus_register_count();

    printf(" MODBUS-TASK :  Creating MODBUS RTU polling timers \n");

    if (modbus_reg_count > 0) {

        printf(" INFO : Creating %d timers for MODBUS RTU polling \n", modbus_reg_count);

        /*
         * Create timers for each and every configured registers. Also, copy relevent
         * information to run-time-db of MODBUS registers.
         */

        for(i=0; i < modbus_reg_count; i++) {

            p_mbus_reg                          = icom_get_configured_modbus_register(i);
            mbus_reg_rt_info[i].reg_address     = p_mbus_reg->reg_address;
            mbus_reg_rt_info[i].p_mbus_conf_reg = p_mbus_reg;
            mbus_reg_rt_info[i].timer_handle    = icom_create_timer(HEALTH_PING_TIMER_ID, 
                                                                    icom_modbus_rtu_reg_poll_callback,
                                                                    &mbus_reg_rt_info[i]);

            /*
             * If the timer has been registered successfully, then start the timer now.
             */
            if (mbus_reg_rt_info[i].timer_handle != NULL) {

                icom_start_timer(mbus_reg_rt_info[i].timer_handle, 
                                 p_mbus_reg->polling_freq_sec, 1);

            }
        }

    }

    return (0);
}

int icom_open_modbus_port(modbus_t *p_ctx)
{
    uart_param_config(UART_NUM_2, &modbus_uart_config);
    uart_set_pin(UART_NUM_2, 17, 16, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_2, ICOM_UART_BUFFER_SZ, ICOM_UART_BUFFER_SZ, 10, &uart_queue, 0);

    if ((p_ctx->s = open(MODBUS_TTY_PORT, O_RDWR | O_NONBLOCK)) == -1) {
        printf(" ERROR : Cannot open UART1");
        return (1);
    }

    printf(" MODBUS-TASK : Opened serial port successfully. FD(%d) \n", p_ctx->s);
    esp_vfs_dev_uart_use_driver(2);
    return (0);
}

/*
 * API to initialize the MODBUS module
 */

int icom_modbus_init()
{

    p_modbus_rtu_ctx = modbus_new_rtu(MODBUS_TTY_PORT, 9600, 'N', 8, 1);
    printf(" MODBUS-TASK : MODBUS RTU Context : %p \n", p_modbus_rtu_ctx);

    /*
     * Now try connecting to the client. If the connection fails, then create a timer
     * so that we can retry this again.
     */

    if (icom_modbus_connect_to_client() != 0) {

        p_modbus_rtu_client_timer = icom_create_timer(HEALTH_PING_TIMER_ID, 
                                                      icom_modbus_client_connect_timer_callback,
                                                      NULL);
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

    printf(" MODBUS-TASK :  Starting ICOM-MODBUS task \n");

    icom_create_task_queue(ICOM_TASK_ID_MODBUS_MGR);

    icom_modbus_init();
    
    icom_create_modbus_rtu_poll_timers();

    while (1) {

        vTaskDelay(100);

    }

    vTaskDelete(NULL);
}


