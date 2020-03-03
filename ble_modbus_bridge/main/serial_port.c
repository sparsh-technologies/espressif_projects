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
#include "serial_port.h"

int uart_fd = -1;
ICOM_SERIAL_PORT    icom_rs485_port;
esp_timer_handle_t oneshot_timer = NULL;

uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

/*
 * Forward declerations of static functions.
 */
static void oneshot_timer_callback(void* arg);

const esp_timer_create_args_t oneshot_timer_args = {
    .callback = &oneshot_timer_callback,
    .name = "one-shot"
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

static void oneshot_timer_callback(void* arg)
{
    int    serial_data_bytes;

    serial_data_bytes = icom_rs485_port.serial_data_bytes;

#ifdef DEBUG_ENABLE
    ESP_LOGI(TAG, " Serial Timer Expired Bytes: %d ", serial_data_bytes);
    esp_log_buffer_hex(TAG, serial_rx_data, serial_data_bytes);
#endif
    icom_rs485_port.serial_rx_data[++serial_data_bytes] = '\0';
    icom_rs485_port.serial_data_bytes   = serial_data_bytes;
    icom_rs485_port.serial_data_bytes   = 0;
    icom_rs485_port.serial_timer_state  = -1;
}

static void serial_port_timer_start()
{
    ESP_ERROR_CHECK(esp_timer_start_once(oneshot_timer, 100000));
    icom_rs485_port.serial_timer_state = 1;
}

static void serial_port_timer_stop()
{
    ESP_ERROR_CHECK(esp_timer_stop(oneshot_timer));
}

static void read_data_from_rs485_port(int fd, const fd_set *rfds, const char *src_msg)
{
    int              read_bytes;
    unsigned char    data_byte;
    int              serial_data_bytes;

    if (FD_ISSET(fd, rfds)) {

        if ((read_bytes = read(fd, &data_byte, 1)) > 0) {

            /*
             * Once we get the first byte, start the one-shot timer. If the timer
             * gets fired, then it marks the end of packets.
             */
            serial_data_bytes = icom_rs485_port.serial_data_bytes;

            if (icom_rs485_port.serial_timer_state != 1) {

                /*
                 * If the timer is not started, then start the timer now.
                 */
                icom_rs485_port.serial_rx_data[serial_data_bytes++] = data_byte;
                icom_rs485_port.serial_data_bytes = serial_data_bytes;
                serial_port_timer_start();
                icom_rs485_port.serial_timer_state = 1;
            } else {
                serial_port_timer_stop();
                icom_rs485_port.serial_rx_data[serial_data_bytes++] = data_byte;
                icom_rs485_port.serial_data_bytes = serial_data_bytes;
                serial_port_timer_start();
            }

        } else {
            printf(" ERROR : %s read error", src_msg);
        }
    }

}

static void configure_rs485_enable_line(void)
{
    gpio_config_t io_conf;

    memset(&icom_rs485_port, 0x00, sizeof(ICOM_SERIAL_PORT));

    /*
     * Configure the RS485 lines TX-Enable line here
     */

    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;

    gpio_config(&io_conf);

    ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args, &oneshot_timer));
}

void icom_enable_rs485_tx()
{
    gpio_set_level(GPIO_OUTPUT_IO_23, 1);
}

void icom_disable_rs485_tx()
{
    gpio_set_level(GPIO_OUTPUT_IO_23, 0);
}

int icom_send_rs485_data(char *p_data, int length)
{
    uart_write_bytes(UART_NUM_2, p_data, length);
    return (0);
}

void uart_modbus_task(void *param)
{
    int     s;
    fd_set  rfds;
    struct timeval  tv;

    if(init_uart() != 0) {
        printf(" ERROR : Unable to open serial port \n");
        return;
    }

    configure_rs485_enable_line();

    while (1) {

        tv.tv_sec = 5,
        tv.tv_usec = 0,

        FD_ZERO(&rfds);
        FD_SET(uart_fd, &rfds);

        s = select((uart_fd + 1), &rfds, NULL, NULL, &tv);

        if (s < 0) {
            printf(" Select failed. \n");
        } else {
            read_data_from_rs485_port(uart_fd, &rfds, "UART2");
        }
//        vTaskDelay(100);

    }

    deinit_uart(uart_fd);
    vTaskDelete(NULL);
}


