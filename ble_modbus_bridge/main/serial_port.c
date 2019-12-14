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

static void uart1_deinit()
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
        uart1_deinit();
    }

    esp_vfs_dev_uart_use_driver(1);
}

void uart_modbus_task(void *param)
{
    char buf[20];

    init_uart();

    for (uint8_t i = 1;; ++i) 
    {

        vTaskDelay(4000 / portTICK_PERIOD_MS);

        snprintf(buf, sizeof(buf), "UART message U%d", i);
        int write_bytes = write(uart_fd, buf, strlen(buf));
        if (write_bytes < 0) {
            ESP_LOGE(TAG, "UART1 write error");
        } else {
            ESP_LOGI(TAG, "%d bytes were sent to UART1: %s", write_bytes, buf);
        }
    }

    uart1_deinit(uart_fd);
    vTaskDelete(NULL);
}


