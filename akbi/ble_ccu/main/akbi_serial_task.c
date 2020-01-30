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

int uart_fd = -1;
static char *p_ret_msg;

uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

void set_ret_msg_ptr(char *ret_msg_ptr){
    p_ret_msg = ret_msg_ptr;
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
            memcpy(p_ret_msg,buf,read_bytes);
            printf( " INFO : %d bytes were received through %s: %s", read_bytes, src_msg, buf);

            set_data_to_mobile(buf,read_bytes,p_ret_msg);

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

void send_uart_message(const char* p_data, int length ,char *p_recvd_msg_full )
{
    int    ret;

    set_ret_msg_ptr(p_recvd_msg_full);
    akbi_dump_serial_pkt(p_data, length);
    ret = write(uart_fd, p_data, length);
    printf(" UART-WRITE : Sending %d bytes Status(%d)\n", length, ret);

}

void uart_app_main(void *param)
{
    int     s;
    fd_set  rfds;
    struct  timeval tv;

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
