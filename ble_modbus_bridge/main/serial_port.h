/*
 ****************************************************************************************
 * serial_port.h
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 5-Feb-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2019
 *
 ****************************************************************************************
 */

#ifndef _SERIAL_PORT_H_
#define _SERIAL_PORT_H_

#define MAX_TX_BUFFER_SZ                    1024
#define MAX_RX_BUFFER_SZ                    1024

typedef struct _icom_serial_port_
{
    int              port_num;
    int              baud_rate;
    unsigned char    serial_tx_data[MAX_TX_BUFFER_SZ];
    unsigned char    serial_rx_data[MAX_RX_BUFFER_SZ];
    int              serial_data_bytes;
    int              serial_timer_state;

} ICOM_SERIAL_PORT;

#define GPIO_OUTPUT_IO_23                     23
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_23))

extern void icom_enable_rs485_tx(void);
extern void icom_disable_rs485_tx(void);
extern int icom_send_rs485_data(char *p_data, int length);

#endif

