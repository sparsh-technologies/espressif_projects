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
#include "akbi_ccu_msg_handler.h"
#include "esp_timer.h"

#define PKT_DUMP

#define AKBI_UART_BUFFER_SZ     ( 2 * 1024)

typedef struct {
    int handle;
    bool finished;
} task_context_t;

int uart_fd = -1;
static int flag_set_ret_ptr = 0;
char serial_rx_data[500];
char serial_tx_data[100];
esp_timer_handle_t oneshot_timer = NULL;
static int serial_timer_state = -1;
static int serial_data_bytes = 0;
static const char* TAG = "UART-DRIVER";
task_context_t context1;
TaskHandle_t uart_task;
static void oneshot_timer_callback(void* arg);
QueueHandle_t uart_queue;

uart_config_t uart_config = {
    .baud_rate = 19200,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

const esp_timer_create_args_t oneshot_timer_args = {
    .callback = &oneshot_timer_callback,
    /* argument specified here will be passed to timer callback function */
    .name = "one-shot"
};

void send_uart_message_new(void);

static void oneshot_timer_callback(void* arg)
{

    ESP_LOGI(TAG, " Serial Timer Expired Bytes: %d ", serial_data_bytes);

    esp_log_buffer_hex(TAG, serial_rx_data, serial_data_bytes);
    serial_rx_data[++serial_data_bytes] = '\0';
    akbi_process_rx_serial_data(serial_rx_data, serial_data_bytes-1);
    serial_data_bytes = 0;
    serial_timer_state = -1;
}

static void serial_port_timer_init()
{
    printf(" INFO : Creating Timer \n");

    ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args, &oneshot_timer));
}

static void serial_port_timer_start()
{
    ESP_ERROR_CHECK(esp_timer_start_once(oneshot_timer, 100000));
    serial_timer_state = 1;
}

static void serial_port_timer_stop()
{
    ESP_ERROR_CHECK(esp_timer_stop(oneshot_timer));
}


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
    uart_driver_install(UART_NUM_2, AKBI_UART_BUFFER_SZ, AKBI_UART_BUFFER_SZ, 10, &uart_queue, 0);

    printf(" INFO : Opening Serial Port 2 \n");

    if ((uart_fd = open("/dev/uart/2", O_RDWR | O_NONBLOCK)) == -1) {
        printf(" ERROR : Cannot open UART1");
        deinit_uart();
        return (1);
    }

    printf(" Opened serial port successfully. FD(%d) \n", uart_fd);
    esp_vfs_dev_uart_use_driver(2);

    serial_port_timer_init();
    return (0);
}

static void check_and_uart_data(int fd, const fd_set *rfds, const char *src_msg)
{
    int read_bytes;
    unsigned char    data_byte;

    if (FD_ISSET(fd, rfds)) {

        if ((read_bytes = read(fd, &data_byte, 1)) > 0) {

            /*
             * Once we get the first byte, start the one-shot timer. If the timer
             * gets fired, then it marks the end of packets.
             */


            if (serial_timer_state != 1) {

                /*
                 * If the timer is not started, then start the timer now.
                 */
                serial_rx_data[serial_data_bytes++] = data_byte;
                serial_port_timer_start();
                serial_timer_state = 1;
            } else {
                serial_port_timer_stop();
                serial_rx_data[serial_data_bytes++] = data_byte;
                serial_port_timer_start();
            }

        } else {
            printf(" ERROR : %s read error", src_msg);
        }
    }
}

int akbi_dump_serial_pkt(const char *buffer, int length)
{

#ifdef PKT_DUMP

    int    ret;
    int    i, j, row_cnt, size;
    unsigned char  *p_buffer;

    p_buffer = buffer;
    row_cnt = length/16;
    size    = length;
    j       = 0;

    printf("\n Dumping Packet(Tx) :\n");
    printf(" --------------------\n");

    while(size) {

        if (size < 16 ) {

            for (i=0;i<size;i++) {
                printf(" %02x", p_buffer[j]);
                j++;
            }
            printf("\n");
            size = 0;

        } else {

            for (i=0;i<16;i++) {
                printf(" %02x", p_buffer[j]);
                j++;
            }
            printf("\n");
            size -= 16;
        }
    }
#endif

    return (0);
}

void send_uart_message(const char* p_data, int length )
{
    int     ret, i;
    char    data;
	
    memset(serial_tx_data, 0x00, 100);
    memcpy(serial_tx_data, p_data, length);

    for(i=0; i < length; i++) {
        data =  serial_tx_data[i];
        uart_write_bytes(UART_NUM_2, &data, 1);
    }
	
}

void akbi_uart_thread(void *param)
{
    int     s;
    fd_set  rfds;
    struct  timeval tv;

    if(init_uart() != 0) {
        printf(" ERROR : Unable to open serial port \n");
        return;
    }

    while (1) {

        tv.tv_sec = 15,
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

void create_uart_task(void *param)
{
    xTaskCreate(akbi_uart_thread, "akbi_uart_thread", 4*1024, &context1, 0, &uart_task);
}
