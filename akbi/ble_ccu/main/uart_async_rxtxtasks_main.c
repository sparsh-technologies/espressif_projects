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
    uart_set_pin(UART_NUM_2, 17, 16, ECHO_TEST_RTS, ECHO_TEST_CTS);
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

void send_uart_message(const char* p_data, int length)
{
    write(uart_fd, p_data, length);
}

void uart_app_main(void *param)
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






#if 0

/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

static const int RX_BUF_SIZE = 1024;
char   message[20];

#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)
#define BLE_MESSAGE_MAX_SIZE   20

void init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 57600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
}

int sendData(const char* logName)
{
    const int len = strlen(message);
    const int txBytes = uart_write_bytes(UART_NUM_1, message, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

void send_uart_message(const char* i_msg)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    memcpy(message,i_msg,BLE_MESSAGE_MAX_SIZE);
    sendData(TX_TASK_TAG);
}

//static void tx_task(void *arg)
//{
    // static const char *TX_TASK_TAG = "TX_TASK";
    // esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    // while (1) {
    //     sendData(TX_TASK_TAG);
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    // }
//}

static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    //uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    unsigned char data[20] ;
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
        }
    }
    //free(data);
}

void uart_app_main(void)
{
    init();
    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    //xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
}

#endif
