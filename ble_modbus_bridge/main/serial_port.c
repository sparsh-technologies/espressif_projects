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
#include "peripheral.h"

int uart_fd = -1;

uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

static void deinit_uart()
{
    close(uart_fd);
    uart_fd = -1;
    uart_driver_delete(UART_NUM_2);
}

static int init_uart()
{

    /*
     * Initialize the UART port & open the device here.
     */

    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, ECHO_TEST_TXD, ECHO_TEST_RXD, 
                 ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_2, 256, 0, 0, NULL, 0);

    printf(" INFO : Opening Serial Port 2 \n");

    if ((uart_fd = open("/dev/uart/2", O_RDWR | O_NONBLOCK)) == -1) {
        printf(" ERROR : Cannot open UART1");
        deinit_uart();
        return (1);
    }

    printf(" Opened serial port successfully. FD(%d) \n", uart_fd);
    esp_vfs_dev_uart_use_driver(2);
    return (0);
}

static void check_and_uart_data(int fd, const fd_set *rfds, const char *src_msg)
{
    char buf[100];
    int read_bytes;

    if (FD_ISSET(fd, rfds)) {
        if ((read_bytes = read(fd, buf, sizeof(buf)-1)) > 0) {
            buf[read_bytes] = '\0';
            printf( " INFO : %d bytes were received through %s: %s", read_bytes, src_msg, buf);
        } else {
            printf(" ERROR : %s read error", src_msg);
        }
    }
}

void uart_modbus_task(void *param)
{
    int     s;
    fd_set  rfds;
    struct timeval tv;

    if(init_uart() != 0) {
        printf(" ERROR : Unable to open serial port \n");
        return;
    }

    while (1) {

        tv.tv_sec = 5,
        tv.tv_usec = 0,

        FD_ZERO(&rfds);
        FD_SET(uart_fd, &rfds);

        s = select((uart_fd + 1), &rfds, NULL, NULL, &tv);

        if (s < 0) {
            printf(" Select failed. \n");
        } else {
            check_and_uart_data(uart_fd, &rfds, "UART2");
        }
    }

    deinit_uart(uart_fd);
    vTaskDelete(NULL);
}


