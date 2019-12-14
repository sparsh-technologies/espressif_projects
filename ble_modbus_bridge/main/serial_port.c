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

static const char* TAG = "uart_select_example";

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
    UART1.conf0.loopback = 0;
}

static void init_uart()
{
    uart_param_config(UART_NUM_2, &uart_config);
    uart_driver_install(UART_NUM_2, 256, 0, 0, NULL, 0);
    UART1.conf0.loopback = 1;

    printf(" INFO : Opening Serial Port 2 \n");
    if ((uart_fd = open("/dev/uart/2", O_RDWR | O_NONBLOCK)) == -1) {
        ESP_LOGE(TAG, "Cannot open UART1");
        deinit_uart();
    }

    esp_vfs_dev_uart_use_driver(1);
}

static void check_and_uart_data(int fd, const fd_set *rfds, const char *src_msg)
{
    char buf[100];
    int read_bytes;

    if (FD_ISSET(fd, rfds)) {
        if ((read_bytes = read(fd, buf, sizeof(buf)-1)) > 0) {
            buf[read_bytes] = '\0';
            ESP_LOGI(TAG, "%d bytes were received through %s: %s", read_bytes, src_msg, buf);
        } else {
            ESP_LOGE(TAG, "%s read error", src_msg);
        }
    }
}

void uart_modbus_task(void *param)
{
    char    buf[20];
    int     s;
    fd_set  rfds;
    struct timeval tv;

    init_uart();

    while (1) {

        tv.tv_sec = 1,
        tv.tv_usec = 0,

        FD_ZERO(&rfds);
        FD_SET(uart_fd, &rfds);

        s = select((uart_fd + 1), &rfds, NULL, NULL, &tv);

        if (s < 0) {
            ESP_LOGE(TAG, "Select failed: errno %d", errno);
        } else if (s == 0) {
            ESP_LOGI(TAG, "Timeout has been reached and nothing has been received");
        } else {
            check_and_uart_data(socket_fd, &rfds, "socket");
            check_and_uart_data(uart_fd, &rfds, "UART1");
        }
    }

    deinit_uart(uart_fd);
    vTaskDelete(NULL);
}


